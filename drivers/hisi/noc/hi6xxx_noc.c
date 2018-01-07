#include <linux/module.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/syscore_ops.h>

#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/debugfs.h>
#include <asm/io.h>
#include <linux/string.h>

#include "soc_peri_sctrl_interface.h"
#include <soc_ao_sctrl_interface.h>
#include <DrvInterface.h>

#include "hi6xxx_noc.h"
#include "hi6xxx_err_probe.h"
#include "hi6xxx_noc_packet.h"
#include "hi6xxx_noc_bus.h"

void __iomem *error_logger = NULL;
void __iomem *error_logger_media = NULL;

static struct noc_node *nodes_array[MAX_NODES_NR] = {NULL};
static struct hisi_noc_irqd noc_irqdata[NOC_MAX_IRQ_NR];
static unsigned int nodes_array_idx = 0;

static struct dentry *noc_root;
static struct dentry *u32_packet_timer_period;
static struct dentry *u32_packet_stastic_times;
static struct dentry *u32_packet_f0_master_id;
static struct dentry *u32_packet_f1_master_id;
static struct dentry *packet_on;
static struct dentry *packet_off;
static struct dentry *packet_data;

static struct noc_statstic_data *noc_statstic_datas=NULL;

static struct hrtimer hr_timer;
static unsigned int timer_irq_counter =0;
static unsigned int timer_cnt_max =10;
static unsigned int timer_period =200; //200ms
static unsigned int f0_master_id =0x7;//mcu
static unsigned int f1_master_id =0x5;//hifi

static void *noc_seq_start(struct seq_file *m, loff_t *pos)
{
    if (*pos >= timer_cnt_max)
        return NULL;
    return pos;
}

static void *noc_seq_next(struct seq_file *m, void *v, loff_t *pos)
{
    (*pos)++;
    if (*pos >= timer_cnt_max)
        return NULL;
    return pos;
}

static void noc_seq_stop(struct seq_file *m, void *v)
{

}

static int noc_seq_show(struct seq_file *m, void *v)
{
    int i;
    u64 sBeginTime;
    unsigned long nsBeginTime;
    i = *(int *)v;
    pr_info("%s,i=%d\n",__func__,i);

    if(i==0)
    {
       seq_printf(m, "time_second counter3 counter2 counter1 counter0\n");
    }

    if (noc_statstic_datas != NULL)
    {
      sBeginTime = noc_statstic_datas[i].time;
      nsBeginTime = do_div(sBeginTime, 1000000000);
      seq_printf(m, "%05lu.%06lu  %u  %u  %u %u\n",
      (unsigned long)sBeginTime,
      nsBeginTime/1000,
      noc_statstic_datas[i].counter3,
      noc_statstic_datas[i].counter2,
      noc_statstic_datas[i].counter1,
      noc_statstic_datas[i].counter0
      );
    }
    return 0;
}

static struct seq_operations packet_data_seq_ops = {
    .start = noc_seq_start,
    .next  = noc_seq_next,
    .stop  = noc_seq_stop,
    .show  = noc_seq_show
};
static int noc_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &packet_data_seq_ops);
}

static const struct file_operations packet_data_fops = {
    .open = noc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};


static enum hrtimer_restart hrtimer_callback(struct hrtimer *timer)
{

   if((timer_irq_counter < timer_cnt_max)&&(timer_cnt_max <= TIMES_MAX))
   {
      if(timer_period < TIMER_PERIOD_MAX)
      {
          hrtimer_start(timer,ktime_set(timer_period/1000,(timer_period%1000)*1000000),HRTIMER_MODE_REL);
      }

      if (noc_statstic_datas != NULL)
      {
          noc_statstic_datas[timer_irq_counter].time = sched_clock();
          noc_clear_bit(error_logger+0x2000, PACKET_CFGCTL, 0);
          noc_statstic_datas[timer_irq_counter].counter3 = readl(error_logger+0x2000 + PACKET_COUNTERS_3_VAL);
          noc_statstic_datas[timer_irq_counter].counter2 = readl(error_logger+0x2000+ PACKET_COUNTERS_2_VAL);
          noc_statstic_datas[timer_irq_counter].counter1 = readl(error_logger+0x2000 + PACKET_COUNTERS_1_VAL);
          noc_statstic_datas[timer_irq_counter].counter0 = readl(error_logger+0x2000 + PACKET_COUNTERS_0_VAL);
          noc_clear_bit(error_logger+0x2000, PACKET_MAINCTL, 3);
	      noc_set_bit(error_logger+0x2000, PACKET_MAINCTL, 3);
	      noc_set_bit(error_logger+0x2000, PACKET_CFGCTL, 0);
       }
   }
   timer_irq_counter++;
   return HRTIMER_NORESTART;
}

static ssize_t packet_off_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
   pr_info("%s +.\n", __func__);
   vfree(noc_statstic_datas);
   noc_statstic_datas =NULL;
   return (ssize_t)cnt;
}

static const struct file_operations packet_off_fops = {
    .write = packet_off_write,
};

static ssize_t packet_on_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
    timer_irq_counter = 0;

    pr_info("%s +.\n", __func__);
    /*memmory for recode noc stastic data*/

    if(timer_cnt_max > TIMES_MAX)
    {
        printk(KERN_ERR"%s : timer_cnt_max too big \n", __FUNCTION__);
        return -1;
    }

    if(noc_statstic_datas == NULL)
    {
        noc_statstic_datas = (struct noc_statstic_data *)vmalloc((timer_cnt_max+1)*sizeof(struct noc_statstic_data));
        if (noc_statstic_datas ==NULL)
        {
           printk(KERN_ERR"%s : vmalloc failed \n", __FUNCTION__);
           return -1;
        }
        memset(noc_statstic_datas,0,((timer_cnt_max+1)*sizeof(struct noc_statstic_data)));
    }

    /*select master*/
    if(error_logger != NULL)
    {
        noc_clear_bit(error_logger+0x2000, PACKET_CFGCTL, 0);
        noc_clear_bit(error_logger+0x2000, PACKET_MAINCTL, 3);
	    noc_set_bit(error_logger+0x2000, PACKET_MAINCTL, 3);

        writel((f0_master_id &0xf)<<14,error_logger+0x2000 + PACKET_F0_ROUTEIDBASE);
        writel((f1_master_id &0xf)<<14,error_logger+0x2000 + PACKET_F1_ROUTEIDBASE);
        noc_set_bit(error_logger+0x2000, PACKET_CFGCTL, 0);
     }

    if(timer_period <= TIMER_PERIOD_MAX)
    {
        hrtimer_start(&hr_timer,ktime_set(timer_period/1000,(timer_period%1000)*1000000),HRTIMER_MODE_REL);
    }
    else
    {
        printk(KERN_ERR"%s : timer_period is too big \n", __FUNCTION__);
        return -1;
    }

    return (ssize_t)cnt;
}

static const struct file_operations packet_on_fops = {
    .write = packet_on_write,
};


static irqreturn_t sysnoc_err_irq_handler(int irq, void *data)
{
    noc_err_probe_hanlder(noc_irqdata[0].node->base);
	return IRQ_HANDLED;
}

static irqreturn_t media_err_irq_handler(int irq, void *data)
{
    noc_err_probe_hanlder(noc_irqdata[1].node->base);
	return IRQ_HANDLED;
}

static irqreturn_t sysnoc_alarm_irq_handler(int irq, void *data)
{
    noc_packet_probe_hanlder(noc_irqdata[2].node, noc_irqdata[2].node->base);
	return IRQ_HANDLED;
}

static irqreturn_t media_alarm_irq_handler(int irq, void *data)
{
    noc_packet_probe_hanlder(noc_irqdata[3].node,noc_irqdata[3].node->base);
	return IRQ_HANDLED;
}

/*****************************************************************************
 函 数 名  : platform_ddr_protect_forhifi_init
 功能描述  : 初始化ddr 保护,设置不允许hifi访问的区域,并开启保护
 输入参数  : void
 输出参数  : 无
 返 回 值  : 0: 初始化成功
             -1: 初始化失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年1月21日
    作    者   : 卢彦胜 00212112
    修改内容   : 新生成函数

*****************************************************************************/
static int platform_ddr_protect_forhifi_init(void)
{

    unsigned int    uaddr_start = 0;
    unsigned int    uaddr_end = 0;
    unsigned int    ureg_value = 0;
    void __iomem *  sctrl_base = NULL;

    sctrl_base = (void __iomem *)ioremap(SOC_PERI_SCTRL_BASE_ADDR, SZ_8K);
    /*start addr and end addr of ddr to protect setting
    unit is M bytes*/

    /*soc only check high 12 bits,so we have to set like this:*/
    /*the first section*/
    uaddr_start = 0;
    uaddr_end = ((HIFI_SYS_MEM_ADDR & 0xfff00000) - 0x100000) >> 20;

    ureg_value = (uaddr_start | (uaddr_end << SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_end_addr0_START));
    writel(ureg_value, SOC_PERI_SCTRL_SC_HIFI_CTRL3_ADDR(sctrl_base));

    /*the second section*/
    uaddr_start = ((HIFI_SYS_MEM_ADDR + HIFI_SYS_MEM_SIZE) & 0xfff00000) >> 20;
    uaddr_end = (((MCU_SYS_MEM_ADDR + MCU_SYS_MEM_SIZE) & 0xfff00000) - 0x100000) >> 20;

    ureg_value = 0;
    ureg_value = (uaddr_start | (uaddr_end << SOC_PERI_SCTRL_SC_HIFI_CTRL4_hifi_access_end_addr1_START));
    writel(ureg_value, SOC_PERI_SCTRL_SC_HIFI_CTRL4_ADDR(sctrl_base));

    /*enable*/
    ureg_value = readl(SOC_PERI_SCTRL_SC_HIFI_CTRL3_ADDR(sctrl_base));
    ureg_value = ureg_value
                    | BIT(SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_permit_START)
                            | BIT(SOC_PERI_SCTRL_SC_HIFI_CTRL3_hifi_access_en_START);
    writel(ureg_value, SOC_PERI_SCTRL_SC_HIFI_CTRL3_ADDR(sctrl_base));
    iounmap(sctrl_base);
    return 0;
}
/*****************************************************************************
 Description : set sys noc for dfs.
  History
  1.Date: 2014/5/27
    Author : l00212112
    Modification : Created function
*****************************************************************************/
static  int  sysnoc_set_for_dfs(void)
{
    void __iomem * logger_ao = NULL;
    void __iomem * logger = NULL;
    void __iomem * logger_dma = NULL;

    logger_ao = (void __iomem *)ioremap(SOC_SYSNOC_SERVICE_AO_BASE_ADDR, SZ_4K);
    logger = (void __iomem *)ioremap(SOC_SYSNOC_SERVICE_BASE_ADDR, SZ_4K);
    logger_dma = (void __iomem *)ioremap(SOC_SYSNOC_SERVICE_DMA_BASE_ADDR, SZ_4K);

    /*sys noc*/
    writel(0x0, logger_ao+0x10C);
    writel(0x0404, logger_ao+0x108);
    writel(0x0505, logger_ao+0x208);
    writel(0x0, logger_ao+0x20C);

    writel(0x1, logger+0x20C);
    writel(0x0404, logger+0x208);
    writel(0x140, logger+0x210);
    writel(0x10, logger+0x214);
    writel(0x202, logger+0x308);
    writel(0x202, logger+0x408);
    writel(0x202, logger+0x508);
    writel(0x202, logger+0x608);

    writel(0x202, logger_dma+0x108);
    writel(0x202, logger_dma+0x208);
    writel(0x202, logger_dma+0x308);
    writel(0x202, logger_dma+0x408);

    printk(KERN_ERR"sysnoc_set_for_dfs end...");

    iounmap(logger_ao);
    iounmap(logger);
    iounmap(logger_dma);
	
    return 0;
}
/*****************************************************************************
 Description : set media noc for dfs.
  History
  1.Date: 2014/5/27
    Author : l00212112
    Modification : Created function
*****************************************************************************/
static  int medianoc_set_for_dfs(void)
{
    void __iomem *err_logger_video = NULL;

    printk(KERN_ERR"___medianoc set: %d", 0);
	err_logger_video = (void __iomem *)ioremap(SOC_MEDIANOC_SERVICE_VIDEO_BASE_ADDR, SZ_8K);
    if (!err_logger_video){
        printk("%s : failed to ioremap media video error logger\n", __FUNCTION__);
        return -1;
    }

    printk(KERN_ERR"0.5");
    /* 2)	Media NOC寄存器配置ISP：*/
#if 0
    unsigned int reg_val = 0;
    reg_val = readl(err_logger_video+0x104);
    printk(KERN_ERR"_____________________reg_val: 0x%x", reg_val);
    writel(0x0, err_logger_video+0x10C);
    printk(KERN_ERR"0.7");
    writel(0x0404, err_logger_video+0x108);
    printk(KERN_ERR"1");


    /* 4)	Media NOC寄存器配置Video Codec：*/
    writel(0x0, err_logger_video+0x20C);
    writel(0x0303, err_logger_video+0x208);
    writel(0x0, err_logger_video+0x30C);
    writel(0x0303, err_logger_video+0x308);
    writel(0x0, err_logger_video+0x40C);
    writel(0x0303, err_logger_video+0x408);
    printk(KERN_ERR"2");

    iounmap(err_logger_video);
#endif

    /* 在MEDIA NOC中配置ADE两个端口的QoS值方式是端口透传QoS。*/
    writel(0x2, error_logger_media+0x10C);
    writel(0x1, error_logger_media+0x118);
    writel(0x2, error_logger_media+0x20C);
    writel(0x1, error_logger_media+0x218);
    printk(KERN_ERR"3");
    return 0;

}

EXPORT_SYMBOL(medianoc_set_for_dfs);


static int find_bus_id_by_name(const char *bus_name)
{
	int i;

	for (i = 0; i < MAX_NOC_BUSES_NR; i++) {
		if (strcmp(bus_name, noc_buses_info[i].name) == 0)
			return i;
	}
	return -ENODEV;
}

static int  register_noc_node(struct device_node *np)
{
	struct noc_node *node;
	int ret = 0;
	const char *bus_name;

	node = kzalloc(sizeof(struct noc_node), GFP_KERNEL);
	if (!node) {
		pr_err("fail to alloc memory, noc_node=%s!\n", np->name);
		ret = -ENOMEM;
		goto err;
	}

	/* err probe property */
#ifdef CONFIG_HI6XXX_NOC_ERR_PROBE
	if (of_property_read_bool(np, "eprobe-autoenable")){
		node->eprobe_autoenable = true;
    }
	ret = of_property_read_u32(np, "eprobe-hwirq", &node->eprobe_hwirq);
	if (ret < 0) {
		node->eprobe_hwirq = -1;
		pr_debug("the node doesnot have err probe!\n");
	}
	if (node->eprobe_hwirq >= 0) {
		ret = of_property_read_u32(np, "eprobe-offset", &node->eprobe_offset);
		if (ret < 0) {
			node->eprobe_hwirq = -1;
			pr_debug("the node doesnot have err probe!\n");
		}
	}
#endif
	ret = of_property_read_u32(np, "interrupts-num", &node->interrupt_num);
	if (ret < 0) {
		pr_debug("the node doesnot have interrupt_num property!\n");
		ret = -ENODEV;
		goto err_iomap;
	}
    pr_info("interrupt_num =%d +++++++++++ !\n",node->interrupt_num);
    switch(node->interrupt_num)
    {
       case SYS_ERR_INT:
           node->base = error_logger;
           break;
       case MEDIA_ERR_INT:
           node->base = error_logger_media;
           break;
       case SYS_PACKET_INT:
           node->base =error_logger + 0x2000;
           break;
       case MEDIA_PACKET_INT:
           node->base =error_logger_media +0x2000;
           break;
       default:
           node->base = error_logger;
           pr_err("noc interrupt_num =%d is wrong !\n",ret);
           break;
    }

	node->name = kstrdup(np->name, GFP_KERNEL);
	if (!node->name) {
		ret = -ENOMEM;
		goto err_iomap;
	}

	ret = of_property_read_string(np, "bus-name", &bus_name);
	if (ret < 0) {
		WARN_ON(1);
		goto err_property;
	}

	ret = find_bus_id_by_name(bus_name);
	if (ret < 0) {
		WARN_ON(1);
		goto err_property;
	}
	node->bus_id = ret;

	/* FIXME: handle the transprobe & packet probe property */
	/* Debug info */
#ifdef HISI_NOC_DEBUG
	pr_debug("[%s]: nodes_array_idx = %d\n", __func__, nodes_array_idx);
	pr_debug("np->name = %s\n", np->name);
	pr_debug("node->base = 0x%p\n", node->base);
	pr_debug("node->eprobe_hwirq = %d\n", node->eprobe_hwirq);
	pr_debug("node->eprobe_offset = 0x%x\n", node->eprobe_offset);
#endif

	/* put the node into nodes_arry */
	nodes_array[nodes_array_idx] = node;
	nodes_array_idx++;

	/* FIXME: handle the other irq */

	return 0;

err_property:
	kfree(node->name);
err_iomap:
	kfree(node);
err:
	return ret;
}

static void register_irqdata(void)
{
	struct noc_node *node;
	unsigned int i;

	for (i = 0; i < nodes_array_idx; i++) {
		node = nodes_array[i];
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}
#ifdef CONFIG_HI6XXX_NOC_ERR_PROBE
		if ((node->eprobe_hwirq >= 0) && (node->eprobe_hwirq <= 1)) {
			noc_irqdata[node->eprobe_hwirq].type =  NOC_ERR_PROBE_IRQ;
			noc_irqdata[node->eprobe_hwirq].node = node;
		} else if ((node->eprobe_hwirq >= 2) && (node->eprobe_hwirq <= 3)) {
			noc_irqdata[node->eprobe_hwirq].type =  NOC_PACKET_PROBE_IRQ;
			noc_irqdata[node->eprobe_hwirq].node = node;
			init_packet_info(node);
		} else {
			pr_err("[%s]: the node type error!!!\n", __func__);
		}
#endif
	}
}
static void register_noc_nodes(void)
{
	struct device_node *np;
	int ret =0;

    for_each_compatible_node(np, NULL, "hisilicon,hi6xxx-noc-node") {
		register_noc_node(np);
	}

	register_irqdata();

    /*request irq*/
    ret = request_irq(noc_irqdata[0].node->interrupt_num, sysnoc_err_irq_handler, IRQF_NO_SUSPEND, "sysnoc_err", NULL);
    if (ret) {
        printk("%s: irq_request  %d failed!   \n",__func__,noc_irqdata[0].node->interrupt_num);
    }
    ret = request_irq(noc_irqdata[1].node->interrupt_num, media_err_irq_handler, IRQF_NO_SUSPEND, "media_err", NULL);
    if (ret) {
        printk("%s: irq_request  %d failed!   \n",__func__,noc_irqdata[1].node->interrupt_num);
    }
    ret = request_irq(noc_irqdata[2].node->interrupt_num,sysnoc_alarm_irq_handler, IRQF_NO_SUSPEND, "sysnoc_alarm", NULL);
    if (ret) {
        printk("%s: irq_request  %d failed!   \n",__func__,noc_irqdata[2].node->interrupt_num);
    }

    ret = request_irq(noc_irqdata[3].node->interrupt_num,media_alarm_irq_handler, IRQF_NO_SUSPEND, "media_alarm", NULL);
    if (ret) {
        printk("%s: irq_request  %d failed!   \n",__func__,noc_irqdata[3].node->interrupt_num);
    }
}

static void unregister_noc_nodes(void)
{
	struct noc_node *node;
	int i;

	for (i = 0; i < nodes_array_idx; i++) {
		node = nodes_array[i];
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}
#ifdef CONFIG_HI6XXX_NOC_ERR_PROBE
		if ((node->eprobe_hwirq >= 0) && (node->eprobe_hwirq <= 1)) {
			disable_err_probe(node->base + node->eprobe_offset);
		} else if ((node->eprobe_hwirq >= 2) && (node->eprobe_hwirq <= 3)) {
			disable_packet_probe(node->base + node->eprobe_offset);
		} else {
			pr_err("[%s]: the node type error!!!\n", __func__);
		}
#endif

		kfree(node);
	}

	nodes_array_idx = 0;
}


#ifdef CONFIG_HI6XXX_NOC_ERR_PROBE
void __iomem * get_errprobe_base(const char *name)
{
	struct noc_node *node;
	int i;

	for (i = 0; i < nodes_array_idx; i++) {
		node = nodes_array[i];
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}

		if (!strcmp(name, node->name))
			return node->base + node->eprobe_offset;
	}

	pr_warn("[%s]  cannot get node base name = %s\n", __func__, name);
	return NULL;
}
EXPORT_SYMBOL(get_errprobe_base);

struct noc_node * get_probe_node(const char *name)
{
	struct noc_node *node;
	int i;

	for (i = 0; i < nodes_array_idx; i++) {
		node = nodes_array[i];
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}

		if (!strcmp(name, node->name))
			return node;
	}

	pr_warn("[%s]  cannot get node base name = %s\n", __func__, name);
	return NULL;
}
EXPORT_SYMBOL(get_probe_node);

static void enable_errprobe(struct device *dev)
{
	struct noc_node *node;
	unsigned int i;

	for (i = 0; i < nodes_array_idx; i++) {
		node = nodes_array[i];
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}

		if ((node->eprobe_hwirq >= 0) && (node->eprobe_hwirq <= 1) && (node->eprobe_autoenable))
			enable_err_probe(node->base + node->eprobe_offset);
	}
}
static void disable_errprobe(struct device *dev)
{
	struct noc_node *node;
	unsigned int i;

	for (i = 0; i < nodes_array_idx; i++) {
		node = nodes_array[i];
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}

		if ((node->eprobe_hwirq >= 0) && (node->eprobe_hwirq <= 1) && (node->eprobe_autoenable))
			disable_err_probe(node->base + node->eprobe_offset);
	}
}
#ifdef DEFAULT_ENABLE_NOC_PROBE
static void enable_noc_packet_probe(struct device *dev)
{
	struct noc_node *node;
	unsigned int i;

	for (i = 0; i < nodes_array_idx; i++) {
		node = nodes_array[i];
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}
		if ((node->eprobe_hwirq >= 2) && (node->eprobe_hwirq <= 3) && (node->eprobe_autoenable))
			enable_packet_probe(node, node->base + node->eprobe_offset);
	}
}

static void disable_noc_packet_probe(struct device *dev)
{
	struct noc_node *node;
	unsigned int i;

	for (i = 0; i < nodes_array_idx; i++) {
		node = nodes_array[i];
		if (!node) {
			pr_err("[%s]: nodes_array index %d not found.\n", __func__, i);
			continue;
		}

		if ((node->eprobe_hwirq >= 2) && (node->eprobe_hwirq <= 3) && (node->eprobe_autoenable))
			disable_packet_probe(node->base + node->eprobe_offset);
	}
}
#endif
#else
static void enable_errprobe(struct device *dev)
{
}

static void disable_errprobe(struct device *dev)
{
}
#ifdef DEFAULT_ENABLE_NOC_PROBE

static void enable_noc_packet_probe(struct device *dev)
{
}

static void disable_noc_packet_probe(struct device *dev)
{
}
#endif
#endif

#ifdef CONFIG_PM_SLEEP
static int hisi_noc_suspend(struct device *dev)
{
	pr_err("%s+\n", __func__);
#ifdef CONFIG_HI6XXX_NOC_ERR_PROBE
	disable_errprobe(dev);
#ifdef DEFAULT_ENABLE_NOC_PROBE
	disable_noc_packet_probe(dev);
#endif
#endif
	pr_err("%s-\n", __func__);

	return 0;
}


static int hisi_noc_resume(struct device *dev)
{
	pr_err("%s+\n", __func__);
#ifdef CONFIG_HI6XXX_NOC_ERR_PROBE
	enable_errprobe(dev);
#ifdef DEFAULT_ENABLE_NOC_PROBE
	enable_noc_packet_probe(dev);
#endif
#endif
	pr_err("%s-\n", __func__);

	return 0;
}

static SIMPLE_DEV_PM_OPS(noc_pm_ops, hisi_noc_suspend, hisi_noc_resume);
#endif

static int hisi_noc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct hisi_noc_device *noc_dev;
	struct device_node *np;
	int ret = 0;

    pr_info("++++++++++++++ %s in\n", __func__);

	noc_dev = devm_kzalloc(&pdev->dev, sizeof(struct hisi_noc_device), GFP_KERNEL);
	if (!noc_dev) {
		dev_err(dev, "cannot get memory\n");
		ret = -ENOMEM;
		goto err;
	}

	platform_set_drvdata(pdev, noc_dev);

	np = of_find_compatible_node(NULL, NULL, "hisilicon,sysnoc-service-base-addr");
    if(np == NULL){
       pr_err("node sysnoc-service-base-addr not find !\n");
       goto err;
    }
	noc_dev->sys_base = of_iomap(np, 0);
	error_logger =noc_dev->sys_base;
	if (!noc_dev->sys_base) {
		BUG_ON(1);
		goto error_sys_base;
	}

    np = of_find_compatible_node(NULL, NULL, "hisilicon,media-service-base-addr");

	noc_dev->media_base = of_iomap(np, 0);
    error_logger_media = noc_dev->media_base;
	if (!noc_dev->media_base) {
		BUG_ON(1);
		goto error_media_base;
	}

	pr_info("++++++++++++++error_logger=%p \n", error_logger);
	pr_info("++++++++++++++error_logger_media=%p \n", error_logger_media);

	/* process each noc node */
	register_noc_nodes();

	/* enable err probe */
	enable_errprobe(dev);

    /*add error handler when panic*/
    atomic_notifier_chain_register(&panic_notifier_list, &noc_on_sys_panic_nb);
    atomic_notifier_chain_register(&panic_notifier_list, &noc_on_media_panic_nb);
#ifdef DEFAULT_ENABLE_NOC_PROBE
	/* enable noc packet probe */
	enable_noc_packet_probe(dev);
#endif
    /*set sys noc for dfs,l00212112*/
     sysnoc_set_for_dfs();

    /*for saving ADE reg value*/
    ade_reg_addr_init();

    /*Modem Noc print*/
    modem_reg_addr_init();
	return 0;
error_media_base:
    iounmap(noc_dev->media_base);
error_sys_base:
	iounmap(noc_dev->sys_base);
err:
	return ret;
}

static int hisi_noc_remove(struct platform_device *pdev)
{
	struct hisi_noc_device *noc_dev = platform_get_drvdata(pdev);

    /*remove error handler when panic*/
    atomic_notifier_chain_unregister(&panic_notifier_list, &noc_on_media_panic_nb);
    atomic_notifier_chain_unregister(&panic_notifier_list, &noc_on_sys_panic_nb);

    unregister_noc_nodes();

    error_logger =NULL;
    error_logger_media = NULL;

    if(noc_dev != NULL)
    {
	iounmap(noc_dev->media_base);
        iounmap(noc_dev->sys_base);
    }

    /*for saving ADE reg value*/
    ade_reg_addr_exit();

    return 0;
}


#ifdef CONFIG_OF
static const struct of_device_id hisi_noc_match[] = {
	{ .compatible = "hisilicon,hisi6xxx-noc" },
	{},
};
MODULE_DEVICE_TABLE(of, hisi_noc_match);
#endif

static struct platform_driver hisi_noc_driver = {
	.probe		= hisi_noc_probe,
	.remove		= hisi_noc_remove,
	.driver = {
		.name = MODULE_NAME,
		.owner = THIS_MODULE,
#ifdef CONFIG_PM_SLEEP
		.pm	= &noc_pm_ops,
#endif
#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(hisi_noc_match),
#endif
	},
};

static int __init hisi_noc_init(void)
{
    /*for hifi ddr protect*/
    platform_ddr_protect_forhifi_init();

    noc_root = debugfs_create_dir("noc", NULL);
    u32_packet_timer_period= debugfs_create_u32("packet_timer_period",0755,noc_root,&timer_period);
    u32_packet_stastic_times =debugfs_create_u32("packet_stastic_times",0755,noc_root,&timer_cnt_max);
    u32_packet_f0_master_id =debugfs_create_u32("packet_f0_master_id",0755,noc_root,&f0_master_id);
    u32_packet_f1_master_id =debugfs_create_u32("packet_f1_master_id",0755,noc_root,&f1_master_id);
    packet_on = debugfs_create_file("packet_on", S_IRUGO | S_IWUSR, noc_root, NULL, &packet_on_fops);
    packet_off = debugfs_create_file("packet_off", S_IRUGO | S_IWUSR, noc_root, NULL, &packet_off_fops);
    packet_data = debugfs_create_file("packet_data", S_IRUGO, noc_root, NULL, &packet_data_fops);

    /*add hrtimer for statistic */
    hrtimer_init(&hr_timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
    hr_timer.function =&hrtimer_callback;

	return platform_driver_register(&hisi_noc_driver);
}

static void __exit hisi_noc_exit(void)
{

    debugfs_remove(packet_data);
    debugfs_remove(packet_off);
    debugfs_remove(packet_on);
    debugfs_remove(u32_packet_stastic_times);
    debugfs_remove(u32_packet_timer_period);
    debugfs_remove(u32_packet_f0_master_id);
    debugfs_remove(u32_packet_f1_master_id);
    debugfs_remove(noc_root);
    hrtimer_cancel(&hr_timer);
    free_irq(noc_irqdata[3].node->interrupt_num,NULL);
    free_irq(noc_irqdata[2].node->interrupt_num,NULL);
    free_irq(noc_irqdata[1].node->interrupt_num,NULL);
    free_irq(noc_irqdata[0].node->interrupt_num,NULL);
    platform_driver_unregister(&hisi_noc_driver);
}

fs_initcall(hisi_noc_init);
module_exit(hisi_noc_exit);
