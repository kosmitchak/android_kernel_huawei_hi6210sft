/*!
 *****************************************************************************

 * @file    sysdev_utils1.c
 *
 * This file contains DEVIF user mode implementation of the
 * System Device Kernel Mode Utilities Level 1 API.
 *
 * ---------------------------------------------------------------------------

         <b>Copyright (c) 2010 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         otherwise, or disclosed to the third parties without the
         express written permission of Imagination Technologies
         Limited, Home Park Estate, Kings Langley, Hertfordshire,
         WD4 8LZ, U.K.

 *****************************************************************************/

#include <asm/io.h>
#include <asm/page.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/export.h>
#include <linux/kobject.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/clk.h>
#include <linux/regulator/consumer.h>

#include <system.h>
#include <sysmem_utils.h>
#include <sysdev_utils.h>

#define DEVICE_NAME "DT_device"

static int module_irq = -1;
static IMG_PHYSADDR module_reg_base = 0;
static unsigned int module_reg_size = 0;
static void* reg_base_addr;


/* Device information...*/
static SYSDEVU_sInfo *gpsInfo = IMG_NULL;
static bool device_detected = IMG_FALSE;

struct clk *gvenc_clk = NULL;   //j00140427
struct regulator_bulk_data gvenc_regulator = {0};

int driver_probe(struct platform_device *ofdev);

static const struct of_device_id venc_of_match[] = {
	{ .compatible = "hisi,k3v3-venc", },
	{ }
};

static struct platform_driver local_driver = {
    .probe = driver_probe,
    .driver = {
        .name = "goldfish_msvxe",
	    .owner = THIS_MODULE,
	    .of_match_table = venc_of_match
     },
};
#if 0
#define PERI_CRG (0xFFF35000)
#define SCTRL (0xFFF0A000)
#define PMC (0xFFF31000)

//unsigned int val = 0;
//u8 __iomem *reset_mmio_PERI_CRG = NULL;
//reset_mmio_PERI_CRG = ioremap(0xfff35000,4);
//u8 __iomem *reset_mmio_PMC = NULL;
//reset_mmio_PMC= ioremap(0xfff31000,4);

void power_on_encode(int isDec)
{
    unsigned int val =  0;
    unsigned int val2 =  0;
    unsigned int val3 =  0;

    writel(0xe0000060,(ioremap(PERI_CRG,4) + 0x00));/*外设时钟使能*/
    writel(0xe018,(ioremap(PERI_CRG,4) + 0x64));

    val = readl(ioremap(PMC,4) + 0x380);

    val &= (~(0x1<<4));
    writel(val,(ioremap(PMC,4) + 0x380));/*配置VCODEC_SUBSYS退出低功耗模式*/
    //printk("====power_on=====1=======\n");
    do
    {
        val = readl(ioremap(PMC,4) + 0x380);
        val &= (0x01<<4);
    }while(val);              /*查询退出请求是否被响应*/
    //printk("====power_on=====2=======\n");
    do
    {
        val = readl(ioremap(PMC,4) + 0x388);
        val &= (0x01<<4);
    }while(val);         /*确认VCODEC_SUBSYS已退出低功耗模式*/
    //printk("====power_on=====3=======\n");


    if(isDec)
    {
        writel(0x04,(ioremap(PERI_CRG,4) + 0x150));/*MTCMOS使能*/
        writel(0x800,(ioremap(PERI_CRG,4) + 0x30));/*开启VENC和NOC_VENC时钟*/
        writel(0x04,(ioremap(PERI_CRG,4) + 0x148));/*ISO去使能*/
        writel(0x200,(ioremap(PERI_CRG,4) + 0x88));/*解复位*/
        //printk("====power_on=====4=======\n");

        val = readl(ioremap(PMC,4) + 0x380);
        val &= (~(0x1<<10));
	 writel(val,(ioremap(PMC,4) + 0x380));/*配置NOC VDEC退出低功耗模式*/
        do
        {
            val = readl(ioremap(PMC,4) + 0x384);
            val &= (0x01<<10);
        }while(val);              //查询请求是否被响应
        //printk("====power_on=====5=======\n");
        do
        {
            val = readl(ioremap(PMC,4) + 0x388);
            val &= (0x01<<10);
        }while(val);         /*轮询PMC中NOC_POWER_IDLEACK（0x384） bit 10和NOC_POWER_IDLE（0x388） bit 10都为1’b0*/
        //printk("====power_on=====6=======\n");

    }
    else
    {
        writel(0x02,(ioremap(PERI_CRG,4) + 0x150));/*MTCMOS使能*/
	 writel(0x400,(ioremap(PERI_CRG,4) + 0x30));/*开启VENC和NOC_VENC时钟*/
	 writel(0x02,(ioremap(PERI_CRG,4) + 0x148));	/*ISO去使能*/
        writel(0x100,(ioremap(PERI_CRG,4) + 0x88));/*解复位*/

        val = readl(ioremap(PMC,4) + 0x380);
        val &= (~(0x1<<11));

        writel(val,(ioremap(PMC,4) + 0x380));/*配置NOC VENC退出低功耗模式*/
        do
        {
            val = readl(ioremap(PMC,4) + 0x384);
            val &= (0x01<<11);
        }while(val);              //查询请求是否被响应

        do
        {
            val = readl(ioremap(PMC,4) + 0x388);
            val &= (0x01<<11);
        }while(val);         /*轮询PMC中NOC_POWER_IDLEACK（0x384） bit 10和NOC_POWER_IDLE（0x388） bit 10都为1’b0*/

    }
    //printk("====power_on=====7=======\n");

}
#endif

int driver_probe(struct platform_device *ofdev) {
	int ret;
	struct resource res;

	IMG_ASSERT(ofdev->resource[0].flags == IORESOURCE_MEM);
	IMG_ASSERT(ofdev->resource[1].flags == IORESOURCE_IRQ);
      // power_on_encode(0);
	ret = of_address_to_resource(ofdev->dev.of_node, 0, &res);
	if (ret) {
		dev_err(&ofdev->dev, "missing 'reg' property in device tree\n");
		ret = -EINVAL;
		return ret;
	}
	module_reg_base = res.start;
	module_reg_size = resource_size(&res);
	reg_base_addr = of_iomap(ofdev->dev.of_node, 0);
	if (NULL == reg_base_addr) {
		printk("get reg base addr failed\n");
	}

	printk("[vxe] physical address : %llx\n", module_reg_base);

	module_irq = irq_of_parse_and_map(ofdev->dev.of_node, 0);
	if (module_irq == 0) {
		dev_err(&ofdev->dev, "could not map IRQ\n");
		module_reg_base = 0;
		module_reg_size = 0;
		ret = -EINVAL;
		return ret;
	}

	gvenc_regulator.supply = "ldo_venc";

	ret = regulator_bulk_get(&ofdev->dev, 1, &gvenc_regulator);
	if (ret) {
		printk("couldn't get regulators %d\n\r", ret);
		return -1;
	}

	gvenc_clk = of_clk_get(ofdev->dev.of_node, 0);
	if (IS_ERR(gvenc_clk))
	{
		printk("get venc clock failed\n");
		ret =  PTR_ERR(gvenc_clk);
		regulator_put(gvenc_regulator.consumer);
		memset(&gvenc_regulator,0,sizeof(gvenc_regulator));
		return -1;
	}

	device_detected = IMG_TRUE;
	return 0;
}

static irqreturn_t handle_interrupt(int irq, void *dev_id)
{
	bool handled;
	if ( (gpsInfo != IMG_NULL) && (gpsInfo->pfnDevKmLisr != IMG_NULL) )
	{
		SYSOSKM_DisableInt();
		handled = gpsInfo->pfnDevKmLisr(gpsInfo->pvParam);
		SYSOSKM_EnableInt();
		if (handled)
		{
			/* Signal this...*/
			return IRQ_HANDLED;
		}
	}
	printk("irq end.\n");
	return IRQ_NONE;
}

static void handle_suspend(SYSDEVU_sInfo *dev, IMG_BOOL forAPM)
{
	printk("@@@@@@@@@@@@@@@%s,%d\n",__FUNCTION__,__LINE__);

	if (gvenc_clk)
	{
		clk_disable_unprepare(gvenc_clk);
	}

	regulator_bulk_disable(1, &(gvenc_regulator));
}

static void handle_resume(SYSDEVU_sInfo *dev, IMG_BOOL forAPM)
{
	int ret = -1;

	printk("@@@@@@@@@@@@@@@%s,%d\n",__FUNCTION__,__LINE__);

	ret = regulator_bulk_enable(1, &(gvenc_regulator));
	if (ret)
		printk( "failed to enable regulators %d\n", ret);

	if (gvenc_clk)
	{
		ret = clk_prepare_enable(gvenc_clk);
		if (ret)
		{
			printk("enable clock failed\n");
			ret = -EINVAL;
		}
	}
}


static void free_device(SYSDEVU_sInfo *dev)
{
	if(!device_detected)
		return;

	iounmap(reg_base_addr);
	free_irq(module_irq, gpsInfo);
	platform_driver_unregister(&local_driver);

	if (NULL != gvenc_clk) {
		clk_put( gvenc_clk);
		gvenc_clk = NULL;
	}

	regulator_put(gvenc_regulator.consumer);
	memset(&gvenc_regulator,0,sizeof(gvenc_regulator));
}

static struct SYSDEV_ops device_ops = {
		.free_device = free_device,

		.resume_device = handle_resume,
		.suspend_device = handle_suspend
};

IMG_RESULT SYSDEVU_RegisterDriver(SYSDEVU_sInfo *sysdev) {
	int ret = 0;

	gpsInfo = sysdev;

	ret = platform_driver_register(&local_driver);
	if (ret != 0) {
		ret = IMG_ERROR_DEVICE_NOT_FOUND;
		goto failure_register;
	}

	if (device_detected != IMG_TRUE) {
		ret = IMG_ERROR_DEVICE_NOT_FOUND;
		goto failure_detect;
	}

	if (request_irq(module_irq, handle_interrupt, 0, DEVICE_NAME, gpsInfo)) {
		printk(KERN_ERR "goldfish_device: Failed to get IRQ\n");
		ret = IMG_ERROR_GENERIC_FAILURE;
		goto failure_request_irq;
	}

	SYSDEVU_SetDevMap(sysdev, module_reg_base, ((IMG_UINT32 *)reg_base_addr), module_reg_size, 0, IMG_NULL, 0, 0);
	SYSDEVU_SetDeviceOps(sysdev, &device_ops);

	ret = SYSMEMKM_AddSystemMemory(&sysdev->sMemPool);
	if(IMG_SUCCESS != ret)
	{
		goto failure_add_sys_mem;
	}

	/* Return success...*/
	return IMG_SUCCESS;

failure_add_sys_mem:
failure_request_irq:
	iounmap(reg_base_addr);
failure_detect:
	platform_driver_unregister(&local_driver);
failure_register:
	gpsInfo = IMG_NULL;
	return ret;
}

IMG_RESULT SYSDEVU_UnRegisterDriver(SYSDEVU_sInfo *sysdev) {
	SYSMEMU_RemoveMemoryHeap(sysdev->sMemPool);
	sysdev->ops->free_device(sysdev);
	return IMG_SUCCESS;
}
