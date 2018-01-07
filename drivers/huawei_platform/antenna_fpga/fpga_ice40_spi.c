/*
 * Copyright (C) 2013 HUAWEI, Inc.
 * Author: mouzhenguo<mike.mou@huawei.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/spi/spi.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/ctype.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/vmalloc.h>
#include <linux/random.h>
#include <linux/irq.h>
#include <linux/amba/pl022.h>
#include <linux/pinctrl/consumer.h>
#include <huawei_platform/log/hw_log.h>
#include "fpga_ice40.h"

#define HWLOG_TAG antenna_fpga
HWLOG_REGIST();
#define DRIVER_VERSION        "1.0.0"
#define FPGA_ICE40_SPI_SPEED_BOOT    (10000*1000)
#define FPGA_ICE40_SPI_SPEED_NORMAL    (1000*1000)
#define BIT_PER_WORD 8

typedef struct fpga_spi_data_ {
     int spi_cs_gpio;
     u8    *rx_buf;
     /* spi master config and spi device*/
    struct spi_device    *spi;
    struct pl022_config_chip spidev0_chip_info;
}fpga_spi_data;
/*This struct record the spi infomation.*/
static fpga_spi_data *spi_drv_data;

/*
 * local functions, no need to export to other driver modules.
 * include :
 * @lowlevel spi transfer
 */
static fpga_spi_data* get_spi_data(void)
{
    if (likely(spi_drv_data))
        return spi_drv_data;
    else
        return NULL;
}

/*
*This function will be as a callback when set thc cs.
* It will be registered to the low level.
*/
int ice40_cs_set(u32 control)
{
    int ret;
    fpga_spi_data *spidev = get_spi_data();
    if(!spidev) {
        hwlog_err("%s - spidev is NULL.\n",__func__);
        return FAIL;
    }
    hwlog_info("%s: control = %d spi_cs_gpio = %d\n", __func__, control, spidev->spi_cs_gpio);

    if (SSP_CHIP_SELECT == control) {
        /*cs steup time at least 10ns */
        ndelay(10);
        ret = gpio_direction_output(spidev->spi_cs_gpio, control);
    } else {
        /*cs hold time at least 4*40ns(@25MHz) */
        ndelay(10);
        ret = gpio_direction_output(spidev->spi_cs_gpio, control);
        ndelay(200);
    }

    if (ret < 0) {
        hwlog_err("%s: fail to set gpio cs, result = %d.\n", __func__, ret);
        ret = FAIL;
    } else {
        ret = SUCCESS;
    }
    return ret;
}
EXPORT_SYMBOL(ice40_cs_set);

int ice40_set_cs_callback(int value, int cs_gpio)
{
    fpga_spi_data *spidev = get_spi_data();
    int ret;
    if(!spidev) {
        hwlog_err("%s - spidev is NULL.\n",__func__);
        return FAIL;
    }
    spidev->spi_cs_gpio =  cs_gpio;
    if(ENABLE == value) {
        spidev->spi->mode = SPI_MODE_3;
        spidev->spi->bits_per_word = 8;
        spidev->spi->controller_data = &spidev->spidev0_chip_info;
        spidev->spi->max_speed_hz = FPGA_ICE40_SPI_SPEED_NORMAL;
        ret = spi_setup(spidev->spi);
        if (ret<0) {
            hwlog_err("%s %d: spi_setup fail!\n", __func__, __LINE__);
            return FAIL;
        }
    }else if(DISABLE == value) {
        spidev->spi->mode = SPI_MODE_3;
        spidev->spi->bits_per_word = 8;
        spidev->spi->controller_data = NULL;
        spidev->spi->max_speed_hz = FPGA_ICE40_SPI_SPEED_BOOT;
        ret = spi_setup(spidev->spi);
        if (ret<0) {
            hwlog_err("%s %d: spi_setup fail!\n", __func__, __LINE__);
            return FAIL;
        }
    }else {
        hwlog_err("%s %d:invilade value!\n", __func__, __LINE__);
    }
    return SUCCESS;
}
EXPORT_SYMBOL(ice40_set_cs_callback);

/*
*Frame:  SPI slave address + wr/rd + register address + data
*Bits count:     7             +    1    +       16             + 48(7or8 GPIO + 5 x 8 ), or 24(70r8 GPIO + 2 x 8, because only register 0x03 and 0x04change)
*
*
*/
int ice40_data_spi_write_then_read(u8 *indata, u32 indata_size, u8* outdata, u32 outdata_size)
{
    fpga_spi_data *spidev = NULL;
    int ret = 0;
    struct spi_message  m;
    struct spi_transfer	t = {
        .tx_buf = indata,
        .rx_buf = outdata,
        .len = indata_size,
    };
    hwlog_info("%s: write indata = %s.\n", __func__, indata);

    if (!indata || !outdata) {
        hwlog_err("%s - invalid arg data!\n",__func__);
        return FAIL;
    }
    spidev = get_spi_data();
    if(!spidev) {
        hwlog_err("%s - spidev is NULL.\n",__func__);
        return FAIL;
    }
    spi_message_init(&m);
    spi_message_add_tail(&t, &m);
    ret = spi_sync(spidev->spi, &m);
    if (ret < 0) {
        hwlog_err("%s:ice40_data_spi_recv fail , result = %d.\n", __func__, ret);
        return FAIL;
    }
    return SUCCESS;
}
EXPORT_SYMBOL(ice40_data_spi_write_then_read);

int ice40_data_spi_write(u8 *data, u32 data_size)
{

    int ret;
    fpga_spi_data *spidev = NULL;
    if (!data) {
        hwlog_err("%s - invalid arg buf=%p\n",__func__, data);
        return FAIL;
    }
    spidev = get_spi_data();
    if(!spidev) {
        hwlog_err("%s - spidev is NULL.\n",__func__);
    }
    hwlog_err("%s - ice40_data_spi_write data_size %d\n",__func__, data_size);
    ret = spi_write(spidev->spi, data, data_size);
    hwlog_err("%s spi_write %d\n", __func__, ret);
    if (ret < 0) {
        hwlog_err("%s spi send error %d\n", __func__, ret);
        return FAIL;
    }
    return SUCCESS;

}
EXPORT_SYMBOL(ice40_data_spi_write);

int ice40_data_spi_recv(u8 *data, u32 data_size)
{
     int ret;
    fpga_spi_data *spidev = NULL;
    if (!data) {
        hwlog_err("%s - invalid arg buf=%p\n",__func__, data);
        return FAIL;
    }

    spidev = get_spi_data();
    if(!spidev) {
        hwlog_err("%s - spidev is NULL.\n",__func__);
    }
    ret = spi_read(spidev->spi, data, data_size);
    if (ret < 0) {
        hwlog_err("%s spi send error %d\n", __func__, ret);
        return FAIL;
    }
    return SUCCESS;
}
EXPORT_SYMBOL(ice40_data_spi_recv);

int ice40_spi_exit()
{
    fpga_spi_data *spidev = NULL;
    spidev = get_spi_data();
    if(!spidev) {
        kfree(spidev);
    }
    return 0;
}
EXPORT_SYMBOL(ice40_spi_exit);

static int fpga_ice40_spi_resume(struct spi_device *spi)
{
   return 0;
}

static int fpga_ice40_spi_suspend(struct spi_device *spi, pm_message_t mesg)
{
    return 0;
}

static int fpga_ice40_spi_remove(struct spi_device *spi)
{
    return 0;
}
/*
*First, the spi config from dts.
*Second, set the cs control callback, the callback will send to the spi driver.
*/
int fpga_spi_get_dt_data(struct device *dev, fpga_spi_data *plat_data)
{
    struct device_node *np;
    np = of_find_compatible_node(NULL, NULL, "fpga_spi");

    plat_data->spidev0_chip_info.hierarchy = 0;
    of_property_read_u32(np, "pl022,interface", &plat_data->spidev0_chip_info.iface);
    of_property_read_u32(np, "pl022,com-mode", &plat_data->spidev0_chip_info.com_mode);
    of_property_read_u32(np, "pl022,rx-level-trig", &plat_data->spidev0_chip_info.rx_lev_trig);
    of_property_read_u32(np, "pl022,tx-level-trig", &plat_data->spidev0_chip_info.tx_lev_trig);
    of_property_read_u32(np, "pl022,ctrl-len", &plat_data->spidev0_chip_info.ctrl_len);
    of_property_read_u32(np, "pl022,wait-state", &plat_data->spidev0_chip_info.wait_state);
    of_property_read_u32(np, "pl022,duplex", &plat_data->spidev0_chip_info.duplex);
    plat_data->spidev0_chip_info.cs_control = ice40_cs_set;
    return SUCCESS;
}

static int fpga_ice40_spi_probe(struct spi_device *spi)
{
    int ret = 0;
    fpga_spi_data *drv_data = NULL;

    /* step 1: alloc driver data struct */
    drv_data = kmalloc(sizeof(fpga_spi_data), GFP_KERNEL);
    if (!drv_data) {
        hwlog_err("%s %d: alloc data errorl!\n", __func__, __LINE__);
        return FAIL;
    }
    memset(drv_data, 0, sizeof(fpga_spi_data));
    /*Get the dts data to config the spi dev.*/
    ret = fpga_spi_get_dt_data(&spi->dev, drv_data);
    if(ret) {
        hwlog_err("%s %d: get dt data fail!\n", __func__, __LINE__);
        goto err_get_dt_data;
    }
    /* step 2: init driver data */
    drv_data->spi = spi;
    /* step 3: setup spi */
    spi->mode = SPI_MODE_3;
    spi->bits_per_word = BIT_PER_WORD;
    spi->controller_data = &drv_data->spidev0_chip_info;
    spi->max_speed_hz = FPGA_ICE40_SPI_SPEED_BOOT;
    ret = spi_setup(spi);
    if (ret<0) {
        hwlog_err("%s %d: spi_setup fail!\n", __func__, __LINE__);
        goto err_spi_setup;
    }
    /* step 4:set drv data */
    spi_set_drvdata(spi, drv_data);
    spi_drv_data = drv_data;
    /*Finally, return success.*/
    hwlog_info("fpga_ice40_spi_probe success");
    return SUCCESS;
err_get_dt_data:
err_spi_setup:
    kfree(drv_data);
    spi_drv_data = NULL;
    return FAIL;
}

static const struct of_device_id fpga_ice40_spi_ids[] = {
    { .compatible = "fpga_spi" },
    {
    },
};

MODULE_DEVICE_TABLE(of, fpga_ice40_spi_ids);

static struct spi_driver fpga_ice40_spi_driver = {
    .driver = {
        .name = "fpga_spi",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(fpga_ice40_spi_ids),
    },
    .probe = fpga_ice40_spi_probe,
    .remove = fpga_ice40_spi_remove,
    .suspend = fpga_ice40_spi_suspend,
    .resume = fpga_ice40_spi_resume,
};

static int __init fpga_ice40_spi_init()
{
    int ret;
    ret = spi_register_driver(&fpga_ice40_spi_driver);
    if (ret) {
        hwlog_err("%s %d: spi_register_driver failed.\n", __func__, __LINE__);
    }
    else{
        hwlog_info("%s %d: spi driver is initialized successfully!\n", __func__, __LINE__);
    }
    return ret;
}

static void __exit fpga_ice40_spi_exit(void)
{
    spi_unregister_driver(&fpga_ice40_spi_driver);
}

module_init(fpga_ice40_spi_init);
module_exit(fpga_ice40_spi_exit);
MODULE_LICENSE("Huawei/GPL");
