/*
 *
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
#include <linux/irqreturn.h>
#include <linux/delay.h>
#include <asm/segment.h>
#include <linux/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/ctype.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/vmalloc.h>
#include <linux/random.h>
#include <linux/irq.h>
#include <linux/amba/pl022.h>
#include <huawei_platform/log/hw_log.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/wakelock.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/wakelock.h>
#include <linux/workqueue.h>
#include <asm/atomic.h>
#include <linux/of_gpio.h>
#include <huawei_platform/dsm/dsm_pub.h>
#include "fpga_ice40.h"
#include <linux/kernel.h>

#define HWLOG_TAG antenna_fpga
HWLOG_REGIST();
#define DRIVER_VERSION        "1.0.0"
#define GPIO_PULL_HIGH 1
#define GPIO_PULL_LOW 0
#define FPGA_READ 1
#define FPGA_WRITE 0
#define CRC_ERR_MAX 3
#define PRE_ICE40_IMAGE_FILENAME 32
#define INVALID_CDONE_GPIO 123
#define MAX_DOWNLOAD_TIMES 3
#define ICE40_IMAGE_FILE_DIR "/system/fpgaice40/"
#define ICE40_IMAGE_FILENAME_LENTH 32
#define ICE40_IMAGE_FILEPAHT_LENTH 64 + ICE40_IMAGE_FILENAME_LENTH
#define SPI_DATA_LEN 6
#define TEST_MODE_INPUT_LEN 9
#define INVALID_VERSION1 "000000"
#define INVALID_VERSION2 "ffffff"
#define INVALID_CRC_GPIO 0

struct fpga_plat_data {
    int spi_cs_gpio;
    int crc_gpio;
    int reset_gpio;
    int power_gpio;
    int cdone_gpio;
};

struct fpga_data {
    struct spi_device    *spi;
    struct fpga_plat_data plat_data;
    /*
     * NOTE: All buffers should be dma-safe buffers
     * tx_buf :40K-Bytes Block-send
     */
    u8    *tx_buf;
    u8    *rx_buf;
};

typedef enum {
    NO_ERR,
    SPI_READ_ERR,
    VOLTAGE_ERR,
    CRC_ERR,
    CLK_ERR,
    DOWNLOAD_ERR,
    HARDWARE_ERR,
    INVILED_ERR
} FPGA_EXCEPTION;

typedef struct {
    int input;
    char output[SPI_DATA_LEN];
}talbe_command;

static s8 ice40_bin_dir[ICE40_IMAGE_FILEPAHT_LENTH];
static s8 ice40_pre_image_filename[PRE_ICE40_IMAGE_FILENAME];
FPGA_EXCEPTION erro_num = NO_ERR;
u8 reg_val[2];
struct dsm_client *fpga_ice40_dclient = NULL;
static struct dsm_dev fpga_ice40 = {
    .name = "dsm_fpgaice40",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
    .fops = NULL,
    .buff_size = 1024,
};
static struct fpga_data *fpga_drv_data = NULL;
struct wake_lock spi_write_lock;
FPGA_EXCEPTION download_err = NO_ERR;

static int get_image_version(char *buf);
static struct fpga_data* get_fpga_data(void);

static void fpga_notify_exception(FPGA_EXCEPTION fpga_err)
{
    if (!dsm_client_ocuppy(fpga_ice40_dclient)) {
        /* record message */
        dsm_client_record(fpga_ice40_dclient, "fpga  reg 0x%x,  reg 0x%x\n", reg_val[0], reg_val[1]);
        /* notify */
        dsm_client_notify(fpga_ice40_dclient, erro_num);
    }
    return;
}

static int get_exception_type()
{
    int ret;
    char read_exception[SPI_DATA_LEN];
    char indata[SPI_DATA_LEN];
    struct fpga_data *drv_data = get_fpga_data();
    struct fpga_plat_data *plat_data = NULL;
    plat_data = &drv_data->plat_data;

    memset(read_exception, 0, SPI_DATA_LEN);
    memset(indata, 0, SPI_DATA_LEN);
    indata[0] = 0;
    indata[1] = 0x80;
    indata[2] = 0;
    ice40_cs_set(GPIO_PULL_HIGH);
    ice40_set_cs_callback(ENABLE, plat_data->spi_cs_gpio);
    ret = ice40_data_spi_write_then_read(indata, SPI_DATA_LEN, read_exception, SPI_DATA_LEN);
    if(!ret) {
        hwlog_err("%s read version fail!\n", __func__);
        return -1;
    }
    return read_exception[5];
}

static int crc_err_num = 0;
static irqreturn_t fpga_exception_callback(int irq, void *data)
{
    int ret;
    struct fpga_data *drv_data = get_fpga_data();
    struct fpga_plat_data *plat_data = NULL;
    FPGA_EXCEPTION fpga_err = NO_ERR;

    plat_data = &drv_data->plat_data;
    if(GPIO_OEM_VALID(plat_data->crc_gpio )){
         hwlog_info("%s %d fpga_exception_callback(plat_data->crc_gpio)=%d \n",__func__,__LINE__,gpio_get_value(plat_data->crc_gpio ));
         if(gpio_get_value(plat_data->crc_gpio)){
             return;
         }else {
             if(gpio_get_value(plat_data->cdone_gpio)){
                 ret = get_exception_type();
                 if (ret < 0){
                     hwlog_err("%s %d read spi register ret=%d fail! \n",__func__,__LINE__,ret);
                     fpga_err = SPI_READ_ERR;
                 }else{
                     if(ret == CRC_ERR){
                         fpga_err = CRC_ERR;
                     }else if(ret == CLK_ERR){
                         fpga_err = CLK_ERR;
                     } else {
                         fpga_err = INVILED_ERR;
                     }
                }
            }else {
                fpga_err = VOLTAGE_ERR;
            }
        }
    }

    /*dmd process*/
    fpga_notify_exception(fpga_err);
    switch(fpga_err) {
        case VOLTAGE_ERR:
        case CLK_ERR:
            crc_err_num = 0;//Clear crc err, before download the firmware.
            //ret = ice40_dir_download_firmware(ice40_bin_dir);
            break;
        case CRC_ERR:
        {
            if(crc_err_num == CRC_ERR_MAX) {
                crc_err_num = 0;
                //ret = ice40_dir_download_firmware(ice40_bin_dir);
            }else{
                crc_err_num++;
            }
            break;
        }
        case SPI_READ_ERR:
        case INVILED_ERR:
        default:
            hwlog_err("%s %d fpga unhandled error ! \n",__func__,__LINE__);
            break;
    }
    return IRQ_HANDLED;
}

static int ice40_firmware_cdone()
{
    int ret;
    struct fpga_data *drv_data = get_fpga_data();
    struct fpga_plat_data *plat_data = NULL;

    plat_data = &drv_data->plat_data;
    ret = gpio_get_value(plat_data->cdone_gpio);
    msleep(1);
    hwlog_err("FPGA: firmware cdone = %d \n", ret);
    return ret;
}

/*
 * local functions, no need to export to other driver modules.
 * include :
 * @lowlevel spi transfer
 * @firmware load
 * @interrupte handle...
 */
static struct fpga_data* get_fpga_data(void)
{
    if (likely(fpga_drv_data))
        return fpga_drv_data;
    else
        return NULL;
}

/*
*
*/
static int ice40_send_dummy_data()
{
    u8 dummy_data[50];
    return ice40_data_spi_write(dummy_data, 50);
}

static int check_valid_version()
{
    char buf[SPI_DATA_LEN];

    get_image_version(buf);
    hwlog_err("%s check_valid_version image version: %s.\n", __func__, buf);
    if(!strncasecmp(buf,INVALID_VERSION1 , SPI_DATA_LEN) || !strncasecmp(buf,INVALID_VERSION2 , SPI_DATA_LEN) ) {
        return FAIL;
    }
    return SUCCESS;
}

/*
 *Input:The ice40bin file data and the size of the image.
 *Output:If download success.

*/
static int ice40_dir_download_firmware(s8 *name)
{
    int ret = SUCCESS;
    int i = 0;
    mm_segment_t oldfs = get_fs();
    struct file *fp = NULL;
    int file_size, one_size, left, block;
    loff_t pos;
    struct fpga_plat_data *plat_data = NULL;
    struct fpga_data *drv_data = get_fpga_data();

    if(!drv_data) {
        hwlog_err("%s drv data is null.\n", __func__);
        return FAIL;
    }
    plat_data = &drv_data->plat_data;
    /*Try three times if one time failed. If it is still fail, we think that there is hardware exception.*/
    ice40_set_cs_callback(ENABLE, plat_data->spi_cs_gpio);
    wake_lock(&spi_write_lock);
    for (i = 0; i < MAX_DOWNLOAD_TIMES; i++) {

        /*step1:pull the reset gpio, make the fpga to download mode. */
        gpio_direction_output(plat_data->reset_gpio, 0);
        msleep(1);
        ice40_cs_set(GPIO_PULL_LOW);
        gpio_direction_output(plat_data->reset_gpio, 1);
        msleep(1);
        /*step2:open the image file and get the image.*/
        set_fs(KERNEL_DS);
        fp = filp_open(name, O_RDONLY, 755);
        if (IS_ERR_OR_NULL(fp)) {
            hwlog_err("%s open file %s error\n", __func__, name);
            set_fs(oldfs);
            continue;
        }
        file_size =(int)(vfs_llseek(fp, 0L, SEEK_END));
        hwlog_info("start sending fpga firmware.. size=0x%x\n", file_size);
        vfs_llseek(fp, 0L, SEEK_SET);
        pos = fp->f_pos;
        for (left = file_size, block = 0; left > 0; left -= FPGA_SPI_TRAN_SIZE) {
            one_size = (left > FPGA_SPI_TRAN_SIZE)? FPGA_SPI_TRAN_SIZE : left;
            ret = vfs_read(fp, (char *)drv_data->tx_buf, one_size, &pos);
            if (ret < 0) {
                hwlog_err("%s vfs read error %d\n", __func__, ret);
                ret = FAIL;
                break;
            }
            msleep(1);
            /*step3:send the image.*/
            ret = ice40_data_spi_write( drv_data->tx_buf, one_size);
            if (ret) {
                hwlog_err("%s spi send error %d\n", __func__, ret);
                ret = FAIL;
                break;
            }
            block++;
        }

        /*step4: close the fp and check if there is write error.*/
        filp_close(fp, 0);
        set_fs(oldfs);

        if (ret) {
            hwlog_err("FPGA:firmware download fail\n");
            continue;
        }
        msleep(1);
        /*step5:send the dummy data.*/
        ret = ice40_send_dummy_data();
        if(ret) {
            hwlog_err("FPGA:send dummy data fail\n");
            continue;
        }
        /*step6:get the cdone gpio to judge if firmware success.*/
        if(INVALID_CDONE_GPIO != plat_data->cdone_gpio) {
            ret = ice40_firmware_cdone();
            if(ret) {
                hwlog_info("FPGA:firmware cdone fail\n");
                continue;
            }else {
                break;
            }
        } else {
            msleep(10);
            ret = check_valid_version();
            hwlog_err("%s check_valid_version = %d \n", __func__, ret);
            if(ret)
                continue;
            else
                break;
        }
    }

    if(i == MAX_DOWNLOAD_TIMES){
        download_err = DOWNLOAD_ERR;
        fpga_notify_exception(HARDWARE_ERR);
    }
    ice40_cs_set(GPIO_PULL_HIGH);
    wake_unlock(&spi_write_lock);
    return ret;
}
EXPORT_SYMBOL(ice40_dir_download_firmware);

/*
*Now, there are two mode of download. Up level could send the ice40 image context or the file dir.
*
*/
static ssize_t store_ice40bin_dir(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int ret = 0;
    if(!buf) {
        hwlog_err("store_ice40bin buf is null!\n");
        return ret;
    }
    hwlog_info("%s \n", __func__);
    download_err = NO_ERR;
    ret = sscanf(buf, "%s",ice40_bin_dir );
    hwlog_info("%s sscanf ret = %d ice40_bin_dir = %s\n", __func__, ret, ice40_bin_dir);
    /*call the image download.*/
    ret = ice40_dir_download_firmware(ice40_bin_dir);
    if(ret){
        hwlog_err("%s firmware error through ice40_download_firmware!\n", __func__);
        return count;
    }
    hwlog_info("%s: firmware download success!\n", __func__);
    return count;
}

static ssize_t show_download_result(struct device *pdev, struct device_attribute *attr, char *buf)
{
    return snprintf(buf, sizeof(download_err), "%d", download_err);
}

static int get_image_version(char *buf)
{
    int ret = 0;
    char read_version[SPI_DATA_LEN];
    char indata[SPI_DATA_LEN];
    char *offset = NULL;
    struct fpga_plat_data *plat_data = NULL;
    struct fpga_data *drv_data =NULL;

    offset = buf;
    drv_data = get_fpga_data();
    if(!drv_data) {
        snprintf(buf, SPI_DATA_LEN, "ERROR");
        return SPI_DATA_LEN;
    }
    plat_data = &drv_data->plat_data;
    memset(read_version, 0, SPI_DATA_LEN);
    memset(indata, 0, SPI_DATA_LEN);
    indata[0] = 0;
    indata[1] = 0x40;
    indata[2] = 0;
    hwlog_err("%s show_version!\n", __func__);
    ice40_cs_set(GPIO_PULL_HIGH);
    ice40_set_cs_callback(ENABLE, plat_data->spi_cs_gpio);
    ret = ice40_data_spi_write_then_read(indata, SPI_DATA_LEN, read_version, SPI_DATA_LEN);
    if(!ret) {
        hwlog_err("%s read version fail!\n", __func__);
    }
    if(read_version[3] > 10) {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_version[3]);
    }else {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", 0);
        offset += ret;
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_version[3]);
    }
    offset += ret;
    if(read_version[4] > 10) {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_version[4]);
    }else {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", 0);
        offset += ret;
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_version[4]);
    }
    offset += ret;
    if(read_version[5] > 10) {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_version[5]);
    }else {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", 0);
        offset += ret;
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_version[5]);
    }
    return SPI_DATA_LEN;
}

static ssize_t show_version(struct device *pdev, struct device_attribute *attr, char *buf)
{
    return get_image_version(buf);
}

static ssize_t set_version(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    return count;
}

static ssize_t input_scan_talbe(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{

    return count;
}

static ssize_t show_scan_result(struct device *pdev, struct device_attribute *attr, char *buf)
{
    return SUCCESS;
}

static ssize_t show_exception(struct device *pdev, struct device_attribute *attr, char *buf)
{

    int ret;
    char read_exception[SPI_DATA_LEN];
    char indata[SPI_DATA_LEN];
    char *offset = NULL;
    struct fpga_plat_data *plat_data = NULL;
    struct fpga_data *drv_data = get_fpga_data();
    offset = buf;
    plat_data = &drv_data->plat_data;

    memset(read_exception, 0, SPI_DATA_LEN);
    memset(indata, 0, SPI_DATA_LEN);
    indata[0] = 0;
    indata[1] = 0x80;
    indata[2] = 0;
    ice40_cs_set(GPIO_PULL_HIGH);
    ice40_set_cs_callback(ENABLE, plat_data->spi_cs_gpio);
    ret = ice40_data_spi_write_then_read(indata, SPI_DATA_LEN, read_exception, SPI_DATA_LEN);

    if(!ret) {
        hwlog_err("%s read version fail!\n", __func__);
    }
    if(read_exception[3] > 10) {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_exception[3]);
    }else {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", 0);
        offset += ret;
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_exception[3]);
    }
    offset += ret;
    if(read_exception[4] > 10) {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_exception[4]);
    }else {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", 0);
        offset += ret;
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_exception[4]);
    }
    offset += ret;
    if(read_exception[5] > 10) {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_exception[5]);
    }else {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", 0);
        offset += ret;
        ret = snprintf(offset, SPI_DATA_LEN, "%x", read_exception[5]);
    }
    return SPI_DATA_LEN;
}

static ssize_t show_cdone(struct device *pdev, struct device_attribute *attr, char *buf)
{
    struct fpga_data *drv_data = get_fpga_data();
    int cdone = drv_data->plat_data.cdone_gpio;
    int ret;
    char *offset;
    int gpio_value= gpio_get_value(cdone);
    offset = buf;

    ret = snprintf(offset, PAGE_SIZE, "%d", cdone);
    offset += ret;
    ret = snprintf(offset, PAGE_SIZE, "_%d\n", gpio_value);
    return sizeof(cdone) + sizeof(gpio_value);
}

static ssize_t show_reset(struct device *pdev, struct device_attribute *attr, char *buf)
{
    struct fpga_data *drv_data = get_fpga_data();
    int reset = drv_data->plat_data.reset_gpio;
    int ret;
    char *offset;
    int gpio_value= gpio_get_value(reset);
    offset = buf;

    ret = snprintf(offset, PAGE_SIZE, "%d", reset);
    offset += ret;
    ret = snprintf(offset, PAGE_SIZE, "_%d", gpio_value);
    return sizeof(reset) + sizeof(gpio_value);
}

static ssize_t set_reset(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct fpga_data *drv_data = get_fpga_data();
    int reset = drv_data->plat_data.reset_gpio;
    if(buf[0] == '0') {
        gpio_direction_output(reset, 0);
    }else if (buf[0] == '1') {
        gpio_direction_output(reset, 1);
    }else {
        hwlog_err("%s Wrong input!\n", __func__);
    }

    return count;
}

static ssize_t show_spics(struct device *pdev, struct device_attribute *attr, char *buf)
{
    struct fpga_data *drv_data = get_fpga_data();
    int cs = drv_data->plat_data.spi_cs_gpio;
    int ret = 0;
    char *offset = NULL;
    int gpio_value= gpio_get_value(cs);
    offset = buf;

    ret = snprintf(offset, PAGE_SIZE, "%d", cs);
    offset += ret;
    ret = snprintf(offset, PAGE_SIZE, "_%d\n", gpio_value);
    return sizeof(cs) + sizeof(gpio_value);
}

static ssize_t show_preimagename(struct device *pdev, struct device_attribute *attr, char  __user *buf)
{
    return snprintf(buf, PRE_ICE40_IMAGE_FILENAME, ice40_pre_image_filename);
}

static int hexTou16(const u8 *indata, int len)
{
   int value, i, j;
   value = 0;
   j = 1;

   for(i=len -1; i >= 0; i--) {
       if(indata[i] == 49){
           value =  value + j;
       }
       j = j *2;
   }
   return value;
}

static ssize_t set_testmode(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int ret = 0;
    u8 indata[SPI_DATA_LEN];
    u8 input_address[TEST_MODE_INPUT_LEN];
    struct fpga_data *drv_data = get_fpga_data();
    struct fpga_plat_data *plat_data = NULL;

    plat_data = &drv_data->plat_data;
    ret = sscanf(buf, "%s",input_address );
    ret = hexTou16(input_address, TEST_MODE_INPUT_LEN);
    hwlog_info("%s hexTou16 ret = %d\n", __func__, ret);
    memset(indata, 0, SPI_DATA_LEN);
    indata[0] = 1;
    indata[1] = 0xc0;
    indata[2] = 0;
    indata[3] = 0x80;
    indata[4] = ((ret>>8) & 0x1);
    indata[5] = (ret & 0xff);

    ice40_cs_set(GPIO_PULL_HIGH);
    ice40_set_cs_callback(ENABLE, plat_data->spi_cs_gpio);
    ret = ice40_data_spi_write(indata, SPI_DATA_LEN);
    if(ret) {
        hwlog_err("%s read version fail!\n", __func__);
    }
    return count;
}

static ssize_t show_testmode(struct device *pdev, struct device_attribute *attr, char *buf)
{
    int ret = 0;
    char indata[SPI_DATA_LEN];
    char outdata[SPI_DATA_LEN];
    char *offset = NULL;
    struct fpga_plat_data *plat_data = NULL;
    struct fpga_data *drv_data = get_fpga_data();
    offset = buf;
    plat_data = &drv_data->plat_data;

    ice40_cs_set(GPIO_PULL_HIGH);
    ice40_set_cs_callback(ENABLE, plat_data->spi_cs_gpio);
    memset(indata, 0, SPI_DATA_LEN);
    memset(outdata, 0, SPI_DATA_LEN);
    indata[0] = 0;
    indata[1] = 0xc0;
    indata[2] = 0;

    ret = ice40_data_spi_write_then_read(indata, SPI_DATA_LEN, outdata, SPI_DATA_LEN);
    if(ret) {
        hwlog_err("%s read version fail!\n", __func__);
        return FAIL;
    }
    if(outdata[3] > 10) {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", outdata[3]);
    }else {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", 0);
        offset += ret;
        ret = snprintf(offset, SPI_DATA_LEN, "%x", outdata[3]);
    }
    offset += ret;
    if(outdata[4] > 10) {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", outdata[4]);
    }else {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", 0);
        offset += ret;
        ret = snprintf(offset, SPI_DATA_LEN, "%x", outdata[4]);
    }
    offset += ret;
    if(outdata[5] > 10) {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", outdata[5]);
    }else {
        ret = snprintf(offset, SPI_DATA_LEN, "%x", 0);
        offset += ret;
        ret = snprintf(offset, SPI_DATA_LEN, "%x", outdata[5]);
    }
    return SPI_DATA_LEN;
}

static DEVICE_ATTR(ice40bin_download_dir, 0777, show_download_result, store_ice40bin_dir);
static DEVICE_ATTR(ice40bin_version,  0777, show_version, set_version);
static DEVICE_ATTR(ice40bin_scan_fpga_table,  S_IRUGO |S_IWUSR, show_scan_result, input_scan_talbe);
static DEVICE_ATTR(ice40bin_fpga_exception,  S_IRUGO , show_exception, NULL);
static DEVICE_ATTR(cdone,  S_IRUGO , show_cdone, NULL);
static DEVICE_ATTR(reset,  S_IRUGO |S_IWUSR , show_reset, set_reset);
static DEVICE_ATTR(spics,  S_IRUGO , show_spics, NULL);
static DEVICE_ATTR(preimagename,  S_IRUGO , show_preimagename, NULL);
static DEVICE_ATTR(testmode,  S_IRUGO |S_IWUSR , show_testmode, set_testmode);

static struct attribute *testmode_attributes[] = {
        &dev_attr_testmode.attr,
        NULL
};

static const struct attribute_group testmode_attr_group = {
        .attrs = testmode_attributes,
};

static struct attribute *preimagename_attributes[] = {
        &dev_attr_preimagename.attr,
        NULL
};

static const struct attribute_group preimagename_attr_group = {
        .attrs = preimagename_attributes,
};

static struct attribute *cdone_attributes[] = {
        &dev_attr_cdone.attr,
        NULL
};

static const struct attribute_group cdone_attr_group = {
        .attrs = cdone_attributes,
};

static struct attribute *reset_attributes[] = {
        &dev_attr_reset.attr,
        NULL
};

static const struct attribute_group reset_attr_group = {
        .attrs = reset_attributes,
};
static struct attribute *spics_attributes[] = {
        &dev_attr_spics.attr,
        NULL
};

static const struct attribute_group spics_attr_group = {
        .attrs = spics_attributes,
};

static struct attribute *ice40bin_download_dir_attributes[] = {
        &dev_attr_ice40bin_download_dir.attr,
        NULL
};

static const struct attribute_group ice40bin_download_dir_attr_group = {
        .attrs = ice40bin_download_dir_attributes,
};

static struct attribute *ice40bin_version_attributes[] = {
        &dev_attr_ice40bin_version.attr,
        NULL
};

static const struct attribute_group ice40bin_version_attr_group = {
        .attrs = ice40bin_version_attributes,
};

static struct attribute *ice40bin_scan_fpga_table_attributes[] = {
        &dev_attr_ice40bin_scan_fpga_table.attr,
        NULL
};

static const struct attribute_group ice40bin_scan_fpga_table_attr_group = {
        .attrs = ice40bin_scan_fpga_table_attributes,
};

static struct attribute *ice40bin_fpga_exception_attributes[] = {
        &dev_attr_ice40bin_fpga_exception.attr,
        NULL
};

static const struct attribute_group ice40bin_fpga_exception_attr_group = {
        .attrs =ice40bin_fpga_exception_attributes,
};

/*Platform device struct.*/
static struct platform_device fpga_ice40_dev = {
    .name = "antennafpga",
};

static int fpga_ice40_remove(struct platform_device *dev)
{
    hwlog_info("called remove %s\n", __func__);
    return 0;
}

static int create_system_group()
{
    int ret;
    ret =  sysfs_create_group(&fpga_ice40_dev.dev.kobj, &ice40bin_download_dir_attr_group);
    if (ret){
        hwlog_err("Error creating ice40bin_download_dir_attr_group sysfs entries\n");
        return FAIL;
    }
    ret =  sysfs_create_group(&fpga_ice40_dev.dev.kobj, &ice40bin_version_attr_group);
    if (ret){
        hwlog_err("Error creating ice40bin_version_attr_group sysfs entries\n");
        return FAIL;
    }
    ret =  sysfs_create_group(&fpga_ice40_dev.dev.kobj, &ice40bin_scan_fpga_table_attr_group);
    if (ret){
        hwlog_err("Error creating ice40bin_version_attr_group sysfs entries\n");
        return FAIL;
    }
    ret =  sysfs_create_group(&fpga_ice40_dev.dev.kobj, &ice40bin_fpga_exception_attr_group);
    if (ret){
        hwlog_err("Error creating ice40bin_version_attr_group sysfs entries\n");
        return FAIL;
    }
    ret =  sysfs_create_group(&fpga_ice40_dev.dev.kobj, &cdone_attr_group);
    if (ret){
        hwlog_err("Error creating cdone_attr_group sysfs entries\n");
        return FAIL;
    }
        ret =  sysfs_create_group(&fpga_ice40_dev.dev.kobj, &reset_attr_group);
    if (ret){
        hwlog_err("Error creating reset_attr_group sysfs entries\n");
        return FAIL;
    }
    ret =  sysfs_create_group(&fpga_ice40_dev.dev.kobj, &spics_attr_group);
    if (ret){
        hwlog_err("Error creating spics_attr_group sysfs entries\n");
        return FAIL;
    }
    ret =  sysfs_create_group(&fpga_ice40_dev.dev.kobj, &preimagename_attr_group);
    if (ret){
        hwlog_err("Error creating preimagename_attr_group sysfs entries\n");
        return FAIL;
    }
    ret =  sysfs_create_group(&fpga_ice40_dev.dev.kobj, &testmode_attr_group);
    if (ret){
        hwlog_err("Error creating testmode_attr_group sysfs entries\n");
        return FAIL;
    }
    return SUCCESS;
}

static int fpga_ice40_regulator_config(struct device *dev)
{
    struct regulator *fpga_vcc;
    int rc;
    fpga_vcc = regulator_get(dev, "LSW50");
    if (IS_ERR(fpga_vcc)) {
        rc = PTR_ERR(fpga_vcc);
        hwlog_err("%s: regulator get failed rc=%d\n", __func__, rc);
        return FAIL;
    }
    rc = regulator_enable(fpga_vcc);
    if (rc) {
        pr_info("%s: regulator vcc_i2c enable failed rc=%d\n", __func__, rc);
        return FAIL;
    }
    return SUCCESS;
}

static int fpga_ice40_get_dts_config(struct fpga_plat_data *plat_data)
{
    int ret;
    struct device_node *np = NULL;
    const char *p_string = NULL;
    hwlog_info("%s start...\n", __func__);
    np = of_find_compatible_node(NULL, NULL, "antenna_fpga");
    /*First, CDONE GPIO.*/
    plat_data->cdone_gpio = of_get_named_gpio(np, "antenna_fpga,gpio-cdone", 0);
    hwlog_info(">>>> %s get gpio-cdone gpio %d\n", __func__, plat_data->cdone_gpio);
    if(GPIO_OEM_VALID(plat_data->cdone_gpio) && INVALID_CDONE_GPIO != plat_data->cdone_gpio){
        ret = gpio_request(plat_data->cdone_gpio, DRIVER_NAME "(fpga_cdone)");
        if(ret < 0){
            hwlog_err("%s: %d fail to requset  cdone gpio %d ret =%d!!\n", __func__, __LINE__, plat_data->cdone_gpio, ret);
            goto err_req_irq_cdone;
        }
        if (gpio_direction_input(plat_data->cdone_gpio))
            hwlog_err("probe - cdone_gpio direction input error");
    }
    /*Second, CRC_Result GPIO.*/
    plat_data->crc_gpio = of_get_named_gpio(np, "antenna_fpga,gpio-exception", 0);
    hwlog_info(">>>> %s get CRC_Result gpio %d\n", __func__, plat_data->crc_gpio );
    if(GPIO_OEM_VALID(plat_data->crc_gpio) && INVALID_CRC_GPIO != plat_data->crc_gpio){
        ret = gpio_request(plat_data->crc_gpio, DRIVER_NAME "(fpga_crc)");
        if(ret < 0){
            hwlog_err("%s: %d fail to requset  crc_gpio gpio %d ret =%d!!\n", __func__, __LINE__, plat_data->crc_gpio, ret);
            goto err_req_irq_crc;
        }

        gpio_direction_input(plat_data->crc_gpio);

        ret = request_irq(plat_data->crc_gpio, fpga_exception_callback, IRQF_TRIGGER_FALLING, DRIVER_NAME "(fpga_crc)", NULL);
        if (ret < 0) {
            hwlog_err("%s: %d fail to request irq for CRC_Result!!\n", __func__, __LINE__);
            goto err_req_irq_crc;
        }
    }
    /*Third, FPGA reset GPIO.*/
    plat_data->reset_gpio = of_get_named_gpio(np, "antenna_fpga,gpio-reset", 0);
    hwlog_info(">>>> %s get gpio-reset gpio %d\n", __func__, plat_data->reset_gpio );
    if(GPIO_OEM_VALID(plat_data->reset_gpio)){
        ret = gpio_request(plat_data->reset_gpio, DRIVER_NAME "(fpga_reset)");
        if(ret < 0){
            hwlog_err("%s: %d fail to requset  reset_gpio gpio %d ret =%d!!\n", __func__, __LINE__, plat_data->reset_gpio, ret);
            goto err_req_irq_rst;
        }
    }
    /*Fourth, FPGA spi cs GPIO.*/
    plat_data->spi_cs_gpio = of_get_named_gpio(np, "antenna_fpga,gpio-cs", 0);
    hwlog_info(">>>> %s get fpga_spi_cs gpio %d\n", __func__, plat_data->spi_cs_gpio );
    if(GPIO_OEM_VALID(plat_data->spi_cs_gpio)){
        ret = gpio_request(plat_data->spi_cs_gpio, DRIVER_NAME "(fpga_spi_cs)");
        if(ret < 0){
            hwlog_err("%s: %d fail to requset  cs_gpio gpio %d ret =%d!!\n", __func__, __LINE__, plat_data->spi_cs_gpio, ret);
            goto err_req_irq_spi_cs;
        }
    }
    /*Fifth, read pre-image-filename from dts.*/
    ret = of_property_read_string(np, "antenna_fpga,pre-image-filename", &p_string);
    if(ret < 0) {
        hwlog_err("%s: %d fail to pre-image-filename!\n", __func__, __LINE__);
        goto err_get_pre_image_filename;
    }
    memcpy(ice40_pre_image_filename, p_string, strlen(p_string));
    hwlog_err("antenna_fpga,pre-image-filename = %s\n", ice40_pre_image_filename);

    return SUCCESS;
err_get_pre_image_filename:
    if(GPIO_OEM_VALID(plat_data->spi_cs_gpio)){
        gpio_free(plat_data->spi_cs_gpio);
    }
err_req_irq_spi_cs:
    if(GPIO_OEM_VALID(plat_data->reset_gpio)){
        gpio_free(plat_data->reset_gpio);
    }
err_req_irq_rst:
    if(GPIO_OEM_VALID(plat_data->crc_gpio)){
        gpio_free(plat_data->crc_gpio);
    }
err_req_irq_crc:
    if(GPIO_OEM_VALID(plat_data->crc_gpio)){
        free_irq(gpio_to_irq(plat_data->crc_gpio), NULL);
    }
 err_req_irq_cdone:
    return FAIL;
}

static  int fpga_ice40_dmd_register()
{
   if (!fpga_ice40_dclient) {
        fpga_ice40_dclient = dsm_register_client(&fpga_ice40);
        if(!fpga_ice40_dclient){
            hwlog_err("dsm_register_client failed\n");
            return FAIL;
        }
    }
    return SUCCESS;
}

static int fpga_ice40_wakelock_init()
{
    wake_lock_init(&spi_write_lock, WAKE_LOCK_SUSPEND, "fpga_spi_write_wake_lock");
    return SUCCESS;
}

int fpga_ice40_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct fpga_data *drv_data = NULL;
    struct fpga_plat_data *plat_data = NULL;
    /* step 1: alloc driver data struct */
    drv_data = kmalloc(sizeof(*drv_data), GFP_KERNEL);
    if (!drv_data) {
        hwlog_err("probe - can not alloc driver data");
        goto err_data_alloc;
    }
    memset(drv_data, 0, sizeof(*drv_data));
    plat_data = &drv_data->plat_data;
    /*step 2: Configure gpio, and get irq*/
    ret = fpga_ice40_get_dts_config(plat_data);
    if(ret) {
         hwlog_err("fpga_ice40_get_dts_config error!");
         goto err_get_dts_config;
    }
    /*step3: alloc trasfer data and receive data*/
    drv_data->tx_buf = __get_free_pages(GFP_KERNEL|GFP_DMA, get_order(FPGA_SPI_TRAN_SIZE));
    drv_data->rx_buf  = (void *)__get_free_pages(GFP_KERNEL|GFP_DMA, get_order(FPGA_SPI__FIFO_SIZE));
    hwlog_info("probe - tx_buf=0x%p rx_buf=0x%p ", drv_data->tx_buf, drv_data->rx_buf);
    if ((!drv_data->tx_buf) || (!drv_data->rx_buf) ){
        hwlog_err("probe - can not alloc dma buf page");
        goto err_alloc_buf;
    }
    /*step4:register device*/
    ret = platform_device_register(&fpga_ice40_dev);
    if (ret) {
        hwlog_err( "%s: platform_device_register failed, ret:%d.\n", __func__, ret);
        goto err_register_platform_device;
    }
    ret =  create_system_group();
    if (ret) {
        hwlog_err( "%s: create group failed, ret:%d.\n", __func__, ret);
        goto err_create_sysfs_entries;
    }
    /*step5: set dev*/
    fpga_drv_data = drv_data;

    /*step6: dmd register.*/
    ret = fpga_ice40_dmd_register();
    if(ret) {
        hwlog_err( "%s: dmd register failed, ret:%d.\n", __func__, ret);
    }
    ret = fpga_ice40_wakelock_init();
    if (ret) {
        hwlog_err( "%s: wakelock int failed, ret:%d.\n", __func__, ret);
        goto err_wakelock_init;
    }
    ret = fpga_ice40_regulator_config(&pdev->dev);
    if(ret) {
        hwlog_err( "%s: fpga_ice40_regulator_config , ret:%d.\n", __func__, ret);
    }
    ret = ice40_set_cs_callback(DISABLE, plat_data->spi_cs_gpio);
    if(ret) {
        hwlog_err( "%s: ice40_set_cs_callback error, ret:%d.\n", __func__, ret);
    }
    return SUCCESS;

err_wakelock_init:
err_create_sysfs_entries:
    platform_device_unregister(&fpga_ice40_dev);
err_register_platform_device:
    free_pages((unsigned long)drv_data->tx_buf,  get_order(FPGA_SPI_TRAN_SIZE));
    free_pages((unsigned long)drv_data->rx_buf,  get_order(PAGE_SIZE));
err_alloc_buf:
err_get_dts_config:
    kfree(drv_data);
err_data_alloc:
    return FAIL;
}

static const struct of_device_id fpga_ice40_match_table[] = {
    {
        .compatible = "antenna_fpga",
        .data = NULL,
    },
    {
    },
};

MODULE_DEVICE_TABLE(of, fpga_ice40_match_table);

static struct platform_driver fpga_ice40_driver = {
    .probe = fpga_ice40_probe,
    .remove = fpga_ice40_remove,
    .driver = {
        .name = "antenna_fpga",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(fpga_ice40_match_table),
    },

};

static int __init fpga_ice40__init(void)
{
    int err = 0;
    err = platform_driver_register(&fpga_ice40_driver);
    if (err){
        hwlog_err("FPGA__ %s fpga_ice40__init.\n", __func__);
    }

    return err;
}

static void __exit fpga_ice40__exit(void)
{
    struct fpga_data *drv_data = get_fpga_data();
    struct fpga_plat_data *plat_data = NULL;
    plat_data = &drv_data->plat_data;
    platform_driver_unregister(&fpga_ice40_driver);
    if(GPIO_OEM_VALID(plat_data->spi_cs_gpio)){
        gpio_free(plat_data->spi_cs_gpio);
    }
    if(GPIO_OEM_VALID(plat_data->reset_gpio)){
        gpio_free(plat_data->reset_gpio);
    }
     if(GPIO_OEM_VALID(plat_data->crc_gpio)){
        gpio_free(plat_data->crc_gpio);
    }
    if(GPIO_OEM_VALID(plat_data->crc_gpio)){
        free_irq(gpio_to_irq(plat_data->crc_gpio), NULL);
    }
    if (!drv_data->tx_buf)
        free_pages((unsigned long)drv_data->tx_buf,  get_order(FPGA_SPI_TRAN_SIZE));
    if (!drv_data->rx_buf)
        free_pages((unsigned long)drv_data->rx_buf,  get_order(PAGE_SIZE));
}

module_init(fpga_ice40__init);
module_exit(fpga_ice40__exit);
