#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <soc_irqs.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/io.h>
#include <asm/io.h>
#include <soc_mddrc_dmc_interface.h>
#include <soc_mddrc_security_interface.h>
#include <soc_mddrc_qos_interface.h>
#include <linux/hisi/hi6xxx-platform.h>
#include <linux/hisi/reg_ops.h>
#include "soc_pmctrl_interface.h"
#include "BSP.h"
#include <DrvInterface.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */


#include "../mntn/excDrv.h"
#include "soc_ao_sctrl_interface.h"
#include <soc_baseaddr_interface.h>
#include <soc_mddrc_dmc_interface.h> /* config ddr autorefresh in sram_reboot() */
#include <pwrctrl_multi_memcfg.h>


#define MEDIA_NOC_ERR_PORT 1
#define SYS_NOC_ERR_PORT   2

#define MAX_PROTECT_DDR_24M 0x1800000
#ifdef CONFIG_DEBUG_FS
typedef union
{
    u32 ddrc_his_data[14];
    struct
    {
        u32 ddrc_his_flux_wr;
        u32 ddrc_his_flux_rd;
        u32 ddrc_his_flux_wcmd;
        u32 ddrc_his_flux_rcmd;
        u32 ddrc_his_fluxid_wr;
        u32 ddrc_his_fluxid_rd;
        u32 ddrc_his_fluxid_wcmd;
        u32 ddrc_his_fluxid_rcmd;
        u32 ddrc_his_wlatcnt0;
        u32 ddrc_his_wlatcnt1;
        u32 ddrc_his_rlatcnt0;
        u32 ddrc_his_rlatcnt1;
        u32 ddrc_his_inhere_rlat_cnt;
        u32 ddrc_his_cmd_sum;
    }ddrc_reg;
} DDRC_HIS_DATA_UNION;

struct ddrc_dmc_data {
    u64 ddrc_timer;
    DDRC_HIS_DATA_UNION u_ddrc_his_data;
};

struct ddrc_info_stru
{
    unsigned int count; /* counts */
    unsigned int ddrc_intmask;
    unsigned int ddrc_rint;
    unsigned int ddrc_intstat;
    char error_logger_info[REG_ERROR_LOGGER_0_IOSIZE];
    char ddrc_sec_info[1];
};

unsigned int count;
struct ddrc_dmc_data *ddrc_datas;
struct dentry *ddrc_dmc_root;
unsigned int interval;
unsigned int ddrc_freq;
unsigned int sta_id;
unsigned int sta_id_mask;

static void __iomem *ddrc_dmc0_base_virt = 0;
static void __iomem *ddrc_sec0_base_virt = 0;
static void __iomem *ddrc_qos_base_virt = 0;
/*static unsigned int ddrc_info_save_addr = 0;*/

static DEFINE_SPINLOCK(ddrc_sec_lock);


#define DDRC_FLUX_INT    (IRQ_GIC_START + 80)
#define DDRC_SEC_INT    IRQ_DDRC_INT



#define SOC_MDDRC_DMC_DDRC_RINT_rec_err_rint_START     (3)
#define SOC_MDDRC_DMC_DDRC_RINT_tmon_err_rint_START  (6)
#define SOC_MDDRC_DMC_DDRC_RINT_recram_serr_rint_START     (29)
#define SOC_MDDRC_DMC_DDRC_RINT_recram_merr_rint_START    (30)



#define DDRC_DMC_ERROR_MASK             ((1<<SOC_MDDRC_DMC_DDRC_RINT_serr_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_rec_err_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_tmon_err_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_recram_serr_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_recram_merr_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_merr_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_sref_err_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_dimm_parity_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_rdtimeout_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_aref_alarm_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_sbram_serr_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_sbram_merr_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_rpram_serr_rint_START)| \
                                         (1<<SOC_MDDRC_DMC_DDRC_RINT_rpram_merr_rint_START))
#define MAX_DDRC_DMC_DATA               0x80000 //512
#define DDRC_SEC_LOCK(flags)            spin_lock_irqsave(&ddrc_sec_lock,flags)
#define DDRC_SEC_UNLOCK(flags)          spin_unlock_irqrestore(&ddrc_sec_lock,flags)

#define REG_BASE_DDRC_DMC0_VIRT         ddrc_dmc0_base_virt
#define REG_BASE_DDRC_SEC0_VIRT         ddrc_sec0_base_virt
#define REG_BASE_DDRC_QOS_VIRT          ddrc_qos_base_virt
#define DDRC_INFO_SAVE_ADDR             exch_get_drv_addr()


#ifdef _DRV_LLT_
#define STATIC
#else
#define STATIC    static
#endif

extern void dump_err_sys_logger(void);
extern void copy_err_sys_logger(void * uaddr,size_t len);
extern void dump_err_media_logger(void);
extern void copy_err_media_logger(void * uaddr,size_t len);
void platform_ddr_protect_24M(int portnum, int flag)
{
    unsigned long flags;
    volatile SOC_MDDRC_SECURITY_SEC_INT_EN_UNION *sec_int_en_reg;
    volatile SOC_MDDRC_SECURITY_SEC_RGN_MAP_UNION *sec_map_region1_reg,*sec_map_region2_reg;
    volatile SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_UNION *sec_rgn_region1_reg,*sec_rgn_region2_reg;
    char *base_addr = (char *)REG_BASE_DDRC_SEC0_VIRT;

#ifdef _DRV_LLT_
#else
    DDRC_SEC_LOCK(flags);

    sec_int_en_reg =(volatile SOC_MDDRC_SECURITY_SEC_INT_EN_UNION *)(SOC_MDDRC_SECURITY_SEC_INT_EN_ADDR(base_addr) + 0x400*portnum);
    sec_map_region1_reg =(volatile SOC_MDDRC_SECURITY_SEC_RGN_MAP_UNION *)(SOC_MDDRC_SECURITY_SEC_RGN_MAP_ADDR(base_addr, 0x1)+ 0x400*portnum);
    sec_map_region2_reg =(volatile SOC_MDDRC_SECURITY_SEC_RGN_MAP_UNION *)(SOC_MDDRC_SECURITY_SEC_RGN_MAP_ADDR(base_addr, 0x2)+ 0x400*portnum);
    sec_rgn_region1_reg =(volatile SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_UNION *)(SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_ADDR(base_addr, 0x1)+ 0x400*portnum);
    sec_rgn_region2_reg =(volatile SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_UNION *)(SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_ADDR(base_addr, 0x2)+ 0x400*portnum);

    sec_int_en_reg->reg.int_en =0x1; /*enable interrupt*/
    /*region0*/
    //*(volatile unsigned int*)(base_addr + 0x104 + 0x400*portnum) |= 0xf;   //y_todo:

    /*region1,addr start*/
    sec_map_region1_reg->reg.rgn_base_addr =0x0;
#ifndef BSP_COMPILE_ALLY
    if( 2 != portnum)
#endif
    {
    if (flag)/*start ddr protect*/
    {
        /*region1,addr end*/
        sec_map_region1_reg->reg.rgn_size =0x0;
        sec_rgn_region1_reg->reg.subrgn_disable =0x0;
        sec_rgn_region1_reg->reg.sp=0x0;

        sec_map_region1_reg->reg.rgn_size = 0x17;/*16M*/
        sec_map_region1_reg->reg.rgn_en =0x1;
        /*region2,addr start*/
        sec_map_region2_reg->reg.rgn_base_addr= 0x100; /*高位地址*/
        /*region2,addr end*/
        sec_map_region2_reg->reg.rgn_size =0x0;
        sec_rgn_region2_reg->reg.subrgn_disable =0x0;
        sec_rgn_region2_reg->reg.sp=0x0;

        sec_map_region2_reg->reg.rgn_size = 0x14;/*2M*/
        sec_map_region2_reg->reg.rgn_en =0x1;
        //*(volatile unsigned int*)(base_addr + 0x004 + 0x400*portnum)  &= (~0x1);    /*安全功能使能*/
    }
    else/*stop ddr protect*/
    {
        sec_map_region1_reg->reg.rgn_size =0x0;
        sec_map_region1_reg->reg.rgn_en =0x0;
        sec_rgn_region1_reg->reg.subrgn_disable =0x0;
        sec_rgn_region1_reg->reg.sp=0x0;

        sec_map_region2_reg->reg.rgn_base_addr= 0x0;; /*高位地址设为0*/
        sec_map_region2_reg->reg.rgn_size =0x0;
        sec_map_region2_reg->reg.rgn_en =0x0;
        sec_rgn_region2_reg->reg.subrgn_disable =0x0;
        sec_rgn_region2_reg->reg.sp=0x0;
        //*(volatile unsigned int*)(base_addr + 0x004 + 0x400*portnum)  |= 0x1;    /*安全功能bypass*/
    }
    }
    DDRC_SEC_UNLOCK(flags);
#endif
}


int platform_ddr_protect_init(int flag)
{

    int port = 1;

    int end_port = 4;   /*v8 有5个端口，端口0给ccpu，是否可以使用DTS*/


    pr_info("platform_ddr_protect_24M_init %d\n",flag);
    while (port <= end_port) {
        platform_ddr_protect_24M(port, flag);
        port++;
    }

    return 0;
}
int ddr_protect_init(void)
{


    ddrc_dmc0_base_virt = ioremap(SOC_MDDRC_DMC_BASE_ADDR, PAGE_ALIGN(SZ_4K)); //y_todo:是否可以使用DTS

    if (0 == ddrc_dmc0_base_virt) {
        pr_err("%s: ioremap failed\n",__func__);
        return -1;
    }


    ddrc_sec0_base_virt = ioremap(SOC_MDDRC_SECURITY_BASE_ADDR, PAGE_ALIGN(SZ_1K * 5));  //y_todo: 是否可以使用DTS

    if (0 == ddrc_sec0_base_virt) {
        iounmap(ddrc_dmc0_base_virt);
        ddrc_dmc0_base_virt = 0;
        pr_err("%s: ioremap failed\n",__func__);
        return -1;
    }
	
	ddrc_qos_base_virt = ioremap(SOC_MDDRC_QOS_BASE_ADDR, PAGE_ALIGN(SZ_4K));
	if (0 == ddrc_qos_base_virt) {
        iounmap(ddrc_sec0_base_virt);
	ddrc_sec0_base_virt = 0;
	iounmap(ddrc_dmc0_base_virt);
        ddrc_dmc0_base_virt = 0;
        pr_err("%s: ioremap failed\n",__func__);
        return -1;
    }
    /*platform_ddr_protect_init(1);*/
    return 0;
}
early_initcall(ddr_protect_init);

STATIC void dump_ddr_protect_logger(void *base, unsigned int size)
{
    unsigned int offset;
    unsigned int *reg;

    printk(KERN_ERR "ddrc logger dump : \n");

    for(offset = 0 ; offset < size ; offset += 4 * sizeof(unsigned int)){
        reg = (unsigned int *)((char *)base + offset);
        printk(KERN_ERR "[%08x] : %08x %08x %08x %08x\n", offset, *reg, \
            *(reg + 1), *(reg + 2), *(reg + 3));
    }

    return;
}

/* process the ddrc security & other events */

static irqreturn_t ddrc_events_handler(int irq, void *dev)
{
    int portnum = 1;

    int end_port = 4;

    char * base_addr = (char *)REG_BASE_DDRC_SEC0_VIRT;
    int first_invader;
    unsigned int fail_addr;
    unsigned int masterval = 0;
    unsigned int accesstype = 0;
    unsigned int ddrc_rint = 0;
    unsigned int ddrc_sec_event = 0;
	unsigned int ddrc_tmod_rint = 0;
    struct ddrc_info_stru *p_ddrc_info = (struct ddrc_info_stru *)DDRC_INFO_SAVE_ADDR;

#ifdef _DRV_LLT_
#else
    p_ddrc_info->count++;

    p_ddrc_info->ddrc_intmask = readl(SOC_MDDRC_DMC_DDRC_INTMSK_ADDR(REG_BASE_DDRC_DMC0_VIRT));
    ddrc_rint = readl(SOC_MDDRC_DMC_DDRC_RINT_ADDR(REG_BASE_DDRC_DMC0_VIRT));
    p_ddrc_info->ddrc_rint = ddrc_rint;
    p_ddrc_info->ddrc_intstat = readl(SOC_MDDRC_DMC_DDRC_INTSTS_ADDR(REG_BASE_DDRC_DMC0_VIRT));


    pr_err("%s:count=%d,ddrc_intmask=0x%x,ddrc_rint=0x%x,ddrc_intstat=0x%x\n",__func__,p_ddrc_info->count, \
            p_ddrc_info->ddrc_intmask,p_ddrc_info->ddrc_rint,p_ddrc_info->ddrc_intstat);
    pr_err("%s:count=%d,qos_intmask=0x%x,qos_rint=0x%x,qos_intstat=0x%x\n",__func__,p_ddrc_info->count, \
					readl(SOC_MDDRC_QOS_QOSB_INTMSK_ADDR(REG_BASE_DDRC_QOS_VIRT)),	\
					readl(SOC_MDDRC_QOS_QOSB_RINT_ADDR(REG_BASE_DDRC_QOS_VIRT)),	\
					readl(SOC_MDDRC_QOS_QOSB_INTSTS_ADDR(REG_BASE_DDRC_QOS_VIRT)));
	ddrc_tmod_rint = p_ddrc_info->ddrc_rint & 0xc0;
    if (ddrc_tmod_rint) {
        pr_err("warning! the DDR temperture >= 85C \n");
        unsigned int tmp = ddrc_tmod_rint | p_ddrc_info->ddrc_intmask;
        writel(tmp, SOC_MDDRC_DMC_DDRC_INTMSK_ADDR(REG_BASE_DDRC_DMC0_VIRT));

        writel(ddrc_tmod_rint, SOC_MDDRC_DMC_DDRC_RINT_ADDR(REG_BASE_DDRC_DMC0_VIRT));


        return IRQ_HANDLED;
    }

    while (portnum <= end_port) {
        if (readl(SOC_MDDRC_SECURITY_SEC_INT_STATUS_ADDR(base_addr) + 0x400*portnum)) {  /*中断状态有效*/
            fail_addr = readl(SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_0_ADDR(base_addr) + 0x400*portnum); /*记录越权地址*/
            masterval = (readl(SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_2_ADDR(base_addr) + 0x400*portnum))& 0xffffff; /*越权者*/
            accesstype = (readl (SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_ADDR(base_addr) + 0x400*portnum)) & (0x1 << 20) ;/*读/写*/


            if (fail_addr < MAX_PROTECT_DDR_24M) {/*platform_ddr_protect_32k 配置为保护32k*/
                first_invader = portnum;
                printk(KERN_ERR "ddr_protect_irq: portnum! (0x%x)\n", portnum);
                dump_ddr_protect_logger((void *)(base_addr + 0x400*portnum), 0x400);
                memcpy((&p_ddrc_info->ddrc_sec_info[0] + portnum*0x100),(char*)(base_addr + 0x400*portnum),0x100);
                ddrc_sec_event++;
                printk(KERN_ERR "ddr_protect_irq:  portnum (0x%x),addr(0x%x),master(0x%x), type(0x%x)\n", portnum, fail_addr,masterval,accesstype);

            } else {
                printk(KERN_ERR "ddr_protect_irq: invalid! portnum (0x%x),addr(0x%x),master(0x%x), type(0x%x)\n", portnum, fail_addr,masterval,accesstype);
            }

            if(portnum == MEDIA_NOC_ERR_PORT)
            {
               dump_err_media_logger();
               copy_err_media_logger(&p_ddrc_info->error_logger_info[0],REG_ERROR_LOGGER_0_IOSIZE);
            }
            else if(portnum == SYS_NOC_ERR_PORT)
            {
               dump_err_sys_logger();
               copy_err_sys_logger(&p_ddrc_info->error_logger_info[0],REG_ERROR_LOGGER_0_IOSIZE);
            }
            writel(0x1,SOC_MDDRC_SECURITY_SEC_INT_CLEAR_ADDR(base_addr) + 0x400*portnum); /*清除中断和越权失效地址*/
       }
       portnum++;
    }

    if ((ddrc_sec_event || ddrc_rint) && check_himntn(HIMNTN_ACCESS_ERROR_REBOOT)){
        systemError((int)BSP_MODU_MNTN, (int)EXCH_S_DDRC_SEC, 0, 0, 0);
    }

#endif

	return IRQ_HANDLED;
}

void ddrc_flux_irq(int irq)
{
    int reg_val = 0;
    if (ddrc_datas != NULL) {
        ddrc_datas[count].ddrc_timer = sched_clock();
        memcpy(ddrc_datas[count].u_ddrc_his_data.ddrc_his_data,
                SOC_MDDRC_DMC_DDRC_HIS_FLUX_WR_ADDR(REG_BASE_DDRC_DMC0_VIRT),14*4);
    }
    if (count < (MAX_DDRC_DMC_DATA - 1))
        count++;
    reg_val = (irq & 0x00000002) ? 1<<1:1<<9;
    writel(reg_val, SOC_MDDRC_DMC_DDRC_RINT_ADDR(REG_BASE_DDRC_DMC0_VIRT));
    writel(1, SOC_MDDRC_DMC_DDRC_CTRL_PERF_ADDR(REG_BASE_DDRC_DMC0_VIRT));
}

irqreturn_t ddrc_dmc_handle(int irq, void *dev)
{
    int ddrc_int;

    ddrc_int = readl(SOC_MDDRC_DMC_DDRC_INTSTS_ADDR(REG_BASE_DDRC_DMC0_VIRT));
    if (ddrc_int & 0x00000202) {
        ddrc_flux_irq(ddrc_int);
    }

    return IRQ_HANDLED;
}


STATIC void *ddrc_data_seq_start(struct seq_file *m, loff_t *pos)
{
    if (*pos >= count)
        return NULL;
    return pos;
}

STATIC void *ddrc_data_seq_next(struct seq_file *m, void *v, loff_t *pos)
{
    (*pos)++;
    if (*pos >= count)
        return NULL;
    return pos;
}

STATIC void ddrc_data_seq_stop(struct seq_file *m, void *v)
{
}

STATIC int ddrc_data_seq_show(struct seq_file *m, void *v)
{
    int i;
    u64 sBeginTime;
    unsigned long nsBeginTime;
    struct tm tm_rtc;
    i = *(int *)v;

    if (i == 0) {
        seq_printf(m, "sta_id:%d,sta_id_mask:%d,ddrc_freq:%d,interval:%dms\n",
                sta_id, sta_id_mask, ddrc_freq, interval);
        seq_printf(m, "index,time,rtc_time,\
ddrc_his_flux_wr,ddrc_his_flux_rd,\
ddrc_his_flux_wcmd,ddrc_his_flux_wcmd,\
ddrc_his_fluxid_wr,ddrc_his_fluxid_rd,\
ddrc_his_fluxid_wcmd,ddrc_his_fluxid_rcmd,\
ddrc_his_wlatcnt0,ddrc_his_wlatcnt1,\
ddrc_his_rlatcnt0,ddrc_his_rlatcnt1,\
ddrc_his_inhere_rlat_cnt,ddrc_his_cmd_sum\n");
    }

    if (ddrc_datas != NULL) {
        sBeginTime = ddrc_datas[i].ddrc_timer;
        nsBeginTime = do_div(sBeginTime, 1000000000);
        time_to_tm((time_t)sBeginTime, 0, &tm_rtc);
        seq_printf(m, "%d,%05lu.%06lu,", i, (unsigned long)sBeginTime, nsBeginTime/1000);
        seq_printf(m, "[%lu:%.2d:%.2d %.2d:%.2d:%.2d],",
            1900 + tm_rtc.tm_year, tm_rtc.tm_mon + 1, tm_rtc.tm_mday,
            tm_rtc.tm_hour, tm_rtc.tm_min, tm_rtc.tm_sec);
        seq_printf(m, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_flux_wr,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_flux_rd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_flux_wcmd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_flux_rcmd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_fluxid_wr,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_fluxid_rd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_fluxid_wcmd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_fluxid_rcmd,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_wlatcnt0,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_wlatcnt1,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_rlatcnt0,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_rlatcnt1,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_inhere_rlat_cnt,
            ddrc_datas[i].u_ddrc_his_data.ddrc_reg.ddrc_his_cmd_sum
        );
    }

    return 0;
}

static struct seq_operations ddrc_data_seq_ops = {
    .start = ddrc_data_seq_start,
    .next  = ddrc_data_seq_next,
    .stop  = ddrc_data_seq_stop,
    .show  = ddrc_data_seq_show
};

STATIC int ddrc_data_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &ddrc_data_seq_ops);
}

static const struct file_operations ddrc_data_fops = {
    .open = ddrc_data_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};

ssize_t ddrc_init_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
    u32 freq_interval;

    if (ddrc_datas == NULL) {
        ddrc_datas = (struct ddrc_dmc_data *)vmalloc(MAX_DDRC_DMC_DATA*sizeof(struct ddrc_dmc_data));
    }
    if (interval == 0) {
        interval = 10;
    }
    if (ddrc_freq == 0) {
        ddrc_freq = 50000;
    };
    freq_interval = ddrc_freq/16;
    freq_interval *= interval; //sft ddr 50M
    printk("%s:freq_interval:%d, interval: %dms, sta_id:%d sta_id_mask:%d\n", __func__,ddrc_freq, interval, sta_id, sta_id_mask);
    count = 0;
    //memset(ddrc_datas, 0, MAX_DDRC_DMC_DATA*sizeof(struct ddrc_dmc_data));
    writel(freq_interval | 0x10000000, SOC_MDDRC_DMC_DDRC_CFG_PERF_ADDR(REG_BASE_DDRC_DMC0_VIRT));

    writel(sta_id, SOC_MDDRC_DMC_DDRC_CFG_STAID_ADDR(REG_BASE_DDRC_DMC0_VIRT));
    writel(sta_id_mask, SOC_MDDRC_DMC_DDRC_CFG_STAIDMSK_ADDR(REG_BASE_DDRC_DMC0_VIRT));

    return (ssize_t)cnt;
}


static const struct file_operations ddrc_init_fops = {
    .write = ddrc_init_write,
};

ssize_t ddrc_exit_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
    struct ddrc_dmc_data *tmp;
    tmp = ddrc_datas;
    if (ddrc_datas != NULL) {
        ddrc_datas = 0;
        vfree(tmp);
    }
    printk("%s\n", __func__);

    return (ssize_t)cnt;
}


static const struct file_operations ddrc_exit_fops = {
    .write = ddrc_exit_write,
};

/* 0: irq is not setup
   1: irq is enable
   2: irq is disable
   */
static unsigned int irq_setup_flag = 0;

ssize_t on_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{

    int ret = -1;

    SOC_MDDRC_DMC_DDRC_INTMSK_UNION *int_mask;


    pr_info("%s +.\n", __func__);

    /* setup ddrc interrupt for statistic function */
    if (irq_setup_flag == 0)
    {
        ret = request_irq(DDRC_FLUX_INT, ddrc_dmc_handle, IRQF_NO_SUSPEND, "ddrc_dmc", NULL);
	  if (ret) {
	      printk("%s: irq_request gic_1_irq %d failed!   \n","DDRC on write", DDRC_FLUX_INT);
            return -1;
        }
        irq_setup_flag = 1; /*irq is enabled*/
    }

    if (irq_setup_flag == 2) {
        enable_irq(DDRC_FLUX_INT);
        irq_setup_flag = 1;
    }

#ifdef _DRV_LLT_
#else
    int_mask = (SOC_MDDRC_DMC_DDRC_INTMSK_UNION *)SOC_MDDRC_DMC_DDRC_INTMSK_ADDR(REG_BASE_DDRC_DMC0_VIRT);
    int_mask->reg.flux_int_mask = 0;
    writel(1, SOC_MDDRC_DMC_DDRC_CTRL_PERF_ADDR(REG_BASE_DDRC_DMC0_VIRT));
#endif

    return (ssize_t)cnt;
}


static const struct file_operations on_fops = {
    .write = on_write,
};

ssize_t off_write(struct file *filp, const char __user *ubuf, size_t cnt, loff_t *ppos)
{

    SOC_MDDRC_DMC_DDRC_INTMSK_UNION *int_mask;
    pr_info("%s +.\n", __func__);

    if (irq_setup_flag == 1) {
        disable_irq(DDRC_FLUX_INT);
        irq_setup_flag = 2;
    }

#ifdef _DRV_LLT_
#else
    int_mask = (SOC_MDDRC_DMC_DDRC_INTMSK_UNION *)SOC_MDDRC_DMC_DDRC_INTMSK_ADDR(REG_BASE_DDRC_DMC0_VIRT);
    int_mask->reg.flux_int_mask = 1;
#endif

    return (ssize_t)cnt;
}


static const struct file_operations off_fops = {
    .write = off_write,
};

void ext_ddrc_dmc_on(void)
{
    on_write(NULL, NULL, 0, NULL);
}

void ext_ddrc_dmc_init(unsigned int ext_interval)
{
    interval = ext_interval;
    ddrc_init_write(NULL, NULL, 0, NULL);
}
void ext_ddrc_dmc_off(void)
{
    off_write(NULL, NULL, 0, NULL);
}

/*测试函数，没有Unremap，仅限shell口调用*/
void write_phy_addr(void * addr, unsigned int val)
{
    int * paddr_virt = (int *)(ioremap((phys_addr_t)addr, 0x1000));

    writel(val,paddr_virt);
    iounmap(paddr_virt);
	/*测试函数，没有Unremap，仅限shell口调用*/
}

/*测试函数，没有Unremap，仅限shell口调用*/
void read_phy_addr(void * addr)
{
    int * paddr_virt = (int *)ioremap((phys_addr_t)addr, 0x1000);
    printk("read_phy_addr:(0x%p)= 0x%x\n",addr, readl(paddr_virt));
	iounmap(paddr_virt);
}

STATIC int __init ddrc_dmc_init(void)
{
    int ret = 0;

    pr_err("%s:enter! \n",__func__);


    ret = request_irq(DDRC_SEC_INT, ddrc_events_handler, IRQF_NO_SUSPEND, "ddrc_dmc_sec", NULL);
    if (ret) {
        printk("%s: irq_request  ddrc_sec_int %d failed!   \n","DDRC on write", DDRC_SEC_INT);
    }

    /*enable_irq(DDRC_SEC_INT);*/



    ret = BSP_IPC_IntConnect(IPC_ACPU_INT_SRC_MCU_DDR_EXC, (VOIDFUNCPTR)ddrc_events_handler, 0);
    if (ret){
        pr_err("failed to connect IPC irq handle (%d)\n", IPC_ACPU_INT_SRC_MCU_DDR_EXC);
    }
    BSP_IPC_IntEnable(IPC_ACPU_INT_SRC_MCU_DDR_EXC);


    ddrc_datas = NULL;

    ddrc_dmc_root = debugfs_create_dir("ddrc_dmc", NULL);

    debugfs_create_u32("interval", S_IRUGO | S_IWUSR, ddrc_dmc_root, &interval);  //y_todo: 作用
    debugfs_create_u32("ddrc_freq", S_IRUGO | S_IWUSR, ddrc_dmc_root, &ddrc_freq);
    debugfs_create_u32("sta_id", S_IRUGO | S_IWUSR, ddrc_dmc_root, &sta_id);
    debugfs_create_u32("sta_id_mask", S_IRUGO | S_IWUSR, ddrc_dmc_root, &sta_id_mask);
    debugfs_create_file("ddrc_init", S_IWUSR|S_IWGRP, ddrc_dmc_root, NULL, &ddrc_init_fops);
    debugfs_create_file("ddrc_exit", S_IWUSR|S_IWGRP, ddrc_dmc_root, NULL, &ddrc_exit_fops);
    debugfs_create_file("on", S_IWUSR, ddrc_dmc_root, NULL, &on_fops);
    debugfs_create_file("off", S_IWUSR, ddrc_dmc_root, NULL, &off_fops);
    debugfs_create_file("ddrc_data", S_IRUGO, ddrc_dmc_root, NULL, &ddrc_data_fops);

    pr_err("%s:exit! \n",__func__);

    return ret;

}

STATIC void __exit ddrc_dmc_exit(void)
{
    iounmap(REG_BASE_DDRC_DMC0_VIRT);
    iounmap(REG_BASE_DDRC_SEC0_VIRT);
    iounmap(REG_BASE_DDRC_QOS_VIRT);

    disable_irq(DDRC_SEC_INT);
}

#else
static int __init ddrc_dmc_init(void)
{
    return 0;
}
static void __exit ddrc_dmc_exit(void)
{

}
#endif
module_init(ddrc_dmc_init);
module_exit(ddrc_dmc_exit);

MODULE_DESCRIPTION("DDRC DMC Driver");
MODULE_LICENSE("GPL");

