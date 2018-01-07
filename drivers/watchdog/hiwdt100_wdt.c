/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 *  hiwdt100_wdt.c    hisilicon watchdog IP hiwdt100 driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/module.h>             /* For module specific items */
#include <linux/moduleparam.h>        /* For new moduleparam's */
#include <linux/types.h>              /* For standard types (like size_t) */
#include <linux/errno.h>              /* For the -ENODEV/... values */
#include <linux/kernel.h>             /* For printk/... */
#include <linux/watchdog.h>           /* For the watchdog specific items */
#include <linux/init.h>               /* For __init/__exit/... */
#include <linux/fs.h>                 /* For file operations */
#include <linux/platform_device.h>    /* For platform_driver framework */
#include <linux/ioport.h>             /* For io-port access */
#include <linux/spinlock.h>           /* For spin_lock/spin_unlock/... */
#include <linux/uaccess.h>            /* For copy_to_user/put_user/... */
#include <linux/io.h>                 /* For inb/outb/... */
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/of.h>                 /* For of device access */
#include <linux/of_irq.h>             /* For of irq access */
#include <linux/of_address.h>

/*lint -e750*/
#define DEBUG 1

#ifdef DEBUG
#define DBG(args...)	printk(args)
#else
#define DBG(args...)	do { } while (0)
#endif

#ifdef _DRV_LLT_
#define static
#endif

#define MODULE_NAME "hiwdt100"

#define DEFAULT_WDT_PERIOD     12         /*5 second*/
#define DEFAULT_WDG_FREQUENCY (32*1024)  /*the frequency of counting is 32K*/
#define MAX_WDT_PERIOD         86400     /*1 day*/
#define EARLY_WDT_PERIOD        30	  /* 1min wdt reset */

/*register offset definition*/
#define WDG_LOAD     (0x0000)
#define WDG_VALUE    (0x0004)
#define WDG_CONTROL  (0x0008)
#define WDG_INTCLR   (0x000c)
#define WDG_LOCK     (0x0C00)
#define UNLOCK_KEY   (0x1acce551)

struct hiwdt100_wdt {
	void __iomem *membase;	/*io mapped address*/
	struct clk *wdt_clk;   /*clock*/
	unsigned int freq;     /*watchdog clock frequence,
						for caculate countdown value*/
	unsigned int on;
	unsigned int trigger;
	unsigned int interval; /*time interval (second)*/
	unsigned int count;    /*countdown value = period*freq,
						set to WDT_LOAD register*/
	unsigned int irq;      /*irq number*/
	phys_addr_t iobase;    /*watchdog register physical address*/
	size_t size;           /*watchdog register space size*/
};

static int __initdata no_hiwdt100;
static int __init hiwdt100_disable(char *str)
{
	no_hiwdt100 = 1;
	return 1;
}
__setup("no_hiwdt100", hiwdt100_disable);

int early_hiwdt100_wdt_init(void)
{
	struct clk *wdt_clk = NULL;
	void __iomem *wdt_base = NULL;
	unsigned int freq;
	struct device_node *np = NULL;
	int ret;

	if (1 == no_hiwdt100) {
		pr_err("%s: wdt disable in HIMNTN\n", __func__);
		return 0;
	}

	np = of_find_compatible_node(NULL, NULL, "hisilicon,hiwdt100");
	if (IS_ERR(np)) {
		pr_err("%s: can not find hiwdt100 node\n", __func__);
		return -1;
	}

	wdt_base = of_iomap(np, 0);
	if (wdt_base == NULL) {
		pr_err("%s: failed to ioremap() region\n", __func__);
		return -1;
	}

	wdt_clk = of_clk_get(np, 0);
	if (IS_ERR(wdt_clk)) {
		pr_err("%s: failed to get clock\n", __func__);
		return -1;
	}

	clk_prepare_enable(wdt_clk);

	ret = of_property_read_u32_index(np, "clock-count", 0, &freq);
	if (ret) {
		pr_err("Can not find clock-rate property. Use the asic default value: %ds\n",
			DEFAULT_WDG_FREQUENCY);
		freq = DEFAULT_WDG_FREQUENCY;
	}
	freq = freq * EARLY_WDT_PERIOD;

	writel(UNLOCK_KEY, wdt_base + WDG_LOCK);
	/*set count*/
	writel(freq, wdt_base + WDG_LOAD);
	/*set enable*/
	writel(((1<<1) | (1<<0)), wdt_base + WDG_CONTROL);
	writel(0, wdt_base + WDG_LOCK);

	return 0;
}

static ssize_t attr_get_on(struct device *dev,
			   struct device_attribute *attr,
			   char *buf)
{
	struct hiwdt100_wdt *wdt_data;

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", wdt_data->on);
}

static ssize_t attr_set_on(struct device *dev,
			   struct device_attribute *attr,
			   const char *buf, size_t size)
{
	struct hiwdt100_wdt *wdt_data;
	unsigned int on;
	unsigned int val;

	/* only root can play with this */
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);

	if (kstrtouint(buf, 0, &on))
		return -EINVAL;

	if ((0 != on) && (1 != on)) {
		dev_err(dev, "%s: on should be 0 or 1\n", __func__);
		return -EINVAL;
	}

	if (1 == on)
		val = ((1<<1) | (1<<0));  /*11*/
	else
		val = (1<<1);  /*10*/

	/*make effective */
	writel(UNLOCK_KEY, wdt_data->membase + WDG_LOCK);
	writel(wdt_data->count, wdt_data->membase + WDG_LOAD);
	writel(val, wdt_data->membase + WDG_CONTROL);
	writel(0, wdt_data->membase + WDG_LOCK);

	wdt_data->on = on;

	return size;
}

static ssize_t attr_get_interval(struct device *dev,
				 struct device_attribute *attr,
				 char *buf)
{
	struct hiwdt100_wdt *wdt_data;

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", wdt_data->interval);
}

static ssize_t attr_set_interval(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t size)
{
	struct hiwdt100_wdt *wdt_data;
	unsigned int interval;

	/* only root can play with this */
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);

	if (kstrtouint(buf, 0, &interval))
		return -EINVAL;

	if ((0 == interval) || (interval > MAX_WDT_PERIOD)) {
		dev_err(dev, "%s: interval should between %d and %d\n",
			__func__, 0, MAX_WDT_PERIOD);
		return -EINVAL;
	}

	wdt_data->interval = interval;
	wdt_data->count = wdt_data->freq * interval;

	writel(UNLOCK_KEY, wdt_data->membase + WDG_LOCK);
	writel(wdt_data->count, wdt_data->membase + WDG_LOAD);
	writel(0, wdt_data->membase + WDG_LOCK);
	return size;
}

static ssize_t attr_get_base(struct device *dev,
			     struct device_attribute *attr,
			     char *buf)
{
	struct hiwdt100_wdt *wdt_data;

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);
	return sprintf(buf, "0x%p\n", wdt_data->membase);
}

static ssize_t attr_get_value(struct device *dev,
			      struct device_attribute *attr,
			      char *buf)
{
	struct hiwdt100_wdt *wdt_data;
	unsigned int val;

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);
	val = readl(wdt_data->membase + WDG_VALUE);
	return sprintf(buf, "0x%x\n", val);
}

static ssize_t attr_get_load(struct device *dev,
			     struct device_attribute *attr,
			     char *buf)
{
	struct hiwdt100_wdt *wdt_data;
	unsigned int val;

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);
	val = readl(wdt_data->membase + WDG_LOAD);
	return sprintf(buf, "0x%x\n", val);
}

static ssize_t attr_get_ctrl(struct device *dev,
			     struct device_attribute *attr,
			     char *buf)
{
	struct hiwdt100_wdt *wdt_data;
	unsigned int val;

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);
	val = readl(wdt_data->membase + WDG_CONTROL);
	return sprintf(buf, "0x%x\n", val);
}


static ssize_t attr_get_trigger(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	struct hiwdt100_wdt *wdt_data;

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", wdt_data->trigger);
}

static ssize_t attr_set_trigger(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t size)
{
	struct hiwdt100_wdt *wdt_data;
	unsigned int trigger;

	/* only root can play with this */
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);

	if (kstrtouint(buf, 0, &trigger))
		return -EINVAL;

	if ((0 != trigger) && (1 != trigger)) {
		dev_err(dev, "%s: trigger should be 0 or 1\n", __func__);
		return -EINVAL;
	}

	wdt_data->trigger = trigger;
	return size;
}

static struct device_attribute attributes[] = {
	__ATTR(on, 0664, attr_get_on, attr_set_on),
	__ATTR(interval, 0664, attr_get_interval, attr_set_interval),
	__ATTR(trigger, 0664, attr_get_trigger, attr_set_trigger),
	__ATTR(base, 0444, attr_get_base, NULL),
	__ATTR(reg_value, 0444, attr_get_value, NULL),
	__ATTR(reg_load, 0444, attr_get_load, NULL),
	__ATTR(reg_ctrl, 0444, attr_get_ctrl, NULL),
};

static int create_sysfs_interfaces(struct device *dev)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(attributes); i++)
		if (device_create_file(dev, attributes + i))
			goto error;

	return 0;

error:
	for (; i >= 0; i--)
		device_remove_file(dev, attributes + i);

	dev_err(dev, "%s:Unable to create interface\n", __func__);
	return -1;
}

static int remove_sysfs_interfaces(struct device *dev)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(attributes); i++)
		device_remove_file(dev, attributes + i);

	return 0;
}

/*****************************************************************************
 Description : watchdog irq handle , write WDT_INTCLR register to keep watchdog alive
  History
  1.Date: 2012/9/13
    Author : x00138766
    Modification : Created function
*****************************************************************************/
static irqreturn_t hiwdt100_wdt_irq(int irqno, void *param)
{
	struct device *dev = (struct device *)param;
	struct hiwdt100_wdt *wdt_data;

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);
	if (1 == wdt_data->trigger)
		return IRQ_HANDLED;

	/*write any value to WDG_INTCLR to keep watchdog alive*/
	writel(UNLOCK_KEY, wdt_data->membase + WDG_LOCK);
	writel(0xffffffff,  wdt_data->membase + WDG_INTCLR);
	writel(0, wdt_data->membase + WDG_LOCK);

	return IRQ_HANDLED;
}

/*****************************************************************************
 Description : watchdog driver probe function,
			to enable watchdog IP, connect IRQ handle,
			get/enable clock, create sysctl file,
  History
  1.Date: 2012/9/13
    Author : x00138766
    Modification : Created function
*****************************************************************************/
static int hiwdt100_wdt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct hiwdt100_wdt *wdt_data;
	struct clk *wdt_clk = NULL;
	void __iomem *wdt_base = NULL;
	unsigned int freq;
	int ret = 0;
	struct device_node *np;
	unsigned int default_timeout;
	int irq;

	DBG("probe: hiwdt100 probe id %d.\r\n", pdev->id);

	np = of_find_compatible_node(NULL, NULL, "hisilicon,hiwdt100");
	if (IS_ERR(np)) {
		dev_err(dev, "Can not find hiwdt100 node\n");
		goto err_out;
	}
	wdt_data = kmalloc(sizeof(struct hiwdt100_wdt), GFP_KERNEL);
	if (NULL == wdt_data) {
		dev_err(dev, "kmalloc fail\n");
		ret = -EBUSY;
		goto err_out;
	}
	if (!devm_request_mem_region(dev, pdev->resource->start,
		resource_size(pdev->resource), MODULE_NAME)) {
		dev_err(dev, "failed to get memory region\n");
		ret = -EBUSY;
		goto err_free;
	}
	wdt_base = devm_ioremap(dev, pdev->resource->start,
		resource_size(pdev->resource));
	if (wdt_base == NULL) {
		dev_err(dev, "failed to ioremap() region\n");
		ret = -EINVAL;
		goto err_req;
	}

	DBG("probe: mapped wdt_base=%p\n", wdt_base);

	irq = irq_of_parse_and_map(np, 0);
	if (irq <= 0) {
		dev_err(dev, "failed to get irq resource\n");
		ret = -EINVAL;
		goto err_map;
	}

	/*set disable before request_irq*/
	writel(UNLOCK_KEY, wdt_base + WDG_LOCK);
	writel(((0<<1) | (0<<0)), wdt_base + WDG_CONTROL);
	writel(0xffffffff, wdt_base + WDG_INTCLR);
	writel(0, wdt_base + WDG_LOCK);

	ret = devm_request_irq(dev, irq, hiwdt100_wdt_irq,
		IRQF_TRIGGER_RISING, MODULE_NAME, dev);
	if (ret != 0) {
		dev_err(dev, "failed to install irq (%d)\n", ret);
		ret = -EINTR;
		goto err_map;
	}
	/* there must be watchdog clock definition,
	to caculate watchdog countdown value*/
	wdt_clk = devm_clk_get(dev, NULL);
	if (IS_ERR(wdt_clk)) {
		dev_err(dev, "failed to find watchdog clock source\n");
		ret = -ENXIO;
		goto err_irq;
	}

	clk_prepare_enable(wdt_clk);

	ret = of_property_read_u32_index(np, "default-timeout",
		0, &default_timeout);
	if (ret) {
		dev_warn(dev,
			"Can not find default-timeout property. Use the default value: %ds\n",
			DEFAULT_WDT_PERIOD);
		default_timeout = DEFAULT_WDT_PERIOD;
	}

	ret = of_property_read_u32_index(np, "clock-count", 0, &freq);
	if (ret) {
		dev_warn(dev,
			"Can not find clock-rate property. Use the asic default value: %ds\n",
			DEFAULT_WDG_FREQUENCY);
		freq = DEFAULT_WDG_FREQUENCY;
	}
	dev_info(dev, "probe : watchdog clock frequency count %d\n", freq);

	wdt_data->membase = wdt_base;
	wdt_data->wdt_clk = wdt_clk;
	wdt_data->freq = freq;
	wdt_data->on = 1;
	wdt_data->trigger = 0;
	wdt_data->interval = default_timeout;
	wdt_data->count = freq * default_timeout;
	wdt_data->irq = irq;
	wdt_data->iobase = pdev->resource->start;
	wdt_data->size = resource_size(pdev->resource);
	dev_set_drvdata(dev, wdt_data);

	/*set count*/
	writel(UNLOCK_KEY, wdt_data->membase + WDG_LOCK);
	writel(wdt_data->count, wdt_data->membase + WDG_LOAD);
	/*set enable*/
	writel(((1<<1) | (1<<0)), wdt_data->membase + WDG_CONTROL);
	writel(0, wdt_data->membase + WDG_LOCK);

	create_sysfs_interfaces(dev);

	dev_info(dev, "watchdog probe : watchdog clock active, interval %u second, count %u\n",
		wdt_data->interval, wdt_data->count);

	return 0;

err_irq:
	devm_free_irq(dev, irq, dev);
err_map:
	devm_iounmap(dev, wdt_base);
err_req:
	devm_release_mem_region(dev, wdt_data->iobase, wdt_data->size);
err_free:
	kfree(wdt_data);
err_out:
	dev_err(dev, "hiwdt100 probe fail !\n");
	return ret;
}

/*****************************************************************************
 Description : for device shutdown/suspend use, disable watchdog, disable clock
  History
  1.Date: 2012/9/13
    Author : x00138766
    Modification : Created function
*****************************************************************************/
static void hiwdt100_wdt_turnoff(struct platform_device *pdev)
{
	struct device *dev;
	struct hiwdt100_wdt *wdt_data;

	dev = &pdev->dev;
	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);

	/*set disable*/
	writel(UNLOCK_KEY, wdt_data->membase + WDG_LOCK);
	writel(0, wdt_data->membase + WDG_CONTROL);
	writel(0, wdt_data->membase + WDG_LOCK);

	/*set disable clock*/
	clk_disable(wdt_data->wdt_clk);

	return;
}

/*****************************************************************************
 Description : for device shutdown use, disable watchdog, disable clock
  History
  1.Date: 2012/9/13
    Author : x00138766
    Modification : Created function
*****************************************************************************/
static void hiwdt100_wdt_shutdown(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "watchdog timer shutdown\n");

	hiwdt100_wdt_turnoff(pdev);

	return;
}

/*****************************************************************************
 Description : for device suspend use, disable watchdog, disable clock
  History
  1.Date: 2012/9/13
    Author : x00138766
    Modification : Created function
*****************************************************************************/
static int hiwdt100_wdt_suspend(struct platform_device *pdev,
					pm_message_t state)
{
	dev_info(&pdev->dev, "watchdog timer suspend %d\n",
		state.event);

	hiwdt100_wdt_turnoff(pdev);

	return 0;
}

/*****************************************************************************
 Description : for device resume use, enable clock, enable watchdog
  History
  1.Date: 2012/9/13
    Author : x00138766
    Modification : Created function
*****************************************************************************/
static int hiwdt100_wdt_resume(struct platform_device *pdev)
{
	struct device *dev;
	struct hiwdt100_wdt *wdt_data;
	unsigned int val;
	int ret = 0;

	dev_info(&pdev->dev, "watchdog timer resume\n");

	dev = &pdev->dev;
	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);

	/*set enable clock*/
	ret = clk_enable(wdt_data->wdt_clk);
	if (ret) {
		dev_err(&pdev->dev, "watchdog clock enable fail\n");
		return -1;
	}

	/*set cound down value*/
	writel(UNLOCK_KEY, wdt_data->membase + WDG_LOCK);
	writel(wdt_data->count, wdt_data->membase + WDG_LOAD);
	writel(0, wdt_data->membase + WDG_LOCK);

	if (1 == wdt_data->on)
		val = ((1<<1) | (1<<0));  /*11*/
	else
		val = (1<<1);  /*10*/

	/*write any value to WDG_INTCLR to keep watchdog alive*/
	writel(UNLOCK_KEY, wdt_data->membase + WDG_LOCK);
	writel(0xffffffff,  wdt_data->membase + WDG_INTCLR);
	writel(0, wdt_data->membase + WDG_LOCK);

	/*set enable wdt ip*/
	writel(UNLOCK_KEY, wdt_data->membase + WDG_LOCK);
	writel(val, wdt_data->membase + WDG_CONTROL);
	writel(0, wdt_data->membase + WDG_LOCK);

	return 0;
}

/***************************************************************
 Description : for device remove use, disbale/relase clock,
			disable watchdog, release iomap, sysctl file
  History
  1.Date: 2012/9/13
    Author : x00138766
    Modification : Created function
***************************************************************/
static int hiwdt100_wdt_remove(struct platform_device *pdev)
{
	struct device *dev;
	struct hiwdt100_wdt *wdt_data;
	struct clk *wdt_clk;

	dev_info(&pdev->dev, "watchdog timer remove\n");

	dev = &pdev->dev;

	remove_sysfs_interfaces(dev);

	wdt_data = (struct hiwdt100_wdt *)dev_get_drvdata(dev);

	/* disable wdt ip*/
	writel(UNLOCK_KEY, wdt_data->membase + WDG_LOCK);
	writel(0, wdt_data->membase + WDG_CONTROL);
	writel(0, wdt_data->membase + WDG_LOCK);

	/*disable and release(clk_put) clock*/
	wdt_clk = wdt_data->wdt_clk;
	clk_disable(wdt_clk);
	clk_put(wdt_clk);

	/*clear irq, memmap, region*/
	devm_free_irq(dev, wdt_data->irq, dev);
	devm_iounmap(dev, wdt_data->membase);
	devm_release_mem_region(dev, wdt_data->iobase, wdt_data->size);

	kfree(wdt_data);

	return 0;
}

static struct of_device_id hiwdt100_of_match[] = {
	{ .compatible = "hisilicon,hiwdt100"},
	{ },
};
MODULE_DEVICE_TABLE(of, hiwdt100_of_match);

/* Description : platform driver definition*/
static struct platform_driver platform_hiwdt100_wdt_driver = {
	.probe    = hiwdt100_wdt_probe,
	.remove   = hiwdt100_wdt_remove,
	.shutdown = hiwdt100_wdt_shutdown,
	.suspend  = hiwdt100_wdt_suspend,
	.resume   = hiwdt100_wdt_resume,
	.driver   = {
		.owner          = THIS_MODULE,
		.name           = MODULE_NAME,
		.of_match_table = hiwdt100_of_match,
	},
};

static int __init hiwdt100_wdt_init(void)
{
	if (1 == no_hiwdt100)
		return 0;

	return platform_driver_register(&platform_hiwdt100_wdt_driver);
}

static void __exit hiwdt100_wdt_exit(void)
{
	platform_driver_unregister(&platform_hiwdt100_wdt_driver);

	return;
}

late_initcall_sync(hiwdt100_wdt_init);
module_exit(hiwdt100_wdt_exit);


MODULE_DESCRIPTION("Hisilicon WatchDog Timer hiwdt100 Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xuyiping <xuyiping@huawei.com>");
/*lint +e750*/

