
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/notifier.h>         /* For notifier... */
#include <linux/hisi/reset.h>
#include "soc_peri_sctrl_interface.h"
#include <soc_modem_sctrl_interface.h>
#include "soc_pmctrl_interface.h"
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#include <soc_ao_sctrl_interface.h>
#include <soc_wdt_interface.h>
#include <soc_baseaddr_interface.h>
#include <soc_mddrc_axi_interface.h>
#include <linux/io.h>
#include "MemoryMap.h"
#include "rfilesystem_process.h"
#include "rfilesystem_interface.h"
#include "BSP_IPF.h"
#include "../modem_hi6xxx/icc/bsp_icc_drv.h"
#include "../ipc_hi6xxx/bsp_drv_ipc.h"
#include "bsp_mailbox.h"
#include "../usb/usb_shell.h"
#include <linux/hisi/etb.h>
#include "excDrv.h"
#include "drv_timer.h"
#ifndef _DRV_LLT_
#include <mach/util.h>          /* For mach_call_usermoduleshell*/
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
宏以及变量定义
*****************************************************************************/
/*Ccore复位标志，寄存器值*/
#define CCORE_RESET_FLAG_VALUE            0x87654321


/*Ccore复位成功标志*/
#define CCORE_RESET_OK_FROM_CCORE         0x12345678

#if 0
/*C核狗相关值*/
#define CCORE_WDT_REG_VALUE_UNLOCK        0x1ACCE551
#define CCORE_WDT_REG_VALUE_LOCK          0x00000000
#endif

#define RESET_CBFUNC_PRIO_LEVEL_LOWT      0
#define RESET_CBFUNC_PRIO_LEVEL_HIGH      49

/*ms, time of wating mail msg reply from hifi/mcu*/
#define RESET_WAIT_TIMEOUT_MAILMSG        5000

/*ms, time of wating mail msg reply from ccpu*/
#define RESET_WAIT_CCPU_STARTUP_TIMEOUT   30000

#define RESET_CBFUN_IGNORE_NAME           "NONAME"

/*因为b090以后已经支持把printk打印信息存储到文件，所以不需要本模块自己存储
如功能开启，请检查init.rc中已mkdir log/record */
#define PRINK_TO_FILE

#ifndef NULL
#define NULL    0
#endif

#define CCORE_RESET_TASK_PHASE_SET(a)     (g_stResetDebugInfo.ulCcpuTaskPhase = (a))
#define CCORE_RESET_TASK_PHASE_GET()      (g_stResetDebugInfo.ulCcpuTaskPhase)

/*用于协助完成复位过程*/
sreset_mgr_assistant   g_reset_assistant;
/*Link used for c core reset*/
sreset_mgr_LLI  *g_pmgr_ccorereset_data = NULL;
/*Link used for hifi reset*/
sreset_mgr_LLI  *g_pmgr_hifireset_data = NULL;
/*single reset debug info*/
sreset_debug_info g_stResetDebugInfo;

static int     s_iccore_pwr_on = 0;
static void __iomem *reset_sub_mstrl = NULL; /*for modem sc register*/

extern unsigned int modem_reset_reason; /*0: reboot, 1: panic*/

/*****************************************************************************
函数声明
*****************************************************************************/
int ccorereset_enable_wdt_irq(void);
int platform_ddr_protect_init(int flag);
extern void reset_set_cpu_status(unsigned int iOff, unsigned int iflag);
extern int ccorereset_loadcbin_loadaddr(void);
extern void codec_btn_simu_func(int pressed_button);

#ifndef _DRV_LLT_
extern void check_doreset_for_noc(void);
extern void check_modem_run(void);
#endif

extern void do_hifi_runstall(void);

/*****************************************************************************
函数定义
*****************************************************************************/
IFC_GEN_CALL1(MAILBOX_IFC_ACPU_TO_CCPU_RST_MODEM, BSP_CCPU_Reset_Byhimself,
                IFC_INCNT, int, is_normal, 0)

/*****************************************************************************
 函 数 名  : mailbox_recfun_mcu
 功能描述  : 用于接收来自于MCU的MAILBOX消息
 输入参数  : void  *UserHandle, 用户句柄
            void *MailHandle, 邮箱句柄
            unsigned long MailLen, 数据长度
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
void mailbox_recfun_mcu(void  *UserHandle,void *MailHandle,unsigned int MailLen)
{
    unsigned long   ulresult = 0;
    unsigned int   msg_len = sizeof(int);
    int msg_mcu = 0;

    ulresult = mailbox_read_msg_data(MailHandle, (unsigned char *)(&msg_mcu), &msg_len);
    printk(KERN_INFO "%s: receive mail from mcu, msg_mcu = %d,result = %ld\n", __FUNCTION__, msg_mcu, ulresult);
    if (0 == msg_mcu)
    {
        up(&(g_reset_assistant.sem_wait_mcu_msg));
    }
    else
    {
        /*保存log*/
        reset_no_ok_savelog("mcu", (int)msg_mcu, DRV_RESET_CALLCBFUN_RESET_BEFORE, BSP_RESET_MODULE_CCORE);

        /*复位系统*/
        printk(KERN_ERR "%s: mcu give wrong msg, msg is %d\n", __FUNCTION__, msg_mcu);
        do_reset_system(RESET_TYPE_MSG_FROM_MCU_WRONG);
    }
}

/*****************************************************************************
 函 数 名  : mailbox_recfun_mcu_hifireset
 功能描述  : 用于接收来自于MCU的MAILBOX消息
 输入参数  : void  *UserHandle, 用户句柄
            void *MailHandle, 邮箱句柄
            unsigned long MailLen, 数据长度
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
void mailbox_recfun_mcu_hifireset(void  *UserHandle,void *MailHandle,unsigned int MailLen)
{
    unsigned long   ulresult = 0;
    unsigned int   msg_len = sizeof(int);
    int msg_mcu = 0;

    ulresult = mailbox_read_msg_data(MailHandle, (unsigned char *)(&msg_mcu), &msg_len);
    printk(KERN_INFO "%s: receive mail from mcu, msg_mcu = %d,result = %ld\n", __FUNCTION__, msg_mcu, ulresult);
    if (1 == msg_mcu)
    {
        up(&(g_reset_assistant.sem_wait_mcu_msg_hifireset));
    }
    else
    {
        /*复位系统*/
        printk(KERN_INFO "%s: mcu give wrong msg, msg is %d\n", __FUNCTION__, msg_mcu);
        do_reset_system(RESET_TYPE_MSG_FROM_MCU_WRONG);
    }
}

/*****************************************************************************
 函 数 名  : mailbox_recfun_hifi
 功能描述  : 用于接收来自于HIFI的MAILBOX消息
 输入参数  : void  *UserHandle, 用户句柄
            void *MailHandle, 邮箱句柄
            unsigned long MailLen, 数据长度
 返 回 值  : void
*****************************************************************************/
void mailbox_recfun_hifi(void  *UserHandle,void *MailHandle,unsigned int MailLen)
{
    unsigned long   ulresult = 0;
    unsigned int   msg_len = sizeof(HIFI_AP_CCPU_RESET_CNF_STRU);
    HIFI_AP_CCPU_RESET_CNF_STRU msg_hifi = {0};

    ulresult = mailbox_read_msg_data(MailHandle, (unsigned char *)(&msg_hifi), &msg_len);
    printk(KERN_INFO "%s: receive mail from hifi, msg_hifi ret = %d,result = %ld\n", __FUNCTION__, msg_hifi.uhwResult, ulresult);

    if (ID_HIFI_AP_CCPU_RESET_CNF == msg_hifi.uhwMsgId
        && 0 == msg_hifi.uhwResult)
    {
        up(&(g_reset_assistant.sem_wait_hifi_msg));
    }
    else
    {
        /*复位系统*/
        printk(KERN_ERR "%s: hifi's msg is wrong\n", __FUNCTION__);
        do_reset_system(RESET_TYPE_MSG_FROM_HIFI_WRONG);
    }
}

/*****************************************************************************
 函 数 名  : reset_do_regcbfunc
 功能描述  : 用于其它组件注册回调函数，处理Modem复位前后相关数据。
 输入参数  :
         sreset_mgr_LLI *plink,管理链表，注意，允许为空.
            const char *pname, 组件注册的名字
         pdrv_reset_cbfun cbfun,    组件注册的回调函数
         int userdata,组件的私有数据
         Int Priolevel, 回调函数调用优先级 0-49，其中0-9 保留。
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
sreset_mgr_LLI * reset_do_regcbfunc(sreset_mgr_LLI *plink, const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
    sreset_mgr_LLI  *phead = plink;
    sreset_mgr_LLI  *pmgr_unit = NULL;

    /*判断入参是否合法，不合法返回错误*/
    if (NULL == pname
        || NULL == pcbfun
        || (priolevel < RESET_CBFUNC_PRIO_LEVEL_LOWT || priolevel > RESET_CBFUNC_PRIO_LEVEL_HIGH))
    {
        printk(KERN_ERR "%s: fail in ccore reset regcb,fail, name 0x%x, cbfun 0x%x, prio %d\n", __FUNCTION__, \
                        (unsigned int)pname, (unsigned int)pcbfun, priolevel);
        return NULL;
    }

    /*分配空间*/
    pmgr_unit = (sreset_mgr_LLI*)kmalloc(sizeof(sreset_mgr_LLI), GFP_KERNEL);
    if (NULL != pmgr_unit)
    {
        hisi_io_memset((void*)pmgr_unit, 0, (sizeof(sreset_mgr_LLI)));
        /*赋值*/
        strncpy(pmgr_unit->cbfuninfo.name, pname, DRV_RESET_MODULE_NAME_LEN);
        pmgr_unit->cbfuninfo.priolevel = priolevel;
        pmgr_unit->cbfuninfo.userdata = userdata;
        pmgr_unit->cbfuninfo.cbfun = pcbfun;
    }

    /*第一次调用该函数，链表为空*/
    if (NULL == phead)
    {
        phead = pmgr_unit;
    }
    else
    {
    /*根据优先级插入链表*/
        phead = reset_link_insert(phead, pmgr_unit);
    }
    return phead;
}

/*****************************************************************************
 函 数 名  : ccorereset_regcbfunc
 功能描述  : 用于其它组件注册回调函数，处理Modem复位前后相关数据。
 输入参数  : const char *pname, 组件注册的名字
         pdrv_reset_cbfun cbfun,    组件注册的回调函数
         int userdata,组件的私有数据
         Int Priolevel, 回调函数调用优先级 0-49，其中0-9 保留。
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
    g_pmgr_ccorereset_data = reset_do_regcbfunc(g_pmgr_ccorereset_data, pname, pcbfun, userdata, priolevel);
    printk(KERN_INFO "%s: %s registered a cbfun for ccore reset\n", __FUNCTION__, pname);
    return BSP_RESET_OK;
}

/*****************************************************************************
 函 数 名  : hifireset_regcbfunc
 功能描述  : 用于其它组件注册回调函数，处理HIFI复位前后相关数据。
 输入参数  : const char *pname, 组件注册的名字
         pdrv_reset_cbfun cbfun,    组件注册的回调函数
         int userdata,组件的私有数据
         Int Priolevel, 回调函数调用优先级 0-49，其中0-9 保留。
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int hifireset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel)
{
    g_pmgr_hifireset_data = reset_do_regcbfunc(g_pmgr_hifireset_data, pname, pcbfun, userdata, priolevel);
    printk(KERN_INFO "%s: %s registered a cbfun for hifi reset\n", __FUNCTION__, pname);
    return BSP_RESET_OK;
}

/*****************************************************************************
 函 数 名  : ccorereset_task
 功能描述  : ccpu复位消息通知其他cpu。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_notify_other_cpu(struct notifier_block *cb, unsigned long code, void *p)
{
        int iResult = BSP_RESET_OK;
        /*填充消息内容*/
        g_reset_assistant.smailboxmsg_mcu.iproctec = BSP_RESET_MAILBOX_MSG_PROTECT;
        g_reset_assistant.smailboxmsg_mcu.idata = BSP_RESET_MCU_MSG_CCORE_RESET_BEFORE;
        /*消息ID*/
        g_reset_assistant.smailboxmsg_hifi.uhwMsgId = ID_AP_HIFI_CCPU_RESET_REQ;
#ifndef CONFIG_ARM64
        /*发送消息给HIFI*/
        printk(KERN_INFO "%s: msg id send to hifi is %d\n", __FUNCTION__, g_reset_assistant.smailboxmsg_hifi.uhwMsgId);

        iResult = ccorereset_tonotify(DRV_RESET_CALLCBFUN_RESET_BEFORE, BSP_RESET_MODULE_HIFI);
        if(BSP_RESET_OK != iResult)
        {
            printk(KERN_ERR "%s: fail send msg to hifi\n", __FUNCTION__);
        }
#endif
        iResult = ccorereset_tonotify(DRV_RESET_CALLCBFUN_RESET_BEFORE, BSP_RESET_MODULE_MCU);
        if(BSP_RESET_OK != iResult)
        {
            printk(KERN_ERR "%s: fail send msg to mcu\n", __FUNCTION__);
            return BSP_RESET_ERROR;
        }
        return BSP_RESET_OK;
}

/*****************************************************************************
 函 数 名  : ccorereset_task
 功能描述  : 等待其他cpu反馈。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
void ccorereset_wait_other_cpu(void)
{
#ifndef CONFIG_ARM64
#ifndef _DRV_LLT_
        if (0 != down_timeout(&(g_reset_assistant.sem_wait_hifi_msg), msecs_to_jiffies(RESET_WAIT_TIMEOUT_MAILMSG)))
        {
            printk(KERN_ERR "%s: fail to get mail from hifi,but go on\n", __FUNCTION__);
        }
#endif
#endif
#ifndef _DRV_LLT_
        if (0 != down_timeout(&(g_reset_assistant.sem_wait_mcu_msg), msecs_to_jiffies(RESET_WAIT_TIMEOUT_MAILMSG)))
        {
            /*复位系统*/
            printk(KERN_ERR "%s: fail to get mail from mcu,reset system\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_FAILGET_MSG_FROM_MCU);
            return;
        }
#endif
        printk(KERN_INFO "%s: receive mail from mcu\n", __FUNCTION__);
        printk(KERN_INFO "%s: leave\n", __FUNCTION__);
}
void ccoreset_ccpu_freq(void)
{
    unsigned int reg_val;
    void __iomem *pmctrl_base = ioremap_nocache(SOC_PMCTRL_BASE_ADDR, 0x1000);
    void __iomem *peri_sctrl_base = ioremap_nocache(SOC_PERI_SCTRL_BASE_ADDR, 0x1000);
    void __iomem *modem_sctrl_base = ioremap_nocache(SOC_MODEM_SCTRL_BASE_ADDR, 0x1000);

    /*CCPU needs set pmctrl for CCPU ctrl sys_pll*/
    writel(0x0, (void __iomem *)SOC_PMCTRL_MEDPLLALIAS_ADDR(pmctrl_base));

    /*ACPU select sys_pll for CCPU*/
    reg_val = ((0x1 << SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_ccpu_START) \
    | (0x1 << SOC_PERI_SCTRL_SC_CLK_SEL0_sel_clk_ccpu_msk_START));
    writel(reg_val, (void __iomem *)SOC_PERI_SCTRL_SC_CLK_SEL0_ADDR(peri_sctrl_base));

    /*config div0 for ccpu core pll, div2 for ccpu lbus pll*/
    reg_val = 0x3 | (0x1 << SOC_MODEM_SCTRL_MDMSC_DFS_DIVCFG0_clk_div0_ccpu_cfg_vld_START);
    writel(reg_val, (void __iomem *)SOC_MODEM_SCTRL_MDMSC_DFS_DIVCFG0_ADDR(modem_sctrl_base));

    reg_val = (0x1 << SOC_MODEM_SCTRL_MDMSC_DFS_DIVCFG2_clk_div2_ccpu_lbus_cfg_value_START) \
            | (0x1 << SOC_MODEM_SCTRL_MDMSC_DFS_DIVCFG2_clk_div2_ccpu_lbus_cfg_vld_START);
    writel(reg_val, (void __iomem *)SOC_MODEM_SCTRL_MDMSC_DFS_DIVCFG2_ADDR(modem_sctrl_base));

    iounmap(pmctrl_base);
    iounmap(peri_sctrl_base);
    iounmap(modem_sctrl_base);

	return;

}

/**
 *	reset_log_fsave - create a file
 *	@dir: the path of the file
*	@name: just name, like reset.log
*	@data: pointer to buffer to write
*	@length: length to write
 */
static int reset_log_fsave(char *dir, char *name, void *data, int length)
{
    int ret = 0;
    int fd = 0;
    unsigned long fs = 0;
    char filename[RESET_LOG_FILE_PATH_SIZE];
    
    if((NULL == dir)||(NULL == name)||(NULL == data)||(0 == length))
    {
        printk(KERN_ERR "input para error \n", filename, fd);
        return -1;
    }
    	
    if((strlen(dir) + strlen("/") + strlen(name)) >= RESET_LOG_FILE_PATH_SIZE)
    {
        printk(KERN_ERR "pathname too loooooooong , %s, %s \n", dir, name);
        return -1;
    }    

    memset(filename, 0, RESET_LOG_FILE_PATH_SIZE);

    fs = get_fs();
    set_fs(KERNEL_DS);
    
    fd = sys_open(dir, O_DIRECTORY, 0);
    if (fd < 0) { /* if dir is not exist,then create new dir */
	fd = sys_mkdir(dir, 0774);
	if (fd < 0) {
		printk(KERN_ERR "mkdir fd is 0x%x\n", fd);
		set_fs(fs);
		return fd;
	}
    }
    
    sys_close((unsigned int)fd);
        
    strncpy(filename, dir, strlen(dir));
    *(filename + strlen(dir)) = '\0';
    strncat(filename, "/", strlen("/"));
    strncat(filename, name, strlen(name));

    fd = sys_open((const char __user *)filename, (O_RDWR | O_CREAT | O_CLOEXEC), EXCH_FILE_OPEN_MODE);
    if (fd < 0) {
        printk(KERN_ERR "sys_open  fail, filename:%s, fd is %x \n", filename, fd);
        set_fs(fs);
        return fd;
    }

    ret = sys_write(fd, data, length);
    if (ret  >  0) {
        ret = sys_fsync(fd);
        if (ret < 0) {
            printk(KERN_ERR "sys_fsync fail,the ret is %x\n", ret);
            goto out;
            }
    }
    else 
    {
        printk(KERN_ERR "sys_write  fail,the ret is %x\n", ret);
        goto out;
     }
     
out:
    ret = sys_close(fd);
    if (ret < 0) {
        printk(KERN_ERR "sys_close  fail, the ret is %x\n",ret);
    }    
    set_fs(fs);
    return ret;
}

static char* get_reset_reason(unsigned int whyReboot)
{
    char * reason = NULL;
    switch (whyReboot) {
		case EXCH_CCORE_S_NORMAL:
			reason = "EXCH_S_NORMAL";
			break;
		case EXCH_CCORE_S_ARM:
			reason = "EXCH_S_ARM";
			break;
		case EXCH_CCORE_S_OS:
			reason = "EXCH_S_OS";
			break;
		case EXCH_CCORE_S_WDT:
			reason = "EXCH_S_WDT";
			break;
		case EXCH_CCORE_S_SF:
			reason = "EXCH_S_SF";
			break;
		case EXCH_CCORE_S_NOC:
			reason = "EXCH_S_NOC";
			break;
		case EXCH_CCORE_S_SD:
			reason = "EXCH_S_SD";
			break;
		case EXCH_CCORE_S_SCI:
			reason = "EXCH_S_SCI";
			break;
		case EXCH_CCORE_S_USB:
			reason = "EXCH_S_USB";
			break;
		case EXCH_CCORE_S_ONLINE:
			reason = "EXCH_S_ONLINE";
			break;
		case EXCH_CCORE_S_SSI_READ:
			reason = "EXCH_S_SSI_READ";
			break;
		case EXCH_CCORE_S_SSI_WRITE:
			reason = "EXCH_S_SSI_WRITE";
			break;
		case EXCH_CCORE_S_MODEM_REBOOT_BYRIL:
			reason = "EXCH_S_MODEM_REBOOT_BYRIL";
			break;
		case EXCH_CCORE_S_MODEM_PANIC_BYRIL:
			reason = "EXCH_S_MODEM_PANIC_BYRIL";
			break;
		case EXCH_CCORE_S_IRQ_REQUIRE_ERROR:
			reason = "EXCH_S_IRQ_REQUIRE_ERROR";
			break;
		case EXCH_CCORE_S_SC_ERR:
			reason = "EXCH_S_SC_ERR";
			break;
		case EXCH_CCORE_S_PMU:
			reason = "EXCH_S_PMU";
			break;
		case EXCH_CCORE_S_PMU_ERR:
			reason = "EXCH_S_PMU_ERR";
			break;
		default:
			reason = "UNKNOW EXCE";
			break;
		}

	return reason;
}


static char* get_reset_core(unsigned int whyReboot)
{
    if (EXCH_CCORE_S_MODEM_PANIC_BYRIL == whyReboot)
    {
        return "AP";
    }else
    {
        return "CP";
    }
}


/**
 *	save_modem_reset_info - save the modem crash info --reset.log
 *	It's very small, only 1024 bytes, can keep  in commercial version and upload to APR.
 */
void save_modem_reset_info(void)
{
    char tmp_out_buf[RESET_LOG_MAX_SIZE]={0};
    EH_ROOT_CCORE * exch_root = NULL;
    unsigned int current_int;
    int off=0, ret = 0;
    
    exch_root = (EH_ROOT_CCORE*)EXCH_C_CORE_BASE_ADDR;
    off = snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "rdr:system reboot reason: %s %s\n",
                   get_reset_reason(exch_root->arg1), get_reset_core(exch_root->arg1));
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "reset_core:%s\n", get_reset_core(exch_root->arg1));
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "reset_reason:%s\n", get_reset_reason(exch_root->arg1));
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "reboot_context:0x%x\n", exch_root->rsv1);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "reboot_task:0x%x\n", exch_root->rebootTask);
    if ( NULL == *(exch_root->taskName))
    {
        off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "task_name:%s\n", "NULL");
    }
    else
    {
        off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "task_name:%s\n", exch_root->taskName);
    }

    if (EXCH_STATE_INT == exch_root->rsv1)
    {
        current_int = exch_root->intId;
    }else
    {
        current_int = -1;
    }
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "reboot_int:0x%x\n", current_int);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "modid:0x%x\n", exch_root->modId);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "arg1:0x%x\n", exch_root->arg1);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "arg2:0x%x\n", exch_root->arg2);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "arg3:0x%x\n", exch_root->rsv2);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "arg3_len:0x%x\n", exch_root->rsv3);
    off += snprintf(tmp_out_buf+off, sizeof(tmp_out_buf), "vec:0x%x\n", exch_root->vec);

    if(RESET_LOG_MAX_SIZE > off)
        ret = reset_log_fsave(RESET_LOG_PATH, RESET_LOG_FILE, tmp_out_buf, off);
	
    if(ret < 0)
        printk(KERN_ERR "reset_log_fsave, ret is %x \n", ret);

}

/*****************************************************************************
 函 数 名  : ccorereset_task
 功能描述  : 用于处理Modem复位。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_task(void *arg)
{
    int iresult = BSP_RESET_OK;

#ifdef _DRV_LLT_
#else
    for ( ; ; )
#endif
    {
        down(&(g_reset_assistant.sem_wait_ccorereset));

        printk(KERN_ERR "%s: enter\n", __FUNCTION__);

        CCORE_RESET_TASK_PHASE_SET(0);

        /*等待其他cpu的回复,这里主要指hifi和mcu*/
        ccorereset_wait_other_cpu();

        CCORE_RESET_TASK_PHASE_SET(1);

        /*调用复位前各组件注册回调函数*/
        iresult = ccorereset_runcbfun(DRV_RESET_CALLCBFUN_RESET_BEFORE);
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to run callback fun before reset\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_RUNCB_STEP1_FAIL);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(2);

        save_modem_reset_info();

        /*复位ccpu、modem uart以及ipf等*/
        iresult = ccorereset_doreset();
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s:: fail to reset ccore\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_DORESET);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(3);

#ifdef _DRV_LLT_
#else
       /*重新加载BBE16核映像*/
        iresult = ccorereset_loadbbebin();
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to load bbe16 bin\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_LOAD_BBE16_FAIL);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(4);

       /*重新加载C核映像*/
        iresult = ccorereset_loadcbin();
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to load ccpu bin\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_LOAD_CCPU_FAIL);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(5);
#endif
        /*用于表示ccore已经复位*/
        g_reset_assistant.iccore_reset = 1;

        /*设置寄存器值,告诉ccpu这是复位后开机*/
        (void)ccorereset_setCresetFlag();

        CCORE_RESET_TASK_PHASE_SET(6);

        /*解复位ccpu*/
        (void)ccorereset_dostart();

        CCORE_RESET_TASK_PHASE_SET(7);

        //enable_etm_reset(); /*just for compile by x00195127*/
        CCORE_RESET_TASK_PHASE_SET(8);

        /*设置发送给mcu的mailbox数据值,发送消息给MCU*/
        g_reset_assistant.smailboxmsg_mcu.iproctec = BSP_RESET_MAILBOX_MSG_PROTECT;
        g_reset_assistant.smailboxmsg_mcu.idata = BSP_RESET_MCU_MSG_CCORE_RESET_AFTER;
        iresult = ccorereset_tonotify(DRV_RESET_CALLCBFUN_RESET_AFTER, BSP_RESET_MODULE_MCU);
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to notify mcu after reset\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_NOTIFY_AFTER);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(9);

        /*等待ccpu启动成功通知*/
        printk(KERN_ERR "%s: wait for ccore startup finish\n", __FUNCTION__);

        if (0 != down_timeout(&(g_reset_assistant.sem_wait_ccorereset_ok), msecs_to_jiffies(RESET_WAIT_CCPU_STARTUP_TIMEOUT)))
        {
            printk(KERN_ERR "%s: wait for ccpu startup, timeout\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_CRUN_NO_OK);
            return BSP_RESET_ERROR;
        }

        /*ccpu启动成功通知*/
        printk(KERN_ERR "%s: ccore startup finish yet\n", __FUNCTION__);

        CCORE_RESET_TASK_PHASE_SET(10);

        /*调用复位后回调函数*/
        iresult = ccorereset_runcbfun(DRV_RESET_CALLCBFUN_RESET_AFTER);
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to run after-cb fun\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_RUNCB_STEP2_FAIL);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(11);

        /*设置发送给mcu的mailbox数据值,发送消息给MCU*/
        g_reset_assistant.smailboxmsg_mcu.iproctec = BSP_RESET_MAILBOX_MSG_PROTECT;
        g_reset_assistant.smailboxmsg_mcu.idata = BSP_RESET_MCU_MSG_CCORE_RESET_FINISH;
        iresult = ccorereset_tonotify(DRV_RESET_CALLCBFUN_RESET_AFTER, BSP_RESET_MODULE_MCU);
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to notify mcu finish reset\n", __FUNCTION__);
            do_reset_system(RESET_TYPE_CRESET_NOTIFY_FINISH);
            return BSP_RESET_ERROR;
        }

        CCORE_RESET_TASK_PHASE_SET(12);
        printk(KERN_ERR "%s: leave\n", __FUNCTION__);
/*C核Noc不触发整机复位
#ifndef _DRV_LLT_
        check_doreset_for_noc();
#endif*/
		if (modem_reset_reason)
		{
			mach_call_usermodeshell("/system/etc/log/crash_notice.sh",0);/*notice LogServer to handle log-bins*/
	    }
    }
}

/**
 *	save_hifi_reset_info - save the hifi crash info -------hifi_reset.log
 *	It's 95 kbytes, can keep  in commercial version and upload to APR.
 */
 /*****************************************************************************
 函 数 名  : save_hifi_reset_info
 功能描述  : 用于处理HIFI noc时LOG的保存。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void save_hifi_reset_info(void)
{
    int dump_size = 0, ret = 0, i = 0, off = 0;

    void *hifi_dump_vir_addr = NULL;

    OM_MAIN_OBJ_INFO_STRU   *pOmMainObjInfo;
    HIFI_MAIN_OBJ_INFO_STRU  hifi_dump_info;

    /* 将hifi的物理地址映射到虚拟地址空间，并强转为hifi存储地址的结构体 */
    pOmMainObjInfo = (OM_MAIN_OBJ_INFO_STRU *)ioremap_wc(EXCH_H_CORE_BASE_ADDR, sizeof(OM_MAIN_OBJ_INFO_STRU));

    if (NULL == pOmMainObjInfo) {
        printk(KERN_ERR "%s: pOmMainObjInfo ioremap fail.\n", __FUNCTION__);
        return;
    }

    /* 检查hifi存储地址的结构体的关键字 */
    if (pOmMainObjInfo->uwFlagWord1 != HIFI_RESET_KEY_NUM_ONE || pOmMainObjInfo->uwFlagWord2 != HIFI_RESET_KEY_NUM_TWO
        || pOmMainObjInfo->uwFlagWord3 != HIFI_RESET_KEY_NUM_THREE || pOmMainObjInfo->uwFlagWord4 != HIFI_RESET_KEY_NUM_FOUR) {
        printk(KERN_ERR "%s: check protection key fail.\n", __FUNCTION__);
        goto error_remap;
    }

    /* 计算hifi的内存大小dump_size */
    for (i = 0; i < OM_MAIN_OBJ_ID_BUTT; i++) {
        dump_size += pOmMainObjInfo->uwObjSize[i]; //dump_size = 96804 Bytes
    }

    /* kmalloc一段连续内存，以保存hifi的分段内存 */
    hifi_dump_vir_addr = (void *)kmalloc(dump_size, GFP_KERNEL);
    if (NULL == hifi_dump_vir_addr) {
        printk(KERN_ERR "%s: hifi_dump_vir_addr kmalloc fail.\n", __FUNCTION__);
        goto error_remap;
    }

    memset(hifi_dump_vir_addr, 0, dump_size);
    memset(&hifi_dump_info, 0, sizeof(HIFI_MAIN_OBJ_INFO_STRU));

    for (i = 0; i < OM_MAIN_OBJ_ID_BUTT; i++) {

        /* 将hifi的虚拟地址转换为物理地址 */
        pOmMainObjInfo->uwObjAddr[i] = (pOmMainObjInfo->uwObjAddr[i] & 0x0fffffff);

        //printk(KERN_INFO "%s: pOmMainObjInfo_addr%d = 0x%x size = %d \n", __FUNCTION__, i, pOmMainObjInfo->uwObjAddr[i], pOmMainObjInfo->uwObjSize[i]);

        if (pOmMainObjInfo->uwObjAddr[i] == NULL) {
            //printk(KERN_ERR "%s: this addr is NULL, not need map.\n", __FUNCTION__);
        }
        else {

            /* 将hifi的物理地址分段映射到虚拟地址空间 */
            hifi_dump_info.uwObjAddr[i] = (VOS_UINT32)ioremap_wc(pOmMainObjInfo->uwObjAddr[i], pOmMainObjInfo->uwObjSize[i]);

            if (NULL == hifi_dump_info.uwObjAddr[i]) {
                printk(KERN_ERR "%s: hifi_dump_vir_addr ioremap fail.\n", __FUNCTION__);
                goto error_remap;
            }

            /* 将hifi的分段内存保存到一段连续内存上 */
            memcpy(hifi_dump_vir_addr + off, (void *)hifi_dump_info.uwObjAddr[i], pOmMainObjInfo->uwObjSize[i]);

            /* 保存到连续内存地址上的地址偏移量，等于dump_size */
            off += pOmMainObjInfo->uwObjSize[i];
        }
    }

    /* 将hifi内存写入文件，大小限制在1M以内 */
    ret = reset_log_fsave(HIFI_RESET_LOG_PATH, HIFI_RESET_LOG_FILE, hifi_dump_vir_addr, ((off < HIFI_RESET_LOG_MAX_SIZE) ? off : HIFI_RESET_LOG_MAX_SIZE));

    if(ret < 0) {
        printk(KERN_ERR "%s: reset_log_fsave, ret is %x \n", __FUNCTION__, ret);
    }

    /* 释放申请的一段连续内存 */
    kfree(hifi_dump_vir_addr);

error_remap:

    /* unmap申请的hifi的虚拟地址 */
    if (NULL != pOmMainObjInfo) {
        iounmap(pOmMainObjInfo);
    }

    /* unmap映射的hifi的分段的虚拟地址 */
    for (i = 0; i < OM_MAIN_OBJ_ID_BUTT; i++) {
        if (NULL != hifi_dump_info.uwObjAddr[i]) {
            iounmap(hifi_dump_info.uwObjAddr[i]);
        }
    }
}

/*****************************************************************************
 函 数 名  : hifisave_task
 功能描述  : 用于处理HIFI NOC时，HIFI的可维可测信息的保存及A核复位。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int hifisave_task(void *arg)
{
    int iresult = BSP_RESET_OK;

    for ( ; ; )
    {
        down(&(g_reset_assistant.sem_wait_hifisave));

        printk(KERN_INFO "%s: enter hifisave_task.\n", __FUNCTION__);

        save_hifi_reset_info();

        /*runstall hifi*/
        do_hifi_runstall();

        printk(KERN_INFO "%s: leaver hifisave_task and reset Acore...\n", __FUNCTION__);

        /*reset Acore*/
        systemError((int)BSP_MODU_MNTN, EXCH_S_NOC, 0, 0, 0);
    }
}

/*****************************************************************************
 函 数 名  : hifireset_task
 功能描述  : 用于处理HIFI复位。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int hifireset_task(void *arg)
{
    int iresult = BSP_RESET_OK;

#ifdef _DRV_LLT_
#else
    for ( ; ; )
#endif
    {
        down(&(g_reset_assistant.sem_wait_hifireset));

        printk(KERN_INFO "%s: enter hifireset_task\n", __FUNCTION__);

        printk(KERN_INFO "%s: hifi reset int is coming!\n", __FUNCTION__);

        reset_for_savelog("\n=============hifi reset=============\n");

        /*调用回调函数*/
        iresult = hifireset_runcbfun(DRV_RESET_CALLCBFUN_RESET_BEFORE);
        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to run cb func before hifi reset\n", __FUNCTION__);

            /*复位系统*/
            do_reset_system(RESET_TYPE_HIFIRESET_RUNCB_STEP1_FAIL);
            return BSP_RESET_ERROR;
        }

#ifdef _DRV_LLT_
#else
        if (0 != down_timeout(&(g_reset_assistant.sem_wait_mcu_msg_hifireset), msecs_to_jiffies(RESET_WAIT_TIMEOUT_MAILMSG)))
        {
            /*复位系统*/
            printk(KERN_ERR "%s: fail to get mail from mcu,reset system\n", __FUNCTION__);
            /*do_reset_system(RESET_TYPE_FAILGET_MSG_FROM_MCU);*/
            return BSP_RESET_ERROR;
        }

        printk(KERN_INFO "%s: to load hifi bin!\n", __FUNCTION__);

        reset_for_savelog("To load hifi bin\n");

       /*重新加载HIFI映像*/
        iresult = hifireset_loadhifibin();
        if (BSP_RESET_OK != iresult)
        {
            reset_for_savelog("fail to load hifi bin\n");
            printk(KERN_ERR "%s: fail to load hifi bin! reset system\n", __FUNCTION__);
            /*复位系统*/
            do_reset_system(RESET_TYPE_HIFIRESET_LOAD_BIN_FAIL);
            return BSP_RESET_ERROR;
        }
#endif

        printk(KERN_INFO "%s: to run cb fun after hifi reset!\n", __FUNCTION__);

        reset_for_savelog("To run cb func after hifi reset\n");

        /*HIFI加载完毕后，调用回调函数*/
        iresult = hifireset_runcbfun(DRV_RESET_CALLCBFUN_RESET_AFTER);

        /*恢复中断使能*/
        finish_reset_sub();

        if (BSP_RESET_OK != iresult)
        {
            printk(KERN_ERR "%s: fail to run cb fun after hifi reset! reset system\n", __FUNCTION__);
            /*复位系统*/
            do_reset_system(RESET_TYPE_HIFIRESET_RUNCB_STEP2_FAIL);
            return BSP_RESET_ERROR;
        }

        printk(KERN_INFO "%s: reset hifi successfully!\n", __FUNCTION__);

        reset_for_savelog("hifi reset ok\n");
#ifndef _DRV_LLT_
        check_doreset_for_noc();
#endif
    }
}

/*****************************************************************************
 函 数 名  : ccorereset_runcbfun
 功能描述  : Modem复位前后调用回调函数的函数。
 输入参数  : DRV_RESET_CALLCBFUN_MOMENT eparam, 0 表示modem复位前；非零表示复位后。

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int ccorereset_runcbfun (DRV_RESET_CALLCBFUN_MOMENT eparam)
{
    int  iresult = 0;

    sreset_mgr_LLI  *phead = g_pmgr_ccorereset_data;

    if (DRV_RESET_CALLCBFUN_RESET_BEFORE == eparam)
    {
        reset_set_cpu_status(1, RESET_CPU_CCORE_STATUS_OFF);
    }
    else
    {
        reset_set_cpu_status(0, RESET_CPU_CCORE_STATUS_OFF);
    }
    /*根据回调函数优先级，调用回调函数*/
    while (NULL != phead)
    {
        if (NULL != phead->cbfuninfo.cbfun)
        {
            iresult = phead->cbfuninfo.cbfun(eparam, phead->cbfuninfo.userdata);
            if (BSP_RESET_OK != iresult)
            {
                /*如果返回失败，记录下组件名字,返回值*/
                printk(KERN_ERR "%s: fail to run cbfun of %s, at %d return %d\n", __FUNCTION__, phead->cbfuninfo.name, eparam, iresult);
                break;
            }
            printk(KERN_INFO "%s: run %s cb function 0x%x success\n", __FUNCTION__, phead->cbfuninfo.name, (unsigned int)phead->cbfuninfo.cbfun);
        }
        phead = phead->pnext;
    }
    printk(KERN_INFO "%s: leave, eparam = %d, iresult = %d\n", __FUNCTION__, eparam, iresult);
    return iresult;
}

/*****************************************************************************
 函 数 名  :  hifireset_doruncbfun
 功能描述  : HIFI复位前后调用回调函数的函数。由于圈复杂度超标，所以这里封装函数
 输入参数  : DRV_RESET_CALLCBFUN_MOMENT eparam, 0 表示HIFI复位前；非零表示复位后。

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int hifireset_doruncbfun (const char *pname, DRV_RESET_CALLCBFUN_MOMENT eparam)
{
    int  iresult = BSP_RESET_OK;

    sreset_mgr_LLI  *phead = g_pmgr_hifireset_data;

    if (NULL != pname)
    {
        /*不判断模块名字,按顺序执行*/
        if (strcmp(pname, RESET_CBFUN_IGNORE_NAME) == 0)
        {
            while (NULL != phead)
            {
                if (NULL != phead->cbfuninfo.cbfun)
                {
                    iresult = phead->cbfuninfo.cbfun(eparam, phead->cbfuninfo.userdata);
                    if (BSP_RESET_OK != iresult)
                    {
                        /*如果返回失败，记录下组件名字,返回值*/
                        reset_no_ok_savelog(phead->cbfuninfo.name, iresult, eparam, BSP_RESET_MODULE_CCORE);
                        printk(KERN_ERR "%s: fail to run cbfun of %s, at %d return %d\n", __FUNCTION__, phead->cbfuninfo.name, eparam, iresult);
                        break;
                    }
                    printk(KERN_INFO "%s: run %s cb function 0x%x\n", __FUNCTION__, phead->cbfuninfo.name, (unsigned int)phead->cbfuninfo.cbfun);
                }
                phead = phead->pnext;
            }
        }
        else/*需要判断模块名字，执行指定的回调函数*/
        {
            while (NULL != phead)
            {
                if (strcmp(pname, phead->cbfuninfo.name) == 0
                    && NULL != phead->cbfuninfo.cbfun)
                {
                    iresult  = phead->cbfuninfo.cbfun(eparam, phead->cbfuninfo.userdata);
                    printk(KERN_INFO "%s: run %s cb function 0x%x\n", __FUNCTION__, phead->cbfuninfo.name, (unsigned int)phead->cbfuninfo.cbfun);
                    break;
                }
                phead = phead->pnext;
            }
        }
    }
    else
    {
        iresult = BSP_RESET_ERROR;
    }
    if (BSP_RESET_OK != iresult)
    {
        if (NULL != phead)
        {
            reset_no_ok_savelog(phead->cbfuninfo.name, iresult, eparam, BSP_RESET_MODULE_HIFI);
            printk(KERN_ERR "%s: fail to run cbfun of %s, at %d, return %d\n", __FUNCTION__, phead->cbfuninfo.name, eparam ,iresult);

        }
        else
        {
            printk(KERN_ERR "%s: fail to run cbfun, but phead or pname is null\n", __FUNCTION__);
        }
    }
    return iresult;
}

/*****************************************************************************
 函 数 名  :  hifireset _runcbfun
 功能描述  : HIFI复位前后调用回调函数的函数。
 输入参数  : DRV_RESET_CALLCBFUN_MOMENT eparam, 0 表示HIFI复位前；非零表示复位后。

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int hifireset_runcbfun (DRV_RESET_CALLCBFUN_MOMENT eparam)
{
    int  iresult = 0;

    if (DRV_RESET_CALLCBFUN_RESET_BEFORE == eparam)
    {
        /*Mailbox 处理*/
        reset_set_cpu_status(1, RESET_CPU_HIFI_STATUS_OFF);
        /*遍历回调函数链表，调用NAS的回调*/
        iresult = hifireset_doruncbfun("NAS_AT", eparam);
        if (BSP_RESET_OK != iresult)
        {
        /*如果返回失败，记录下组建name, 返回值，保存到文件*/
            goto return_error;
        }

        /*遍历回调函数链表，调用音频的回调*/
        iresult = hifireset_doruncbfun("CODEC", eparam);
        if (BSP_RESET_OK != iresult)
        {
        /*如果返回失败，记录下组建name, 返回值，保存到文件*/
            goto return_error;
        }

        /*通知MCU*/
        g_reset_assistant.smailboxmsg_mcu.iproctec = BSP_RESET_MAILBOX_MSG_PROTECT;
        g_reset_assistant.smailboxmsg_mcu.idata = BSP_RESET_MCU_MSG_HIFI_RESET_BEFORE;
        hifireset_tonotify(eparam, BSP_RESET_MODULE_MCU);
        /*遍历回调函数链表，调用OM的回调*/
        iresult = hifireset_doruncbfun("OAM", eparam);
        if (BSP_RESET_OK != iresult)
        {
        /*如果返回失败，记录下组建name, 返回值，保存到文件*/
            goto return_error;
        }
    }
    else
    {
        /*通知MCU*/
        g_reset_assistant.smailboxmsg_mcu.idata = BSP_RESET_MCU_MSG_HIFI_RESET_AFTER;
        hifireset_tonotify(eparam, BSP_RESET_MODULE_MCU);

        /*MAILBOX处理*/
        reset_set_cpu_status(0, RESET_CPU_HIFI_STATUS_OFF);

        /*遍历回调函数链表，调用回调函数*/
        iresult = hifireset_doruncbfun(RESET_CBFUN_IGNORE_NAME, eparam);
        if (BSP_RESET_OK != iresult)
        {
            goto return_error;
        }
    }
    printk(KERN_INFO "%s: end of run cb functions for hifi reset at %d, %d\n", __FUNCTION__, eparam, iresult);
    return BSP_RESET_OK;
return_error:
    return BSP_RESET_ERROR;
}
/*****************************************************************************
 函 数 名  : reset_link_insert
 功能描述  : 将数据插入链表
 输入参数  : sreset_mgr_LLI *plink, 链表指针
             sreset_mgr_LLI *punit，待插入的节点指针
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
sreset_mgr_LLI * reset_link_insert(sreset_mgr_LLI *plink, sreset_mgr_LLI *punit)
{
    sreset_mgr_LLI    *phead = plink;
    sreset_mgr_LLI    *ppose = plink;
    sreset_mgr_LLI    *ptail = plink;

    if (NULL == plink || NULL == punit)
    {
        return NULL;
    }
    while (NULL != ppose)
    {
        /*根据优先级插入到链表中*/
        if (ppose->cbfuninfo.priolevel > punit->cbfuninfo.priolevel)
        {
            if (phead == ppose)
            {
                punit->pnext = ppose;
                phead = punit;
            }
            else
            {
                ptail->pnext = punit;
                punit->pnext = ppose;
            }
            break;
        }
        ptail = ppose;
        ppose = ppose->pnext;
    }
    if (NULL == ppose)
    {
        ptail->pnext = punit;
    }
    return phead;
}

/*****************************************************************************
 函 数 名  : ccorereset_tonotify
 功能描述  : CCORE复位时，通知MCU或者HIFI
 输入参数  : DRV_RESET_CALLCBFUN_MOMENT eparam,复位前还是复位后
            int imodule, 要通知的模块，0，MCU;1,HIFI
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_tonotify(DRV_RESET_CALLCBFUN_MOMENT eparam, ereset_module emodule)
{
    unsigned long   result = 0;

    if (DRV_RESET_CALLCBFUN_RESET_BEFORE == eparam)
    {
        if (BSP_RESET_MODULE_MCU == emodule)
        {
            result = mailbox_send_msg(BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_RST_CH,
                (void *)(&g_reset_assistant.smailboxmsg_mcu), sizeof(g_reset_assistant.smailboxmsg_mcu));
        }
        else if (BSP_RESET_MODULE_HIFI == emodule)
        {
            result = mailbox_send_msg(MAILBOX_MAILCODE_ACPU_TO_HIFI_CCORE_RESET_ID,
                (void *)(&g_reset_assistant.smailboxmsg_hifi), sizeof(g_reset_assistant.smailboxmsg_hifi));
        }
    }
    else
    {
        if (BSP_RESET_MODULE_MCU == emodule)
        {
            result = mailbox_send_msg(BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_RST_CH,
                (void *)(&g_reset_assistant.smailboxmsg_mcu), sizeof(g_reset_assistant.smailboxmsg_mcu));
        }
        else
        {
            /*printk(KERN_INFO "%s: to notify none in ccorereset_tonotify\n", __FUNCTION__);*/
        }
    }
    /*printk(KERN_INFO "%s: send mail to %d, at %d, result=%d\n", __FUNCTION__, emodule, eparam, result);*/

    return ((MAILBOX_OK == result) ? BSP_RESET_OK : BSP_RESET_ERROR);
}

/*****************************************************************************
 函 数 名  : ccorereset_tonotify
 功能描述  : HIFI复位时，通知MCU
 输入参数  : DRV_RESET_CALLCBFUN_MOMENT eparam,复位前还是复位后
            int imodule, 要通知的模块，0，MCU;1,HIFI
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int hifireset_tonotify(DRV_RESET_CALLCBFUN_MOMENT eparam, ereset_module emodule)
{
    if (DRV_RESET_CALLCBFUN_RESET_BEFORE == eparam)
    {
        if (BSP_RESET_MODULE_MCU == emodule)
        {
            mailbox_send_msg(BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_RST_CH,
                (void *)(&g_reset_assistant.smailboxmsg_mcu), sizeof(g_reset_assistant.smailboxmsg_mcu));
        }
        else
        {
            printk(KERN_ERR "%s: to notify none in hifireset_tonotify\n", __FUNCTION__);
        }
    }
    else
    {
        if (BSP_RESET_MODULE_MCU == emodule)
        {
            mailbox_send_msg(BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_RST_CH,
                (void *)(&g_reset_assistant.smailboxmsg_mcu), sizeof(g_reset_assistant.smailboxmsg_mcu));
        }
        else
        {
            printk(KERN_ERR "%s: to notify none in hifireset_tonotify\n", __FUNCTION__);
        }
    }
    printk(KERN_INFO "%s: after send mail to %d, %d\n", __FUNCTION__, emodule, eparam);
    return BSP_RESET_OK;
}
/*****************************************************************************
 函 数 名  : ccorereset_setCresetFlag
 功能描述  : 设置复位CCORE标志寄存器
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_setCresetFlag(void)
{
#ifdef _DRV_LLT_
#else
#if defined(CHIP_BB_HI6210)
    void __iomem * sctrl_off = (void __iomem *)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR);
    unsigned uvalue = 0;

    uvalue = readl(SOC_PERI_SCTRL_SC_RESERVED16_ADDR(sctrl_off));
    printk(KERN_INFO"RESET LOG: ccorereset_setCresetFlag,org value1 = 0x%x\n", uvalue);

    writel(CCORE_RESET_FLAG_VALUE, SOC_PERI_SCTRL_SC_RESERVED16_ADDR(sctrl_off));
#else
    unsigned long sctrl_off = (unsigned long)IO_ADDRESS(SOC_SC_OFF_BASE_ADDR);
    unsigned long uvalue = 0;

    uvalue = readl((void __iomem *)SOC_SCTRL_SC_RESERVED16_ADDR(sctrl_off));
    printk(KERN_INFO "%s: org value = 0x%lx\n", __FUNCTION__, uvalue);

    writel(CCORE_RESET_FLAG_VALUE, (void __iomem *)SOC_SCTRL_SC_RESERVED16_ADDR(sctrl_off));
#endif

#endif
    printk(KERN_INFO "%s: leave\n", __FUNCTION__);
    return 0;
}
/*****************************************************************************
 函 数 名  : ccorereset_doreset
 功能描述  : 复位CCORE
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_doreset(void)
{
#ifdef _DRV_LLT_
#else
    void __iomem *sctrl_on  = (void __iomem *)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);
    void __iomem *sctrl_off = (void __iomem *)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR);

    unsigned  uvalue_jedge = 0;
    volatile unsigned  uvalue = 0;

    /*reset modem edma*/
    if((get_domain_access_status(ACCESS_DOMAIN_MODEM_SC) == 1)&&(NULL != reset_sub_mstrl) )
    {
        writel(BIT(SOC_MODEM_SCTRL_MDMSC_PERIPH_RSTEN_periph_rsten_mdmac_START),
                SOC_MODEM_SCTRL_MDMSC_PERIPH_RSTEN_ADDR(reset_sub_mstrl));
    }

    /*配置AO_Sctrl寄存器SC_PW_CTRL1的mdm_nsp_domain_idlereq为1'b1*/
    writel((BIT(SOC_AO_SCTRL_SC_PW_CTRL1_mdm_nsp_domain_idlereq_START)
           |BIT(SOC_AO_SCTRL_SC_PW_CTRL1_mdm_nsp_domain_idlereq_msk_START)),
            SOC_AO_SCTRL_SC_PW_CTRL1_ADDR(sctrl_on));

    /*轮询AO_Sctrl寄存器SC_PW_STAT1的mdm_nsp_domain_idle和mdm_nsp_domain_idleack，
    当其值均为"1'b1"时，说明MDM NOC隔离成功，才能对Modem子系统进行全局软复位*/
    while(1)
    {
        uvalue = readl(SOC_AO_SCTRL_SC_PW_STAT1_ADDR(sctrl_on));
        uvalue_jedge = (BIT(SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idleack_START)
                        | BIT(SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idle_START));
        if ((uvalue & uvalue_jedge) == uvalue_jedge)
        {
            break;
        }
    }

    printk(KERN_INFO"RESET LOG: ccorereset_doreset,org value1 = 0x%x\n", uvalue);

    /*配置AO_Sctrl寄存器SC_PERIPH_RSTEN4的periph_rsten4_30mdm_subsys_glb，复位整个Modem子系统*/
    writel(BIT(SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_30mdm_subsys_glb_START),
            SOC_AO_SCTRL_SC_PERIPH_RSTEN4_ADDR(sctrl_on));


    /*iso*/
    writel(BIT(SOC_AO_SCTRL_SC_PW_ISOEN0_pw_isoen0_4mcpu_START), SOC_AO_SCTRL_SC_PW_ISOEN0_ADDR(sctrl_on));

    uvalue = *(unsigned int *)(SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_ADDR(sctrl_on));
    /*mcpu外设复位*/
    if ((uvalue & BIT(SOC_AO_SCTRL_SC_PW_MTCMOS_TIMEOUT_STAT0_pw_mtcmos_timeout_stat0_0peri_START)) > 0)
    {
        writel((BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_atb_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_lbus_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_por_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_core_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_dbg_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_l2_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_socdbg_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_etm_START)
                | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_periph_rsten9_carm_dap_START)),
                SOC_PERI_SCTRL_SC_PERIPH_RSTEN9_ADDR(sctrl_off));
    }


    printk(KERN_INFO "%s: leave\n", __FUNCTION__);
#endif
    return BSP_RESET_OK;
}
/*****************************************************************************
 函 数 名  : ccorereset_enable_wdt_irq
 功能描述  : 通过写寄存器实现其他核中断到a核
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_enable_wdt_irq(void)
{
#ifdef _DRV_LLT_
#else
	unsigned long sctrl_on  = (unsigned long)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);

    unsigned long uvalue = 0;

    uvalue = readl((void __iomem *)SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_ADDR(sctrl_on));
    printk(KERN_INFO "%s: org val = 0x%lx\n", __FUNCTION__, uvalue);

    writel((uvalue | (1 << 6)), (void __iomem *)SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_ADDR(sctrl_on));
    uvalue = readl((void __iomem *)SOC_AO_SCTRL_SC_MCU_WKUP_INT_EN1_ADDR(sctrl_on));
    printk(KERN_INFO "%s: new val = 0x%lx\n", __FUNCTION__, uvalue);

    writel(0x1f, (void __iomem *)SOC_AO_SCTRL_SC_INT_EN_SET_ADDR(sctrl_on));
    uvalue = readl((void __iomem *)SOC_AO_SCTRL_SC_INT_EN_SET_ADDR(sctrl_on));

    printk(KERN_INFO "%s: new uvalue = 0x%lx\n", __FUNCTION__, uvalue);

#endif
    return 0;
}

#ifdef _DRV_LLT_
int platform_ddr_protect_init(int flag)
{
    return 0;
}
#endif

/*****************************************************************************
 函 数 名  : ccorereset_dostart
 功能描述  : 解复位CCORE
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_dostart(void)
{
#ifdef _DRV_LLT_
#else

    unsigned long sctrl_on  = (unsigned long)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);
    unsigned long sctrl_off = (unsigned long)HISI_VA_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR);

    unsigned long   iviraddr_dest1 = 0;

    int      ivalue = 0;
    unsigned int uvalue = 0;
    int      iloadmodem_addr = 0;

    /*配置AO_Sctrl寄存器SC_PERIPH_RSTDIS4的periph_rsten4_30mdm_subsys_glb
      解除Modem子系统的全局复位*/
    writel(BIT(SOC_AO_SCTRL_SC_PERIPH_RSTEN4_periph_rsten4_30mdm_subsys_glb_START),
           (void __iomem *)SOC_AO_SCTRL_SC_PERIPH_RSTDIS4_ADDR(sctrl_on));

    /*配置AO_Sctrl寄存器SC_PW_CTRL1的mdm_nsp_domain_idlereq为1'b0*/
    writel(BIT(SOC_AO_SCTRL_SC_PW_CTRL1_mdm_nsp_domain_idlereq_msk_START),
           (void __iomem *)SOC_AO_SCTRL_SC_PW_CTRL1_ADDR(sctrl_on));

    /*轮询AO_Sctrl寄存器SC_PW_STAT1的mdm_nsp_domain_idleack，当其值均为"1'b0"时，
      说明MDM NOC隔离解除，MDM NOC可以被MCU或ACPU正常访问*/
    while(1)
    {
        uvalue = readl((void __iomem *)SOC_AO_SCTRL_SC_PW_STAT1_ADDR(sctrl_on));

        if (0 == (uvalue & BIT(SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idleack_START)))
        {
            break;
        }
    }

    /* modem 的起始地址固定为零地址，因此需要在零地址写跳转指令 */
    iviraddr_dest1 = (unsigned long)(ioremap_nocache(0x00000000, sizeof(int)));
    printk(KERN_INFO "%s: addr: 0x%lx, data: 0x%x\n", __FUNCTION__, iviraddr_dest1, ivalue);

    writel(0xE59F0000, (void __iomem *)iviraddr_dest1);            /*ldr  r0, 0x8*/
    iounmap((void*)iviraddr_dest1);

    iviraddr_dest1 = (unsigned long)(ioremap_nocache(0x00000004, sizeof(int)));
    printk(KERN_INFO "%s: addr: 0x%lx, data: 0x%x\n", __FUNCTION__, iviraddr_dest1, ivalue);
    writel(0xE1A0F000, (void __iomem *)iviraddr_dest1);   /*mov  pc, r0*/
    iounmap((void*)iviraddr_dest1);

    iloadmodem_addr = ccorereset_loadcbin_loadaddr();
    iviraddr_dest1 = (unsigned long)(ioremap_nocache(0x00000008, sizeof(int)));
    printk(KERN_INFO "%s: addr: 0x%lx, data: 0x%x\n", __FUNCTION__, iviraddr_dest1, iloadmodem_addr);
    writel(iloadmodem_addr, (void __iomem *)iviraddr_dest1);   /*写入modem 镜像起始地址*/
    iounmap((void*)iviraddr_dest1);

    /*w00140341 B050 modify 配置AO_Sctrl寄存器SC_PW_CTRL打开Modem Noc时钟，
    并查询AO_Sctrl寄存器SC_PW_STAT1的mdm_nsp_domain_idleack为1'b0*/
    writel((BIT(SOC_AO_SCTRL_SC_PW_CTRL_clk_en_modemnoc_START)
           |BIT(SOC_AO_SCTRL_SC_PW_CTRL_clk_en_modemnoc_msk_START)),
           (void __iomem *)SOC_AO_SCTRL_SC_PW_CTRL_ADDR(sctrl_on));

    /*wait mdm_nsp_domain_idleack 0:idle req not ack*/
    while(1)
    {
        uvalue = readl((void __iomem *)SOC_AO_SCTRL_SC_PW_STAT1_ADDR(sctrl_on));
        if (0 == (uvalue & (BIT(SOC_AO_SCTRL_SC_PW_STAT1_mdm_nsp_domain_idleack_START))))
        {
            break;
        }
    }

	/*配置Ccpu频率为150M*/
	ccoreset_ccpu_freq();

    writel(BIT(SOC_AO_SCTRL_SC_PW_CLKEN0_pw_clken0_4mcpu_START),
           (void __iomem *)SOC_AO_SCTRL_SC_PW_CLKEN0_ADDR(sctrl_on));

    writel((BIT(SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_lbus_START)
            |BIT(SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_periph_clken9_carm_kernel_START)),
            (void __iomem *)SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_ADDR(sctrl_off));

    writel(BIT(SOC_AO_SCTRL_SC_PW_ISODIS0_pw_isodis0_4mcpu_START),
               (void __iomem *)SOC_AO_SCTRL_SC_PW_ISODIS0_ADDR(sctrl_on));

    writel(BIT(SOC_AO_SCTRL_SC_PW_RSTDIS0_pw_rstdis0_4mcpu_START),
           (void __iomem *)SOC_AO_SCTRL_SC_PW_RSTDIS0_ADDR(sctrl_on));
    /*配置AO_Sctrl寄存器SC_PW_RSTDIS0，解除CCPU分区复位。
    软件等待至少256个ARM时钟周期*/
    udelay(10);

    printk(KERN_INFO"RESET LOG: to restart\n");
    /*temp modify CCPU*/
   writel(BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_lbus_START),
          (void __iomem *)SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_ADDR(sctrl_off));

    writel((BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_dap_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_atb_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_por_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_core_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_dbg_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_l2_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_socdbg_START)
            | BIT(SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_etm_START)),
            (void __iomem *)SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_ADDR(sctrl_off));

    /* carm debug上电 */
    uvalue = *(unsigned int *)SOC_PERI_SCTRL_SC_CARM_CTRL0_ADDR(sctrl_off);
    uvalue |= (1 << SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_dbgpwrup_START);
    writel(uvalue, \
            (void __iomem *)SOC_PERI_SCTRL_SC_CARM_CTRL0_ADDR(sctrl_off));
#endif
    printk(KERN_INFO "%s: leave\n", __FUNCTION__);
    return 0;
}


/*****************************************************************************
 函 数 名  : ccorereset_icc_cok_notify
 功能描述  : 用于Modem复位成功后调用以便通知ACPU复位成功。
 输入参数  : 无

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
unsigned int ccorereset_icc_cok_notify(unsigned int chenalId,signed int u32size)
{
    unsigned int readSize;
    int ireadvalue = 0;


    /*ICC读取数据*/
    readSize = udi_read(g_reset_assistant.ccorereset_IccFd,(void*)(&ireadvalue),u32size);
    printk(KERN_INFO "%s: get icc msg from c core, readsize = %d, wantsize = %d, ireadvalue = 0x%x\n", \
                    __FUNCTION__, readSize, u32size, ireadvalue);

    if (readSize != u32size)
    {
        printk(KERN_ERR "%s: udi_read 's size is error readSize:%d VS u32size:%d\n", __FUNCTION__, readSize, u32size);
        return BSP_RESET_ERROR;
    }
    printk(KERN_INFO "%s: g_reset_assistant.iccore_reset = %d\n", \
                    __FUNCTION__, g_reset_assistant.iccore_reset);

    if (1 == g_reset_assistant.iccore_reset)
    {
        if (CCORE_RESET_OK_FROM_CCORE == ireadvalue)
        {
            printk(KERN_INFO "%s: c tell me it's ok\n", __FUNCTION__);
            /*释放ｃ核启动成功信号量*/
            up(&(g_reset_assistant.sem_wait_ccorereset_ok));
        }
        else
        {
            printk(KERN_ERR "%s: fail to run c core!return %d\n", __FUNCTION__, ireadvalue);
            /*do nothing*/
            do_reset_system(RESET_TYPE_CRESET_CRUN_NO_OK);
        }
    }
    /*非联编版本,用于冒烟时判断c核是否启动成功*/
    else
    {
        s_iccore_pwr_on = 1;
#ifndef _DRV_LLT_
        check_modem_run();
#endif

    }
    return BSP_RESET_OK;
}
#ifndef _DRV_LLT_
void check_modem_run(void)
{
    FILE    *pf = NULL;

    if (1 == s_iccore_pwr_on)
    {
        pf = BSP_fopen("/data/hisi_logs/cp_log/coredump/mdmpwron.log", "w");
        printk(KERN_INFO "%s: pf = 0x%x\n", \
                        __FUNCTION__, (unsigned int)pf);
        BSP_fclose(pf);
        pf = NULL;
    }
}
#endif
/*****************************************************************************
 函 数 名  : drv_ccorereset_cbfun
 功能描述  : 底软在Modem单独复位时用于处理IPF,ICC, FILEAGENT,CSHELL, IFC相关数据。
 输入参数  : DRV_RESET_CALLCBFUN_MOMENT eparam, 表示复位前还是复位后调用
            　int userdata,用户数据．

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int drv_ccorereset_cbfun(DRV_RESET_CALLCBFUN_MOMENT eparam, int userdata)
{
    int     iresult = 0;

#ifdef _DRV_LLT_
#else

    if (DRV_RESET_CALLCBFUN_RESET_BEFORE == eparam)
    {
        /*复位ICC通道*/
        BSP_ICC_ResetFun();

        /*调用处理IPF数据的函数*/
        BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_FORBID);
        /*清除c核ipc锁*/
        BSP_IPC_SemGive_Ccore_All();
    }
    else
    {
        BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_ALLOW);

        /*使能复位前关闭的中断*/
        finish_reset_sub();
    }
#endif

    /*调用FILEAGENT关闭句柄*/
    iresult = close_all_filehandle(eparam);
    if (0 != iresult)
    {
        printk(KERN_ERR "%s: drv_close file fail, return %d\n", __FUNCTION__, iresult);
        goto error_return;
    }

    /*调用处理CSHELL相关的函数*/
    iresult = set_for_cshell(eparam);
    if (0 != iresult)
    {
        printk(KERN_ERR "%s: drv_cshell fail, return %d\n", __FUNCTION__, iresult);
        goto error_return;
    }

    return iresult;
error_return:
    printk(KERN_ERR "%s: drv cb fun run fail, return = %d\n", __FUNCTION__, iresult);
    return BSP_RESET_ERROR;
}
/*****************************************************************************
 函 数 名  : reset_icc_open
 功能描述  : 打开icc
 输入参数  : 无

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int reset_icc_open(void)
{
    g_reset_assistant.attr.enChanMode  = ICC_CHAN_MODE_PACKET;
    g_reset_assistant.attr.u32Priority = 0;
    g_reset_assistant.attr.u32TimeOut  = 10000;
    g_reset_assistant.attr.event_cb    = NULL;
    g_reset_assistant.attr.read_cb     = ccorereset_icc_cok_notify;
    g_reset_assistant.attr.write_cb    = NULL;
    g_reset_assistant.attr.u32FIFOInSize  = 1024;
    g_reset_assistant.attr.u32FIFOOutSize = 1024;

    g_reset_assistant.ccorereset_IccParam.devid = UDI_ICC_GUOM7_ID;
    g_reset_assistant.ccorereset_IccParam.pPrivate = &(g_reset_assistant.attr);

    /*打开ICC通道*/
    g_reset_assistant.ccorereset_IccFd = udi_open(&(g_reset_assistant.ccorereset_IccParam));

    if (g_reset_assistant.ccorereset_IccFd <= 0)
    {
        printk(KERN_ERR "%s: in reset_icc_open ICC failed:[0x%x]\n", __FUNCTION__, g_reset_assistant.ccorereset_IccFd);

        return BSP_RESET_ERROR;
    }
    printk(KERN_INFO "%s: in reset_icc_open ICC SUCCESS:[0x%x]\n", __FUNCTION__, g_reset_assistant.ccorereset_IccFd);
    return BSP_RESET_OK;
}
int  set_for_cshell(DRV_RESET_CALLCBFUN_MOMENT eparam)
{
#ifdef _DRV_LLT_
#else
    if(DRV_RESET_CALLCBFUN_RESET_BEFORE == eparam)
    {
        /*cshell invalid*/
        /* bsp_ccpu_rest_cshell_handle(CSHELL_INVALID); */
    }
    else
    {
        bsp_ccpu_rest_cshell_handle(CSHELL_VALID);
    }
#endif
    return BSP_RESET_OK;
}


int  close_all_filehandle(DRV_RESET_CALLCBFUN_MOMENT eparam)
{
#ifdef _DRV_LLT_
#else
    if(DRV_RESET_CALLCBFUN_RESET_BEFORE == eparam)
    {
        bsp_ccpu_rest_rfile_handle();
    }
    else
    {
        ;
    }
#endif
    return BSP_RESET_OK;
}

/*****************************************************************************
 函 数 名  : do_reset_system
 功能描述  : 用于复位系统
 输入参数  : int ireset_type,复位类型
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
void  do_reset_system(int ireset_type)
{
#ifdef _DRV_LLT_
#else

    systemError((int)BSP_MODU_DORESET, EXCH_S_DORESET, ireset_type, 0, 0);

#endif
}
/*****************************************************************************
 函 数 名  : reset_set_cpu_status
 功能描述  : 当hifi或者ccpu复位时，记下状态，用于mailbox拦击发向hifi  的mail.
 输入参数  : unsigned int iOff  1-不使能;0-关闭
             unsigned int iflag 标识cpu状态对应的偏移
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
void reset_set_cpu_status(unsigned int iOff, unsigned int iflag)
{
    if (1 == iOff)
    {
        g_reset_assistant.icpustatus = (g_reset_assistant.icpustatus) | iflag;
    }
    else
    {
        g_reset_assistant.icpustatus = (g_reset_assistant.icpustatus) & (~iflag);
    }

    printk(KERN_INFO "%s: reset_set_cpu_status, ioff = %d, iflag = %d\n", __FUNCTION__, iOff, iflag);
}

/*****************************************************************************
 函 数 名  : BSP_CPU_StateGet
 功能描述  : 提供给mailbox模块，用于获得相关cpu状态
 输入参数  : int CpuID, MCU, HIFI,CCPU
 输出参数  : 无
 返 回 值  : int,1:使能；0:不使能
*****************************************************************************/
int BSP_CPU_StateGet(int CpuID)
{
    unsigned int u32CpuStatusOffset = 0;

    switch (CpuID)
    {
    case MAILBOX_CPUID_HIFI:
        u32CpuStatusOffset = RESET_CPU_HIFI_STATUS_OFF;
        break;
    case MAILBOX_CPUID_CCPU:
        u32CpuStatusOffset = RESET_CPU_CCORE_STATUS_OFF;
        break;
    default:
        break;
    }

    return (g_reset_assistant.icpustatus & u32CpuStatusOffset) ? 0 : 1;
}

/*****************************************************************************
 函 数 名  : reset_ccore_up_semaphone
 功能描述  : 释放ccore reset信号量
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void reset_ccore_up_semaphone(void)
{
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
    up(&(g_reset_assistant.sem_wait_ccorereset));
    printk(KERN_INFO "%s: reset_ccore_up_semaphone\n", __FUNCTION__);
#endif
}

void reset_ccore_up_byhimself(int is_normal)
{
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
    printk(KERN_INFO "%s: reset_ccore_up_byhimselfs\n", __FUNCTION__);
    BSP_CCPU_Reset_Byhimself(is_normal, 0);

#endif
}

/*****************************************************************************
 函 数 名  : reset_hifi_up_semaphone
 功能描述  : 释放hifi reset信号量
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void reset_hifi_up_semaphone(void)
{
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
    up(&(g_reset_assistant.sem_wait_hifireset));
    printk(KERN_INFO "%s: reset_hifi_up_semaphone\n", __FUNCTION__);
#endif
}

/*****************************************************************************
 函 数 名  : reset_clean_wdt_int
 功能描述  : 收到复位中断后，清理c核/hifi狗中断
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void reset_clean_wdt_int(ereset_module emodule)
{
#ifdef _DRV_LLT_
#else
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE

	unsigned long sctrl_on  = (unsigned long)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);

    switch (emodule)
    {
    case BSP_RESET_MODULE_CCORE:
        writel(0x10, (void __iomem *)SOC_AO_SCTRL_SC_INT_CLEAR_ADDR(sctrl_on));
        break;
    case BSP_RESET_MODULE_HIFI:
        writel(0x4, (void __iomem *)SOC_AO_SCTRL_SC_INT_CLEAR_ADDR(sctrl_on));
        break;
    default:
        break;
    }
    printk(KERN_INFO "%s: reset_clean_wdt_int %d\n", __FUNCTION__, emodule);
#endif
#endif
}

/*****************************************************************************
 函 数 名  : drv_hifireset_cbfun
 功能描述  : 底软在hifi单独复位时用于处理mailbox相关数据。
 输入参数  : DRV_RESET_CALLCBFUN_MOMENT eparam, 表示复位前还是复位后调用
            　int userdata,用户数据．

 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
static int drv_hifireset_cbfun(DRV_RESET_CALLCBFUN_MOMENT eparam, int userdata)
{
    int     iresult = 0;

#ifdef _DRV_LLT_
#else

    if (DRV_RESET_CALLCBFUN_RESET_BEFORE == eparam)
    {
        printk(KERN_INFO"RESET LOG: reset mediaserver task! before\n");

        mach_call_usermodeshell("/system/etc/log/mediaserver_reset.sh" , 0);

        printk(KERN_INFO"RESET LOG: reset mediaserver task! after\n");
    }
    else
    {
    }

    printk(KERN_INFO"RESET LOG: HIFI cb fun %d run ok(%d)\n", eparam, iresult);

#endif

    return iresult;

}

/*************************************************************************
可维可测接口
*************************************************************************/

/*****************************************************************************
 函 数 名  : reset_no_ok_savelog
 功能描述  : 在调用回调函数时如果回调函数返回失败，则记录下模块名字，返回值
 输入参数  : char *pname,组件注册回调函数时注册的名字；
             int iresult,回调函数的返回值。
             ereset_module emodule, 复位的模块,ccore or hifi
 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int reset_no_ok_savelog(char *pname, int iresult, DRV_RESET_CALLCBFUN_MOMENT eparam, ereset_module emodule)
{
#ifdef PRINK_TO_FILE    /*如果实现了printk打印信息生成文件功能，则不需要该函数功能*/
    return BSP_RESET_OK;
#else

    int     ilen = 0;
    char    *psavelog = NULL;
    char    *ptime = (DRV_RESET_CALLCBFUN_RESET_BEFORE == eparam?"before":"after");

    if (NULL == pname)
    {
        printk(KERN_ERR "[%s ]name is NULL, in savelog fun\n", __FUNCTION__);
        return BSP_RESET_ERROR;
    }

    psavelog = (char*)kmalloc(BSP_RESET_LOG_INFO_ITEM_LEN, GFP_KERNEL);
    if (NULL == psavelog)
    {
        printk(KERN_ERR "%s: fail to malloc, in savelog fun\n", __FUNCTION__);
        return BSP_RESET_ERROR;
    }
    hisi_io_memset((void*)psavelog, 0, BSP_RESET_LOG_INFO_ITEM_LEN);
    switch (emodule)
    {
    case BSP_RESET_MODULE_CCORE:
        sprintf(psavelog, "%s ccore reset, %s fail, return %d\n",ptime, pname, iresult);
        break;
    case BSP_RESET_MODULE_HIFI:
        sprintf(psavelog, "%s hifi reset, %s fail, return %d\n",ptime, pname, iresult);
        break;
    default:
        sprintf(psavelog, "valid module, %s fail, return %d\n",pname, iresult);
        printk(KERN_ERR "%s: module id %d invalid!!, in savelog fun\n", __FUNCTION__, emodule);
        break;

    }
    reset_for_savelog(psavelog);
    kfree(psavelog);
    psavelog = NULL;

    return BSP_RESET_OK;
#endif /* PRINK_TO_FILE */
}

/*****************************************************************************
 函 数 名  : reset_for_savelog
 功能描述  : 保存log
 输入参数  : char *pstr,log内容
 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
int reset_for_savelog(char *pstr)
{
#ifdef PRINK_TO_FILE    /*如果实现了printk打印信息生成文件功能，则不需要该函数功能*/
    return BSP_RESET_OK;
#else

#ifdef _DRV_LLT_
#else
    struct file *fd = NULL;
    mm_segment_t fs = 0;
    int iwrite = 0, ilen = 0;

    if (NULL == pstr)
    {
        printk(KERN_ERR "%s: pstr is NULL\n", __FUNCTION__);
        return BSP_RESET_ERROR;
    }

    fd = filp_open(BSP_RESET_LOG_FILE, O_RDWR | O_CREAT | O_APPEND, 0666);
    if (IS_ERR(fd))
    {
        printk(KERN_ERR "%s: file can not open\n", __FUNCTION__);
        return BSP_RESET_ERROR;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);

    ilen = strlen(pstr);
    iwrite = vfs_write(fd, (const char __user *)pstr, ilen, &(fd->f_pos));
    if (iwrite < 1)
    {
        printk(KERN_ERR "%s: can not write all %d/%d\n", __FUNCTION__, iwrite, ilen);
    }
    set_fs(fs);

    printk(KERN_INFO "%s: iwrite %d/%d\n", __FUNCTION__, iwrite, ilen);

    filp_close(fd, NULL);

#endif

    return BSP_RESET_OK;

#endif /* PRINK_TO_FILE */
}

/*****************************************************************************
 函 数 名  : reset_info_show
 功能描述  : 显示可维可测信息
 输入参数  :
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void reset_info_show(void)
{
    sreset_mgr_LLI  *phead = g_pmgr_ccorereset_data;

    while (NULL != phead)
    {
        printk(KERN_INFO "%s: name:%s cbfun:0x%8x data:%d priolevel:%d\n", __FUNCTION__, phead->cbfuninfo.name,
            (unsigned int)phead->cbfuninfo.cbfun, phead->cbfuninfo.userdata, phead->cbfuninfo.priolevel);
        phead = phead->pnext;
    }

    phead = g_pmgr_hifireset_data;
    while (NULL != phead)
    {
        printk(KERN_INFO "%s: hifi reset: name:%s cbfun:0x%8x data:%d priolevel:%d\n", __FUNCTION__, phead->cbfuninfo.name,
            (unsigned int)phead->cbfuninfo.cbfun, phead->cbfuninfo.userdata, phead->cbfuninfo.priolevel);
        phead = phead->pnext;
    }

    /*C核单独复位状态*/
    printk(KERN_INFO "%s: ccore reset task process %d phase\n", __FUNCTION__, CCORE_RESET_TASK_PHASE_GET());
}

/*测试用接口*/
#ifdef BSP_C_HIFI_RESET_ALONE_TESET_VALID
/*These functions are used for testing!!!only!!!, starts*/
extern int simulate_irq_c_freeze(void);
extern int simulate_irq_c_panic(void);
extern int simulate_irq_c_reset(int is_normal);
extern int simulate_irq_hifi_reset(void);
extern int simulate_irq_mcuwdt_reset(void);

#ifdef _DRV_LLT_
#else
extern BSP_S32 BSP_IPF_ST_101(BSP_U32 u32PacketNum);
#endif

void test_clean_mgr_ccore_link(void)
{
    sreset_mgr_LLI  *phead = g_pmgr_ccorereset_data;
    sreset_mgr_LLI  *ptail = g_pmgr_ccorereset_data;

    while (NULL != phead)
    {
        ptail = phead->pnext;
        kfree(phead);
        phead = ptail;
    }
    g_pmgr_ccorereset_data = NULL;
}
void test_clean_mgr_hifi_link(void)
{
    sreset_mgr_LLI  *phead = g_pmgr_hifireset_data;
    sreset_mgr_LLI  *ptail = g_pmgr_hifireset_data;

    while (NULL != phead)
    {
        ptail = phead->pnext;
        kfree(phead);
        phead = ptail;
    }
    g_pmgr_hifireset_data = NULL;
}

/*ends*/

/*****************************************************************************
 函 数 名  : reset_only_for_test
 功能描述  : 该函数只用于测试!!
 输入参数  : int iparam, 测试case:
             0,测试modem 狗复位
             1,测试modem 软件异常复位
             2,测试modem 正常复位
             3,测试hifi复位
             4,测试mcu狗复位
             5,测试底软在ccore复位时所做的处理，如ipf,icc,fileagent,cshell相应功能是否正确
 输出参数  : 无
 返 回 值  : int
        0, 成功，非0，失败
*****************************************************************************/
void reset_only_for_test(int iparam)
{
    //int     iresult = 0;
#ifdef _DRV_LLT_
#else
    switch (iparam)
    {
    case 0:
        up(&(g_reset_assistant.sem_wait_hifi_msg));
        up(&(g_reset_assistant.sem_wait_mcu_msg));
        up(&(g_reset_assistant.sem_wait_ccorereset_ok));
        modem_freeze_give_semaphone();
        break;
    case 1:
        up(&(g_reset_assistant.sem_wait_hifi_msg));
        up(&(g_reset_assistant.sem_wait_mcu_msg));
        up(&(g_reset_assistant.sem_wait_ccorereset_ok));
        modem_freeze_give_semaphone();
        break;
    case 2:
        up(&(g_reset_assistant.sem_wait_hifi_msg));
        up(&(g_reset_assistant.sem_wait_mcu_msg));
        up(&(g_reset_assistant.sem_wait_ccorereset_ok));
        simulate_irq_c_reset(1);
        break;
    case 3:
        up(&(g_reset_assistant.sem_wait_hifireset));
		up(&(g_reset_assistant.sem_wait_mcu_msg_hifireset));
        hifi_freeze_give_semaphone();
        break;
    case 4:
        simulate_irq_mcuwdt_reset();
        break;
    case 5:
		//y00145322
        /*send ip*/
        //iresult = BSP_IPF_ST_101(20);

        drv_ccorereset_cbfun(DRV_RESET_CALLCBFUN_RESET_BEFORE, 0);
        /*send ip*/
        //iresult = BSP_IPF_ST_101(20);
        break;
    case 6:
       reset_no_ok_savelog("drv", -2, 0, BSP_RESET_MODULE_HIFI);
       do_reset_system(RESET_TYPE_FOR_TEST);
        break;
    case 7:
        ccorereset_regcbfunc("drv", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL);
        ccorereset_regcbfunc("OAM", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL+1);
        ccorereset_regcbfunc("TTF", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL-1);
        ccorereset_regcbfunc("NAS", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL+1);

        break;
    case 8:
        hifireset_regcbfunc("OAM", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL+1);
        hifireset_regcbfunc("CODEC", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL+1);
        hifireset_regcbfunc("NAS", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL+1);


        break;
    case 9:
        test_clean_mgr_ccore_link();
        break;
    case 10:
        test_clean_mgr_hifi_link();
        break;
    case 11:
        ccorereset_runcbfun(DRV_RESET_CALLCBFUN_RESET_BEFORE);
        ccorereset_runcbfun(DRV_RESET_CALLCBFUN_RESET_AFTER);
        break;
    case 12:
        hifireset_runcbfun(DRV_RESET_CALLCBFUN_RESET_BEFORE);
        hifireset_runcbfun(DRV_RESET_CALLCBFUN_RESET_AFTER);

        break;
    case 13:
        up(&(g_reset_assistant.sem_wait_hifi_msg));

        break;
    case 14:
        up(&(g_reset_assistant.sem_wait_mcu_msg));
        break;
    case 15:
        up(&(g_reset_assistant.sem_wait_ccorereset_ok));
        break;
    case 16:
        up(&(g_reset_assistant.sem_wait_hifireset));
        break;
    case 17:
        reset_no_ok_savelog("drv", -2, 0, BSP_RESET_MODULE_HIFI);
        break;
    case 18:
        break;
    default:
        break;
    }
#endif
}

#endif


/*Added by l00212112,functions for c reset alone are defined, ends*/
#ifdef BSP_C_HIFI_RESET_ALONE_FEATURE
/*****************************************************************************
 函 数 名  : reset_sub_mgr_init
 功能描述  : 用于C核,hifi单独复位机制的初始化,在reset_module_init中调用,用于创建信号量,创建任务等操作.
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
int reset_sub_mgr_init (void)
{
    struct task_struct    *pCCoreTask = NULL;
    struct task_struct    *pHifiTask = NULL;

    struct task_struct    *pHifiSaveTask = NULL;

    int     iret = BSP_RESET_OK;

    /*默认HIFI/CCPU使能*/
    reset_set_cpu_status(0, RESET_CPU_HIFI_STATUS_OFF);
    reset_set_cpu_status(0, RESET_CPU_CCORE_STATUS_OFF);

    /*remap modem 系统控制寄存器*/
    reset_sub_mstrl = ioremap(SOC_MODEM_SCTRL_BASE_ADDR,0x1000);
    if (NULL == reset_sub_mstrl)
    {
        printk(KERN_ERR "%s: reset_sub_mstrl remap err\n", __FUNCTION__);
    }

    /*创建需要的信号量*/

    printk(KERN_INFO "%s: enter\n", __FUNCTION__);
    hisi_io_memset(&g_stResetDebugInfo, 0, sizeof(sreset_debug_info));
    hisi_io_memset(&g_reset_assistant, 0, sizeof(sreset_mgr_assistant));
    sema_init(&(g_reset_assistant.sem_wait_ccorereset), SEM_EMPTY);
    sema_init(&(g_reset_assistant.sem_wait_hifireset), SEM_EMPTY);
    sema_init(&(g_reset_assistant.sem_wait_ccorereset_ok), SEM_EMPTY);
    sema_init(&(g_reset_assistant.sem_wait_hifi_msg), SEM_EMPTY);
    sema_init(&(g_reset_assistant.sem_wait_mcu_msg), SEM_EMPTY);
    sema_init(&(g_reset_assistant.sem_wait_mcu_msg_hifireset), SEM_EMPTY);

    sema_init(&(g_reset_assistant.sem_wait_hifisave), SEM_EMPTY);

    /*创建C复位处理线程*/
    pCCoreTask = kthread_run(ccorereset_task,  NULL, "ccorereset_task");

    printk(KERN_INFO "%s: create ccorereset_task, return %p\n", __FUNCTION__, pCCoreTask);

    /*创建hifi复位处理线程*/
    pHifiTask = kthread_run(hifireset_task,  NULL, "hifireset_task");

    printk(KERN_INFO "%s: create hifireset_task, return %p\n", __FUNCTION__, pHifiTask);

    /*创建hifi noc时保存hifi内存的线程*/
    pHifiSaveTask = kthread_run(hifisave_task,  NULL, "hifisave_task");

    printk(KERN_INFO "%s: create hifisave_task, return %p\n", __FUNCTION__, pHifiSaveTask);

    /*注册C核复位回调函数*/
    ccorereset_regcbfunc("DRV", drv_ccorereset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL);

    /*注册HIFI复位回调函数*//*hifi复位，底软不注册*/
    hifireset_regcbfunc("CODEC", drv_hifireset_cbfun, 0, BSP_DRV_CBFUN_PRIOLEVEL);

    /*注册C核复位回调函数*/
    reset_icc_open();

    ccorereset_enable_wdt_irq();
    /*注册ACPU与MCU之间的MAILBOX*/
    iret = mailbox_reg_msg_cb(MAILBOX_MAILCODE_MCU_TO_ACPU_CCORE_RESET_ID, mailbox_recfun_mcu, NULL);
    iret = mailbox_reg_msg_cb(MAILBOX_MAILCODE_MCU_TO_ACPU_HIFI_RESET_ID, mailbox_recfun_mcu_hifireset, NULL);
    printk(KERN_INFO"RESET LOG: LEAVE reset_sub_mgr_init0! iret = %d\n", iret);


    /*注册ACPU与HIFI之间的MAILBOX*/
    iret = mailbox_reg_msg_cb(MAILBOX_MAILCODE_HIFI_TO_ACPU_CCORE_RESET_ID, mailbox_recfun_hifi, NULL);

    printk(KERN_INFO "%s: leave, iret = %d\n", __FUNCTION__, iret);

    return BSP_RESET_OK;
}

#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
