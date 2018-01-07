#include <linux/io.h>
#include <linux/bitops.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/bug.h>
#include <linux/delay.h>
#include <linux/notifier.h>
#include <linux/jiffies.h>
#include <linux/hisi/hi6xxx-iomap.h>
#include <linux/nmi.h>
#include "soc_sysnoc_interface.h"
#include "soc_media_noc_interface.h"
#include "soc_peri_sctrl_interface.h"

#include "hi6xxx_err_probe.h"
#include "hi6xxx_noc.h"
#include "hi6xxx_noc_bus.h"


#include "../mntn/excDrv.h"
#include "BSP.h"


#include <linux/hisi/reset.h>
extern sreset_mgr_assistant  g_reset_assistant;

/*global variables define here, starts*/
typedef enum
{
    NOC_INIT_NONE,
    NOC_INIT_MODEM,
    NOC_INIT_HIFI,
    NOC_INIT_MEDIA,
    NOC_INIT_MAX
} enoc_init_type;

static enoc_init_type s_noc_err_init_type = NOC_INIT_NONE;
/*ends*/

/*offset of ADE regs*/
static const int s_ade_regs_offset[] =
{
    0x4,
    0x78,
    0x7c,
    0x80,
    0x84,
    0x88,
    0x98,
    0x1508,
    0x1514,
    0x1588,
    0x1594
};
static void __iomem *ade_reg_baseaddr_group1 = NULL;
static void __iomem *ade_reg_baseaddr_group2 = NULL;

unsigned char*    g_ModemNocBaseAddr = NULL;
unsigned char*    g_ModemCtrlBaseAddr = NULL;


/*ends*/

#define ERR_PROBE_CLEAR_BIT	  BIT(0)
#define ERR_PORBE_ENABLE_BIT	  BIT(0)
#define ERR_PORBE_ERRVLD_BIT			BIT(0)

extern void __iomem *get_errprobe_base(const char *name);
void do_hifi_runstall(void);
void check_doreset_for_noc(void);

/*remap Modem???*/
void modem_reg_addr_init(void)
{
    unsigned int offset;
    unsigned int *reg;

    if (NULL == g_ModemNocBaseAddr)
    {
        g_ModemNocBaseAddr = (unsigned char *)ioremap(SOC_MODEMNOC_SERVICE_BASE_ADDR, SZ_1K);
        if (NULL == g_ModemNocBaseAddr)
        {
            printk(KERN_ERR "the g_ModemNocBaseAddr ioremap fail!!\n");
            return;
        }
    }
    
    if (NULL == g_ModemCtrlBaseAddr)
    {
        g_ModemCtrlBaseAddr = (unsigned char *)ioremap(SOC_MODEM_SCTRL_BASE_ADDR, SZ_1K);
        if (NULL == g_ModemCtrlBaseAddr)
        {
            printk(KERN_ERR "the g_ModemCtrlBaseAddr ioremap fail!!\n");
            return;
        }
    }
    
}
void ade_reg_addr_init(void)
{
    if (NULL == ade_reg_baseaddr_group1)
    {
        ade_reg_baseaddr_group1 = (void __iomem *)ioremap(SOC_ADE_S_BASE_ADDR, SZ_8K);
        if (NULL == ade_reg_baseaddr_group1)
        {
            printk(KERN_ERR "the first ioremap fail!!\n");
            return;
        }
    }

    if (NULL == ade_reg_baseaddr_group2)
    {
        ade_reg_baseaddr_group2 = (void __iomem *)ioremap(SOC_ADE_S_BASE_ADDR, SZ_8K);
        if (NULL == ade_reg_baseaddr_group2)
        {
            printk(KERN_ERR "the first ioremap fail!!\n");
            return;
        }
    }
}
void ade_reg_addr_exit(void)
{
    if (NULL != ade_reg_baseaddr_group1)
    {
        iounmap(ade_reg_baseaddr_group1);
        ade_reg_baseaddr_group1 = NULL;
    }

    if (NULL != ade_reg_baseaddr_group2)
    {
        iounmap(ade_reg_baseaddr_group2);
        ade_reg_baseaddr_group2 = NULL;
    }
}
/*ADE require to print these value for debug*/
/*runinstall hifi*/
void do_hifi_runstall(void)
{
    SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_UNION runstall_val;
    unsigned long sctrl_off = (unsigned long)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR);

    /* runstall hifi */
    runstall_val.value             = 0x0;
    runstall_val.reg.hifi_runstall = 0x1;

    *(volatile unsigned int *)SOC_PERI_SCTRL_SC_DSP_SUBSYS_CTRL0_ADDR(sctrl_off) = runstall_val.value;
}
/*reset system after saving log*/
void check_doreset_for_noc(void)
{
    if (check_himntn(HIMNTN_NOC_ERR_LOGGER_RESET))
    {
        switch (s_noc_err_init_type)
        {
        case NOC_INIT_MODEM:
            systemError((int)BSP_MODU_MNTN, EXCH_S_NOC, 0, 0, 0);
            break;
        case NOC_INIT_HIFI:
        /*    systemError((int)BSP_MODU_MNTN, EXCH_S_NOC, 0, 0, 0);*/
            break;
        default:
            break;
        }
    }
}
static void print_ade_reg_value(void)
{
    int i = 0;
    unsigned long addr= 0;
    unsigned long addr_phy= 0;

    int value = 0;

    int reg_size = sizeof(s_ade_regs_offset)/sizeof(s_ade_regs_offset[0]);

    printk(KERN_ERR "ade reg dump : \n");


    if (NULL == ade_reg_baseaddr_group1)
    {
        printk(KERN_ERR "ade_reg_baseaddr_group1 is NULL!!\n");
        return;
    }
    /*print the first group*/
    for (i = 0;i < reg_size;i++)
    {
        addr = (unsigned long)ade_reg_baseaddr_group1 + s_ade_regs_offset[i];
        value = *(int*)addr;
        addr_phy = SOC_ADE_S_BASE_ADDR + s_ade_regs_offset[i];
        printk(KERN_ERR "[0x%p]: 0x%x\n", (void*)addr_phy, value);
    }

    if (NULL == ade_reg_baseaddr_group2)
    {
        printk(KERN_ERR "ade_reg_baseaddr_group2 is NULL!!\n");
        return;
    }
    /*print the second group*/
    for (i = 0;i < reg_size;i++)
    {
        addr = (unsigned long)ade_reg_baseaddr_group2 + s_ade_regs_offset[i];
        addr_phy = (SOC_ADE_S_BASE_ADDR + 0x8000) + s_ade_regs_offset[i];
        printk(KERN_ERR "[0x%p]: 0x%x\n", (void*)addr_phy, value);
    }
}

static void translate_error_logger(void *base)
{
    unsigned int *reg = 0;
    unsigned int reg_init_value = 0;
    unsigned int reg_target_value = 0;

    reg = (unsigned int *)(base + 0x18);

    if (error_logger == base)
    {
        reg_init_value = ((*reg)&0x0003c000) >> 0xe;
        reg_target_value = ((*reg)&0x00003c00) >> 0xa;
        if (reg_init_value < sizeof(sysnoc_initflow_array)/sizeof(char*)
            && reg_target_value < sizeof(sysnoc_targetflow_array)/sizeof(char*))
        {
            printk(KERN_ERR "sysnoc, %s access %s:\n",sysnoc_initflow_array[reg_init_value], sysnoc_targetflow_array[reg_target_value]);
            /*if it is modem that accesses some ip, we will treat separately*/
            if (NULL != strstr((const char*)sysnoc_initflow_array[reg_init_value], (const char*)"modem"))
            {
                s_noc_err_init_type = NOC_INIT_MODEM;
            }
            /*if it is hifi that accesses some ip, we will treat separately*/
            if (NULL != strstr((const char*)sysnoc_initflow_array[reg_init_value], (const char*)"hifi"))
            {
                s_noc_err_init_type = NOC_INIT_HIFI;
            }
        }
        printk(KERN_ERR "sysnoc error logger dump:\n");

    }
    else if (error_logger_media == base)
    {
        /*print ADE reg's value*/
        print_ade_reg_value();

        reg_init_value = ((*reg)&0x0001c000) >> 0xe;
        reg_target_value = ((*reg)&0x00003c00) >> 0xa;
        if (reg_init_value < sizeof(media_initflow_array)/sizeof(char*)
            && reg_target_value < sizeof(media_targetflow_array)/sizeof(char*))
        {
            printk(KERN_ERR "medianoc, %s access %s:\n",media_initflow_array[reg_init_value], media_targetflow_array[reg_target_value]);
        }
        s_noc_err_init_type = NOC_INIT_MEDIA;
        printk(KERN_ERR "medianoc error logger dump : \n");
    }
    else
    {
        printk(KERN_ERR "other error logger dump : \n");
    }
    return;
}

static void dump_error_logger(void *base, unsigned int size)
{
    unsigned int offset;
    unsigned int *reg;

    reg = (unsigned int *)(base + 0xc);


    /*?TD§êy?Y￡??±?ó·μ???￡*/
    if (0 == *reg)
    {
        return;
    }

    translate_error_logger(base);
    for(offset = 0 ; offset < size ; offset += 4 * sizeof(unsigned int)){
        reg = (unsigned int *)((char *)base + offset);
        printk(KERN_ERR "[%08x] : %08x %08x %08x %08x\n", offset, *reg, \
            *(reg + 1), *(reg + 2), *(reg + 3));
    }

    /*如果是Modem Noc 那么打印Noc信息*/
    if(NULL != g_ModemNocBaseAddr && NOC_INIT_MODEM == s_noc_err_init_type)
    {
        printk(KERN_ERR "modem noc error logger dump:\n");
        for(offset = 0 ; offset < size ; offset += 4 * sizeof(unsigned int)){
            reg = (unsigned int *)((char *)g_ModemNocBaseAddr + offset);
            printk(KERN_ERR "[%08x] : %08x %08x %08x %08x\n", offset, *reg, \
                *(reg + 1), *(reg + 2), *(reg + 3));
        }
    }
    return;
}

void dump_err_sys_logger(void)
{
     if(error_logger != NULL)
     {
         dump_error_logger(error_logger, SZ_128);
     }
     else
     {
         printk(KERN_ERR" error_logger is NULL\n");
     }
}

EXPORT_SYMBOL(dump_err_sys_logger);

void dump_err_media_logger(void)
{
     if(error_logger_media != NULL)
     {
         dump_error_logger(error_logger_media, SZ_128);
     }
     else
     {
         printk(KERN_ERR" error_logger_meida is NULL\n");
     }
}

EXPORT_SYMBOL(dump_err_media_logger);

void copy_err_sys_logger(void * uaddr,size_t len)
{
     if( uaddr == NULL)
     {
        printk(KERN_ERR" uaddr is NULL in %s\n",__func__);
        return;
     }
     if(error_logger != NULL)
     {
         hisi_io_memcpy(uaddr, (const void * )error_logger,len);
     }
     else
     {
         printk(KERN_ERR" error_logger is NULL\n");
     }
}

EXPORT_SYMBOL(copy_err_sys_logger);

void  copy_err_media_logger(void * uaddr,size_t len)
{
     if( uaddr == NULL)
     {
         printk(KERN_ERR" uaddr is NULL in %s\n",__func__);
         return;
     }
     if(error_logger_media != NULL)
     {
         hisi_io_memcpy(uaddr, (const void * )error_logger_media,len);
     }
     else
     {
         printk(KERN_ERR" error_logger_meida is NULL\n");
     }
}

EXPORT_SYMBOL(copy_err_media_logger);

/*call_noc_onoff used by ecall*/
void call_noc_onoff(int ioper, unsigned int ionoff)
{
    switch (ioper)
    {
    case 1:/*sysnoc off*/
        if (error_logger > 0)
        {
            writel(ionoff, SOC_SYSNOC_ERR_LOGGER_FAULTEN_ADDR(error_logger));
        }
        break;
    case 2:/*media noc off*/
        if (error_logger_media > 0)
        {
            writel(ionoff, SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_FAULTEN_ADDR(error_logger_media));
        }
        break;
    case 3:/*both off*/
        if (error_logger > 0)
        {
            writel(ionoff, SOC_SYSNOC_ERR_LOGGER_FAULTEN_ADDR(error_logger));
        }
        if (error_logger_media > 0)
        {
            writel(ionoff, SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_FAULTEN_ADDR(error_logger_media));
        }
        break;
    default:
        break;
    }
}

static int noc_sys_panic_notify(struct notifier_block *self,
			  unsigned long event, void *data)
{
    dump_err_sys_logger();
	return NOTIFY_OK;
}

struct notifier_block noc_on_sys_panic_nb = {
	.notifier_call = noc_sys_panic_notify,
	.priority = INT_MAX,
};


static int noc_media_panic_notify(struct notifier_block *self,
			  unsigned long event, void *data)
{
    dump_err_media_logger();
	return NOTIFY_OK;
}

struct notifier_block noc_on_media_panic_nb = {
	.notifier_call = noc_media_panic_notify,
	.priority = INT_MAX,
};



void noc_err_probe_hanlder(void __iomem *base)
{
    if(NULL == base)
    {
        pr_err("noc err, but base is NULL!!\n");
        systemError((int)BSP_MODU_MNTN, EXCH_S_NOC, 0, base, SZ_128);
        return;
    }
    /*Modem Noc不触发全局复位，需要将该变量置NONE，避免Modem Noc之后，其他异常触发的Noc进Modem Noc流程*/
    s_noc_err_init_type = NOC_INIT_NONE;

    /*show error logger data*/
    dump_error_logger(base,SZ_128);

    if (check_himntn(HIMNTN_NOC_ERR_LOGGER_RESET))
    {
        switch (s_noc_err_init_type)
        {
        case NOC_INIT_MODEM:
            /*don't reset system now if it is modem that triger this noc irq,
            for saving modem log.
            Reset after C starts running again.*/
            break;
        case NOC_INIT_HIFI:
            up(&(g_reset_assistant.sem_wait_hifisave));

            /*runstall hifi*/
            //do_hifi_runstall();

            /*don't reset system now if it is hifi that triger this noc irq,
            for saving hifi log.
            Reset after hifi starts running again.*/
            //systemError((int)BSP_MODU_MNTN, EXCH_S_NOC, 0, base, SZ_128);
            break;
        case NOC_INIT_MEDIA:
            /*don't reset system if media noc, for printking logs*/
            break;
        default:
            systemError((int)BSP_MODU_MNTN, EXCH_S_NOC, 0, base, SZ_128);
            break;
        }
    }
    else
    {
        /*change print level*/
        console_verbose();
        trigger_all_cpu_backtrace();
    }
    /*use disable irq here, from luhua*/
	writel_relaxed(0x0, base + ERR_PORBE_FAULTEN_OFFSET);
}

void enable_err_probe(void __iomem *base)
{
	unsigned int val;
	pr_err("base=%p\n", base);
	val = readl_relaxed(base + ERR_PORBE_ERRVLD_OFFSET);
	if (val & ERR_PORBE_ERRVLD_BIT) {
			pr_err("ErrProbe happened before enabling interrupt\n");
			/* clear errvld */
			writel_relaxed(ERR_PROBE_CLEAR_BIT, base + ERR_PORBE_ERRCLR_OFFSET);
			wmb();
	}

	/* enable err probe intrrupt */
	writel_relaxed(ERR_PORBE_ENABLE_BIT, base + ERR_PORBE_FAULTEN_OFFSET);


}

void disable_err_probe(void __iomem *base)
{
	writel_relaxed(~ERR_PORBE_ENABLE_BIT, base + ERR_PORBE_FAULTEN_OFFSET);


}

void enable_err_probe_by_name(const char *name)
{
	void __iomem *base = get_errprobe_base(name);
	if (base == NULL) {
		pr_err("%s cannot get the node!\n", __func__);
		return;
	}

	enable_err_probe(base);
}
EXPORT_SYMBOL(enable_err_probe_by_name);


void disable_err_probe_by_name(char *name)
{
	void __iomem *base = get_errprobe_base(name);
	if (base == NULL) {
		pr_err("%s cannot get the node!\n", __func__);
		return;
	}

	disable_err_probe(base);
}
EXPORT_SYMBOL(disable_err_probe_by_name);
