

/*******************************************************************************
  1 其他头文件包含
*******************************************************************************/
#include  "global_ddr_map.h"
#include  <asm/io.h>
#include  <linux/spinlock.h>
#include  <linux/sched.h>
#include  <drv_mntn.h>

#ifndef __EXCDRV_H__
#define __EXCDRV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*******************************************************************************
  2 宏定义
*******************************************************************************/

#define EXCH_TEST_FUNCTION_ON

#define EXC_READ_REG(reg, resu32t)  ((resu32t) = *(volatile unsigned int *)(reg))
#define EXC_WRITE_REG(addr,value) (*((volatile unsigned int  *)(addr)) = (value))

#define LINUX_VERSION   "Linux_"

#define EXCH_ERROR      (-1)
#define EXCH_OK         (0)

#define EXCH_READY      (1)
#define EXCH_NOT_READY  (0)
#define EXCH_REBOOT_FROM_EXC (1)
#define EXCH_FILE_OPEN_MODE         0666    //(S_IRUGO | S_IWUGO) (RW)
#define EXCH_FILE_NUM_MAX           2
#define EXCH_FILE_NAME_LEN_MAX      128
#define EXCH_DIR_ENTRY_SIZE         1024
#define EXCH_RST_LOG_BUF_SIZE       256
#define EXCH_COPY_BUF_SIZE          2048
#define EXCH_RESET_LOG_MAX          0x40000
#define EXCH_RTC_TIME_LEN           16
#define EXCH_MODID_PIDMASK	(1<<24)

/*记录任务信息个数*/
#define EXCH_RECORD_TASK_NUM_MAX    124
#define EXCH_CUST_TASK_NUM_MAX      8
#define EXCH_CUST_FUNC_NUM_MAX      64
#define EXCH_8K_SIZE                0x2000
/*#define EXCH_CB_NAME_SIZE           32  跟C核M核保存一致*/
#define EXCH_MEM_IS_READY           0x12345678
#define EXCH_TASK_NAME_LEN          16
#define EXCH_TASK_NAME_PART_LEN     12
#define EXCH_TASK_TCB_SIZE          (0x0400)
#define EXCH_QUEUE_NAME_LEN         12
#ifdef CONFIG_ARM64
#define EXCH_REG_NUM                36
#else
#define EXCH_REG_NUM                18
#endif

#define EXCH_A_CORE                 (1)
#define EXCH_C_CORE                 (2)
#define EXCH_M_CORE                 (3)

#define SYSTEM_NORMAL_RESET         0xffffffff
#define SC_SW_RST_VAL               0x48698284

#define EXCH_INT_EXIT               0xAAAA

#define EXCH_SYSTEM_ERROR           0x12345678

/*******************************************************************************
*                                                                              *
*                    以下保存异常时寄存器地址-Begin                            *
*                                                                              *
*******************************************************************************/

/*确定临终遗言模块DDR内存的起始地址与大小*/
#define EXCH_DDR_BASE_ADDR              ANDROID_DUMP_LOG_ADDR
#define EXCH_BASE_ADDR                  EXCH_A_CORE_BASE_ADDR
#define EXCH_ALL_SIZE                   (EXCH_A_CORE_SIZE + EXCH_C_CORE_SIZE + EXCH_M_CORE_SIZE)

#define EXCH_A_CORE_BASE_ADDR           g_exch_a_core_base_addr
#define EXCH_C_CORE_BASE_ADDR           g_exch_c_core_base_addr
#define EXCH_M_CORE_BASE_ADDR           g_exch_m_core_base_addr

#define EXCH_A_CORE_SIZE                ANDROID_DUMP_LOG_SIZE   //2M
#define EXCH_C_CORE_SIZE                MODEM_DUMP_LOG_SIZE     //2M
#define EXCH_M_CORE_SIZE                MCU_DUMP_LOG_SIZE       //1M

#define EXCH_EXC_BIN_SIZE               (EXCH_A_CORE_SIZE)





/********************************A核内存区规划*********************************/
/*A核起始地址*/
#define EXCH_A_CORE_BASE               (EXCH_A_CORE_BASE_ADDR)//

/*A核内存区起始段用于保存全局结构体g_pExchRoot*/
#define EXCH_ROOT_ADDR                 (EXCH_A_CORE_BASE) //0x0
#define EXCH_ROOT_SIZE                 (0x400)   //1K

/*记录每个CPU上最后一个任务TCB信息*/
#define EXCH_LAST_TASK_TCB_ADDR        (EXCH_ROOT_ADDR + EXCH_ROOT_SIZE)//0x400
#define EXCH_LAST_TASK_TCB_SIZE        (EXCH_TASK_TCB_SIZE * NR_CPUS)     //8K(0x400*8)

/*记录用户订制参数信息*/
#define EXCH_CUSTOM_ARG_ADDR           (EXCH_LAST_TASK_TCB_ADDR + EXCH_LAST_TASK_TCB_SIZE)//0x2400
#define EXCH_CUSTOM_ARG_SIZE           (0x0400)

/*记录每个CPU上中断切换信息*/
#define EXCH_INT_SWITCH_ADDR           (EXCH_CUSTOM_ARG_ADDR + EXCH_CUSTOM_ARG_SIZE)    //0x2800
#define EXCH_INT_SWITCH_SIZE           (0x10000)    //64K

/*记录每个CPU上任务切换信息*/
#define EXCH_TASK_SWITCH_ADDR          (EXCH_INT_SWITCH_ADDR + EXCH_INT_SWITCH_SIZE)    //0x12800
#define EXCH_TASK_SWITCH_SIZE          (0x40000)    //256K

/*记录每个CPU上最后一个异常中断栈信息*/
#define EXCH_LAST_INT_STACK_ADDR       (EXCH_TASK_SWITCH_ADDR + EXCH_TASK_SWITCH_SIZE)  //0x52800
#define EXCH_LAST_INT_STACK_SIZE       (0x0400)     //1K

/*记录每个CPU上最后一个异常任务栈信息*/
#define EXCH_LAST_TASK_STACK_ADDR      (EXCH_LAST_INT_STACK_ADDR + EXCH_LAST_INT_STACK_SIZE)    //0x52C00
#define EXCH_LAST_TASK_STACK_SIZE      (EXCH_8K_SIZE * NR_CPUS)     //64K(0x2000*8)

/*记录A核124个任务的任务名*/
#define EXCH_ALL_TASK_NAME_ADDR        (EXCH_LAST_TASK_STACK_ADDR + EXCH_LAST_TASK_STACK_SIZE)  //0x62C00
#define EXCH_ALL_TASK_NAME_SIZE        (0x1000)   //4K

/*记录A核124个任务的TCB*/
#define EXCH_ALL_TASK_TCB_ADDR         (EXCH_ALL_TASK_NAME_ADDR + EXCH_ALL_TASK_NAME_SIZE)      //0x63C00
#define EXCH_ALL_TASK_TCB_SIZE         (0x1F000)  //124K

/*记录OM模块异常信息*/
#define EXCH_OSA_ADDR                   (EXCH_ALL_TASK_TCB_ADDR + EXCH_ALL_TASK_TCB_SIZE)       //0x82C00
#define EXCH_OSA_SIZE                   (0x8000)  //32K

/*用于记录用户指定任务队列信息*/
#define EXCH_CUSTOM_TASK_QUEUE_ADDR     (EXCH_OSA_ADDR + EXCH_OSA_SIZE) //0x8AC00
#define EXCH_CUSTOM_TASK_QUEUE_SIZE     (0x400)
/*用于记录用户指定任务的任务信息*/
#define EXCH_CUSTOM_TASK_INFO_ADDR      (EXCH_CUSTOM_TASK_QUEUE_ADDR + EXCH_CUSTOM_TASK_QUEUE_SIZE) //0x8B000
#define EXCH_CUSTOM_TASK_INFO_SIZE      (0x10000) //64K
/*用于记录用户注册的回调接口信息*/
#define EXCH_CUSTOM_FUNC_QUEUE_ADDR     (EXCH_CUSTOM_TASK_INFO_ADDR + EXCH_CUSTOM_TASK_INFO_SIZE)   //0x9B000
#define EXCH_CUSTOM_FUNC_QUEUE_SIZE     (0x400)
/*用于记录应用注册的回调传入的buffer*/
#define EXCH_CUSTOM_FUNC_INFO_ADDR      (EXCH_CUSTOM_FUNC_QUEUE_ADDR + EXCH_CUSTOM_FUNC_QUEUE_SIZE) //0x9B400
#define EXCH_CUSTOM_FUNC_INFO_SIZE      (0x10000) //64K

/*记录系统GIC状态寄存器*/
#define EXCH_A_GIC_INT_STS_ADDR         (EXCH_CUSTOM_FUNC_INFO_ADDR + EXCH_CUSTOM_FUNC_INFO_SIZE)//0xAB400
#define EXCH_A_GIC_INT_STS_SIZE         (0x80)    //128B

/*记录系统异常打印信息*/
#define EXCH_LOG_KMSG_ADDR              (EXCH_A_GIC_INT_STS_ADDR + EXCH_A_GIC_INT_STS_SIZE)//0xAB480
#define EXCH_LOG_KMSG_SIZE              (0x40000) //256K

/* 低功耗不再使用该记录区，ram-console因页对齐占用一部分该空间 */
#define PWRCTRL_EXC_ACPU_ADDR           (EXCH_LOG_KMSG_ADDR + EXCH_LOG_KMSG_SIZE)     //0xEB480
#define PWRCTRL_EXC_ACPU_SIZE           (0x2000)

#define SOC_AO_SCTRL_ADDR               (PWRCTRL_EXC_ACPU_ADDR + PWRCTRL_EXC_ACPU_SIZE)   //0xED480
#define SOC_AO_SCTRL_SIZE               (0x2000)  //8K

#define EXCH_DRV_CUSTOM_INFO_ADDR       (SOC_AO_SCTRL_ADDR + SOC_AO_SCTRL_SIZE)   //0xEF480
#define EXCH_DRV_CUSTOM_INFO_SIZE       (0x2000)  //8K

/****************************A/C/M核内存共享区规划*****************************/

/* A/C/M核内存共享区，位于SRAM */
/* 共享内存区，用于保存各核共享信息，主控核写，各核均可读 */
#define EXCH_CORES_SHARE_ADDR             (EXCH_DRV_CUSTOM_INFO_ADDR + EXCH_DRV_CUSTOM_INFO_SIZE)   //0xF1480
#define EXCH_CORES_SHARE_SIZE             (0x1000)                //4K

/*sc off 4k,pmctrl 4k,ddrc 36k,sysnoc 128byte*/
#define SOC_PERI_SCTRL_ADDR                  (EXCH_CORES_SHARE_ADDR + EXCH_CORES_SHARE_SIZE)/*0xF2480*/
#define SOC_PERI_SCTRL_SIZE                  (0x1000)

#define SOC_PMCTRL_ADDR                 (SOC_PERI_SCTRL_ADDR + SOC_PERI_SCTRL_SIZE)/*0xF3480*/
#define SOC_PMCTRL_SIZE                 (0x1000)

#define SOC_SYS_NOC_ADDR                (SOC_PMCTRL_ADDR + SOC_PMCTRL_SIZE)/*0xF4480*/
#define SOC_SYS_NOC_SIZE                (0x80)

#define SOC_DDRC_ADDR                   (SOC_SYS_NOC_ADDR + SOC_SYS_NOC_SIZE)/*0xF4500*/
#define SOC_DDRC_SIZE                   (0x9000)

#define SOC_ACPU_SC_ADDR            (SOC_DDRC_ADDR + SOC_DDRC_SIZE)/*0xFD500*/
#define SOC_ACPU_SC_SIZE             (0x800)    //2k

#define SOC_MODEM_SC_ADDR         (SOC_ACPU_SC_ADDR + SOC_MODEM_SC_SIZE)/*0xFDD00*/
#define SOC_MODEM_SC_SIZE           (0x1000)   //4k

#define SOC_MEDIA_SC_ADDR           (SOC_MODEM_SC_ADDR + SOC_MODEM_SC_SIZE)/*0xFED00*/
#define SOC_MEDIA_SC_SIZE             (0x800)   //2k

#define CPU_NUMS setup_max_cpus

#define HIMNTN_VALID_SIZE   (32)

#define RESET_LOG_PATH         	("/data/hisi_logs/cp_log/coredump")
#define RESET_LOG_FILE               ("reset.log")
#define RESET_LOG_FILE_PATH_SIZE     (64)
#define RESET_LOG_MAX_SIZE (1024)

#define HIFI_RESET_LOG_PATH         	("/data/hisi_logs/hifi_log/coredump")
#define HIFI_RESET_LOG_FILE             ("hifi_reset.log")
#define HIFI_RESET_LOG_MAX_SIZE         (1024*1024) //保存hifi内存的最大容量
#define EXCH_H_CORE_BASE_ADDR           0x05a3285c  //hifi的可谓可测的起始地址

#define HIFI_RESET_KEY_NUM_ONE          0x58585858  //hifi存储地址的结构体的关键字
#define HIFI_RESET_KEY_NUM_TWO          0x55885588
#define HIFI_RESET_KEY_NUM_THREE        0x85858585
#define HIFI_RESET_KEY_NUM_FOUR         0x88558855

/*------------------Portland----------------------*/
#define EXCH_STATE_TASK             (0x10)
#define EXCH_STATE_INT              (0x80)
#define EXCH_RTC_TIME_LEN           16


struct dump_log
{
    char root_info[EXCH_ROOT_SIZE];
    char last_task[EXCH_LAST_TASK_TCB_SIZE];
    char custom_arg[EXCH_CUSTOM_ARG_SIZE];
    char irq_trace[EXCH_INT_SWITCH_SIZE];
    char task_trace[EXCH_TASK_SWITCH_SIZE];
    char last_irq_stack[EXCH_LAST_INT_STACK_SIZE];
    char last_task_stack[EXCH_LAST_TASK_STACK_SIZE];
    char all_tasks_name[EXCH_ALL_TASK_NAME_SIZE];
    char all_tasks_tcb[EXCH_ALL_TASK_TCB_SIZE];
    char osa_info[EXCH_OSA_SIZE];
    char custom_task_queue[EXCH_CUSTOM_TASK_QUEUE_SIZE];
    char custom_task_info[EXCH_CUSTOM_TASK_INFO_SIZE];
    char custom_func_queue[EXCH_CUSTOM_FUNC_QUEUE_SIZE];
    char custom_func_info[EXCH_CUSTOM_FUNC_INFO_SIZE];
    char gic_irq_status[EXCH_A_GIC_INT_STS_SIZE];
    char log_kmsg[EXCH_LOG_KMSG_SIZE];
    char pm_info[PWRCTRL_EXC_ACPU_SIZE];
    char soc_ao_sctrl[SOC_AO_SCTRL_SIZE];
    char drv_custom_info[EXCH_DRV_CUSTOM_INFO_SIZE];
    char cores_share[EXCH_CORES_SHARE_SIZE];
    char soc_peri_sctrl[SOC_PERI_SCTRL_SIZE];
    char soc_pmctrl[SOC_PMCTRL_SIZE];
    char soc_sys_noc[SOC_SYS_NOC_SIZE];
    char soc_ddrc[SOC_DDRC_SIZE];
    char soc_acpu_sc[SOC_ACPU_SC_SIZE];
    char soc_modem_sc[SOC_MODEM_SC_SIZE];
    char soc_media_sc[SOC_MEDIA_SC_SIZE];
};

struct irq_info
{
    int irq;
	int cpu;
    unsigned long slice;
    unsigned long jiff;
};

struct task_info
{
    char taskname[TASK_COMM_LEN];
    int pid;
	int cpu;
    unsigned long slice;
    unsigned long jiff;
};


/*z00179562,ccore unite,远程文件系统可维可测,starts*/
#define EXCH_RFS_ADDR (MEMORY_RAM_CORESHARE_RFS_MNTN_ADDR)
#define EXCH_RFS_SIZE (MEMORY_RAM_CORESHARE_RFS_MNTN_SIZE)
/*z00179562,ccore unite,远程文件系统可维可测,ends*/

/*******************************************************************************
  3 枚举定义
*******************************************************************************/
typedef enum
{
    EXCH_S_NORMAL,
    EXCH_S_ARM,
    EXCH_S_PANIC,
    EXCH_S_DIE,
    EXCH_S_USR_REBOOT,
    EXCH_S_NOC,
    EXCH_S_WDT,
    EXCH_S_PMU = 0xAA,   /*systemErrorCcore 保持跟C核一致*/
    EXCH_S_DORESET,     /*Acore reset,C核/hifi单独复位失败*/
    EXCH_S_LOGDUMP,     /*按组合键主动复位系统进入log dump*/
    EXCH_S_DDRC_SEC,     /*DDRC SECURITY reboot*/
    EXCH_S_WATCHPOINT,      /*for watchpoint 功能*/
    EXCH_S_OTHERS
} EXCH_SOURCE;

#ifdef EXCH_TEST_FUNCTION_ON
enum exchErrType
{
    EXC_UNDEFINE_INSTRUCTION,
    EXC_SOFTWARE_INTERRUPT,
    EXC_PREFETCH_ABORT,
    EXC_DATA_ABORT,
    EXC_DEVISION,
    EXC_PANIC,
    EXC_LOOP,
    EXC_ERR_RESERVED
};
#endif

enum himntnEnum
{
    HIMNTN_DUMP_MEM,
    HIMNTN_IRQ_AND_TASK_SWITCH_RECORD,
    HIMNTN_PRESS_KEY_TO_FASTBOOT,
    HIMNTN_MCU_WDT,
    HIMNTN_ACPU_WDT,
    HIMNTN_CLOSE_ACPU_PANIC_LOOP,
    HIMNTN_GOBAL_RESETLOG,
    HIMNTN_MODEM_RESETLOG,
    HIMNTN_HIFI_RESETLOG,
    HIMNTN_NOC_ErrLog,/* 该字节如果是字符0，则表示noc的超时上报errLog功能被关闭；其他表示打开 */
    HIMNTN_NOC_ERR_LOGGER_RESET,    /*used in kernel, whether reset system when noc error logger irq happend*/
    HIMNTN_FIQ_DEBUGGER,
    HIMNTN_ACCESS_ERROR_REBOOT,/*DDR 安全中断复位使能位*/
    HIMNTN_LOAD_DDR_RESETLOG = 23,/*带宽 负载触发系统异常 记录LOG 使 能，默认 关闭 */
    HIMNTN_MBFULL_ASSERT,
    HIMNTN_PRINTK_WHO_WAKEUP_ACPU,
    HIMNTN_LOAD_DDR_EN, /*带宽 统计 使 能，默认 打开*/
    HIMNTN_LOAD_DDR_SINGLE_PORT,   /*单带宽端口 统计使能*/
    HIMNTN_POWERKEY_RESET,
    HIMNTN_UNLOCK_CORE_FOR_MCUSAVEPC,/*MCU saves pc of acpu*/

    /*Add above, and keep the same as definition in nv_number.h in fastboot !!!!*/
	HIMNTN_BOTTOM
};
extern unsigned int himntn_gobal_resetlog;
extern unsigned int himntn_modem_resetlog;
extern unsigned int himntn_hifi_resetlog;
#define HIMNTN_ALWAYS_RESETLOG     1
/*******************************************************************************
  4 消息头定义
*******************************************************************************/


/*******************************************************************************
  5 消息定义
*******************************************************************************/


/*******************************************************************************
  6 STRUCT定义
*******************************************************************************/


typedef struct
{
    unsigned int    reboot_reason;  //复位原因，A核写，各核可读
    unsigned int    reboot_core;    //复位核，A核写，各核可读
    unsigned int    mem_ready;      //内存准备好，A核写，各核读，开始初始化
    unsigned int    is_rebooting;   //系统重启中

}cores_share_info_t;

typedef struct
{
    unsigned int*    reboot_reason;  /*复位原因，A核写，各核可读*/
    unsigned int*    reboot_core;    /*复位核，A核写，各核可读*/
    unsigned int*    mem_ready;      /*内存准备好，A核写，各核读，开始初始化*/
    unsigned int*    is_rebooting;   /*系统重启中*/
}cores_share_addr_info_t;
typedef struct
{
	/*Don't modify any content bellow!!!, for using analyzing tool!!!*/
    unsigned int    dumpMagic;       /* 0x00  */
    unsigned char   version[32];       /* 0x04  */
    unsigned int    rebootTask;        /* 0x24  */  /*rebootTaskPid*/
    unsigned int    rebootTicks;       /* 0x28  */
    unsigned int    modId;             /* 0x2c  */
    int             arg1;              /* 0x30  */
    int             arg2;              /* 0x34  */
    unsigned int    omTimer;           /* 0x38  */
    unsigned int    vec;               /* 0x3c  */
#ifdef CONFIG_ARM64
	u64 			regSet[36];
	u64				cpsr;
#else
    unsigned int    regSet[18];        /* 0x40  */
    unsigned int    cpsr;              /* 0x88  */
#endif
    unsigned int    reboot_src;        /* 0x8c  */
    unsigned int    excSize;           /* 0x90  */
    spinlock_t *    spinlock;          /* 0x94 */
    cores_share_addr_info_t  share_addr;                 /* 0x98  *//* 0xE4  */
    cores_share_info_t  share_value;                /* 0xa8  */
    unsigned char       rtcTime[EXCH_RTC_TIME_LEN]; /* 0xb8 */


    unsigned char*    taskTcbAddr;       /* 0xc8  */   /*lastTaskTcbAddr*/
    unsigned char*    customAddr;        /* 0xcc  */   /*customArgAddr*/

    unsigned char*    taskSwitchAddr;    /* 0xd0  */
    unsigned char*    intStackAddr;      /* 0xd4  */   /*lastIntStackAddr*/
    unsigned char*    taskStackAddr;     /* 0xd8  */   /*lastTaskStackAddr*/

    unsigned char*    customFuncQueueAddr; /* 0xdc */
    unsigned char*    customTaskQueueAddr; /* 0xe0 */
    unsigned char*    customTaskInfoAddr;  /* 0xe4 */
    unsigned char*    customFuncInfoAddr;  /* 0xe8 */

    unsigned char*    allTaskNameAddr;     /* 0xec */
    unsigned char*    intSwitchAddr;       /* 0xf0 */
    unsigned char*    excBaseAddr;         /* 0xf4 */

    void*    taskId[NR_CPUS];      /* 0xf8 */  /*lastTaskId*/
    unsigned int    taskTicks[NR_CPUS];   /* 0x118 */  /*lastTaskTicks*/
    unsigned int    intId[NR_CPUS];       /* 0x138 */  /*lastIntId*/
    unsigned int    intTicks[NR_CPUS];    /* 0x158 */  /*lastIntTicks*/
	/*Don't modify any content above!!!, for using analyzing tool!!!*/

	/*Add new content here if need.*/
    unsigned int        struEndOffset[1] ;      /* 0x178 */
}
 EH_ROOT;


typedef struct
{
    int  pid;
    char taskname[EXCH_TASK_NAME_PART_LEN];
} TASK_NAME_STRU;


typedef enum
{
    EXCH_CCORE_S_NORMAL,
    EXCH_CCORE_S_ARM,
    EXCH_CCORE_S_OS,
    EXCH_CCORE_S_WDT,
    EXCH_CCORE_S_SF,
    EXCH_CCORE_S_NOC,
    EXCH_CCORE_S_SD,
    EXCH_CCORE_S_SCI,
    EXCH_CCORE_S_USB,
    EXCH_CCORE_S_ONLINE,
    EXCH_CCORE_S_SSI_READ,
    EXCH_CCORE_S_SSI_WRITE,
    EXCH_CCORE_S_MODEM_REBOOT_BYRIL,
    EXCH_CCORE_S_MODEM_PANIC_BYRIL,
    EXCH_CCORE_S_IRQ_REQUIRE_ERROR,
    EXCH_CCORE_S_SC_ERR = 0x99,
    EXCH_CCORE_S_PMU = 0xAA,
    EXCH_CCORE_S_PMU_ERR = 0xDD,
} EXCH_SOURCE_CCORE;

typedef struct
{
    unsigned char   product;           /* 0x00  */
    unsigned char   rsv1;
    unsigned char   rsv2;
    unsigned char   rsv3;

    unsigned char   version[32];       /* 0x04  */

    unsigned int    whyReboot;         /* 0x24  */
    unsigned char   excSave;           /* 0x28  */
    unsigned char   omLogSave;         /* 0x29  */
    unsigned char   omMsgSave;         /* 0x2A  */
    unsigned char   cpuViewSave;       /* 0x2B  */
    unsigned char   wdtSave;           /* 0x2C  */
    unsigned char   sdExcSave;         /* 0x2D  */
    unsigned char   sciExcSave;        /* 0x2E  */
    unsigned char   usbExcSave;        /* 0x2F  */

    unsigned int    rebootTask;        /* 0x30  */  /*rebootTaskPid*/
    unsigned int    rebootTicks;       /* 0x34  */
    unsigned int    modId;             /* 0x38  */
    int             arg1;              /* 0x3C  */

    int             arg2;              /* 0x40  */
    unsigned int    omTimer;           /* 0x44  */
    unsigned int    vec;               /* 0x48  */
    unsigned int    cpsr;              /* 0x4C  */

    unsigned int    regSet[18];        /* 0x50  */

    unsigned char   taskName[16];      /* 0x98  */  /*lastTaskName*/

    unsigned int    taskId;            /* 0xA8 */  /*lastTaskId*/
    unsigned int    taskTicks;         /* 0xAC */  /*lastTaskTicks*/

    unsigned int    intId;             /* 0xB0  */ /*lastIntId*/
    unsigned int    intTicks;          /* 0xB4  */ /*lastIntTicks*/
    unsigned int    subIntId;          /* 0xB8  */
    unsigned int    subIntTicks;       /* 0xBC  */

    unsigned int    rebootstate;       /* 0xC0 */
    unsigned int    reboot_src;        /* 0xC4  */
    unsigned int    excSize;           /* 0xC8  */
    unsigned int    omLogSize;         /* 0xCC  */

    unsigned int    omMsgSize;         /* 0xD0 */
    unsigned int    freeHeapSize;      /* 0xD4 */
    unsigned int    maxFreeBlockSize;  /* 0xD8 */
    unsigned int    spinlock;          /* 0xDC */

    cores_share_info_t  share_addr;                 /* 0xE0  */
    cores_share_info_t  share_value;                /* 0xF0  */
    unsigned char       rtcTime[EXCH_RTC_TIME_LEN]; /* 0x100 */

    /*填充空白，使得 Addr 段能够从0x140这个固定地址开始定义*/
    unsigned int    fillReserved0[0xC];    /*0x110*/

    /*address段，固定在0x140偏移*/
    unsigned int    osMsgAddr;         /* 0x140  */
    unsigned int    taskTcbAddr;       /* 0x144  */   /*lastTaskTcbAddr*/
    unsigned int    customAddr;        /* 0x148  */   /*customArgAddr*/
    unsigned int    intAddr;           /* 0x14c  */

    unsigned int    subintAddr;        /* 0x150  */
    unsigned int    taskSwitchAddr;    /* 0x154  */
    unsigned int    intStackAddr;      /* 0x158  */   /*lastIntStackAddr*/
    unsigned int    taskStackAddr;     /* 0x15c  */   /*lastTaskStackAddr*/

    unsigned int    taskExcStackAddr;  /* 0x160 */
    unsigned int    cpuViewAddr;       /* 0x164 */
    unsigned int    taskAllTcbAddr;    /* 0x168 */
    unsigned int    omLogAddr;         /* 0x16c */

    unsigned int    omMsgAddr;         /* 0x170 */
    unsigned int    sdExcAddr;         /* 0x174 */
    unsigned int    sciExcAddr;        /* 0x178 */
    unsigned int    usbExcAddr;        /* 0x17c */

    unsigned int    customFuncQueueAddr; /* 0x180 */
    unsigned int    customTaskQueueAddr; /* 0x184 */
    unsigned int    customTaskInfoAddr;  /* 0x188 */
    unsigned int    customFuncInfoAddr;  /* 0x18C */

    unsigned int    allTaskNameAddr;     /* 0x190 */
    unsigned int    intSwitchAddr;       /* 0x194 */
    unsigned int    excBaseAddr;         /* 0x198 */
    unsigned int    taskAllStaskAddr;    /* 0x19c */
    unsigned int    memInfoAddr;         /* 0x1a0 */
    unsigned int    sysState;            /* 0x1a4 */
    unsigned int    cacheState;          /* 0x1a8 */
    unsigned int    freqInfo[0xf];       /* 0x1ac */
    unsigned int    armCP15[6];          /* 0x1e8 */

    /*尾部填充，   使struEndOffset为0x200*/
    /*unsigned int    fillReserved1[6];   *//* 0x1f8 */

    /*结束地址，固定为0x200*/
    unsigned int    struEndOffset[1] ;      /* 0x200 */
}EH_ROOT_CCORE;
#if 0
typedef struct
{
    char   aucName[EXCH_CB_NAME_SIZE];
    unsigned char *pucData;
    unsigned int  ulDataLen;
}cb_buf_t;
#endif

typedef unsigned int  VOS_UINT32;

/*****************************************************************************
 实体名称  : OM_MAIN_OBJ_ID_ENUM
 功能描述  : HIFI内部重要模块全局变量的地址
*****************************************************************************/
enum
{
    OM_MAIN_OBJ_ID_AUDIO_PCM             = 0,
    OM_MAIN_OBJ_ID_CPU_VIEW,
    OM_MAIN_OBJ_ID_INNER_LOG,
    OM_MAIN_OBJ_ID_MAILBOX,
    OM_MAIN_OBJ_ID_V_QUEUE,
    OM_MAIN_OBJ_ID_AUDIO_PLAYER,
    OM_MAIN_OBJ_ID_MEM_CTRL,
    OM_MAIN_OBJ_ID_LAST_OM_LOG,
    OM_MAIN_OBJ_ID_VOICE_MC,
    OM_MAIN_OBJ_ID_MEM_DYN,
    OM_MAIN_OBJ_ID_AUDIO_ENHANCE,
    OM_MAIN_OBJ_ID_CPU_LOAD,
    OM_MAIN_OBJ_ID_SJB,
    OM_MAIN_OBJ_ID_DUMP_TRACES,
    OM_MAIN_OBJ_ID_DUMP_TCM,
    OM_MAIN_OBJ_ID_BUTT
};
typedef VOS_UINT32 OM_MAIN_OBJ_ID_ENUM_UINT32;

/* 记录主要模块地址的结构体 */
typedef struct
{
    VOS_UINT32              uwFlagWord1;
    VOS_UINT32              uwFlagWord2;
    VOS_UINT32              uwObjAddr[OM_MAIN_OBJ_ID_BUTT];
    VOS_UINT32              uwObjSize[OM_MAIN_OBJ_ID_BUTT];
    VOS_UINT32              uwFlagWord3;
    VOS_UINT32              uwFlagWord4;
} OM_MAIN_OBJ_INFO_STRU;

/* 将hifi的物理地址分段映射到虚拟地址空间时，记录地址的结构体 */
typedef struct
{
    VOS_UINT32              uwObjAddr[OM_MAIN_OBJ_ID_BUTT];
} HIFI_MAIN_OBJ_INFO_STRU;

/*******************************************************************************
  7 UNION定义
*******************************************************************************/


/*******************************************************************************
  8 OTHERS定义
*******************************************************************************/


/*******************************************************************************
  9 全局变量声明
*******************************************************************************/
typedef void (*FUNC_VOID)(void);
/*typedef int  (*exchCBReg)(cb_buf_t *);, remove, redefined!*/
extern unsigned int g_exch_base_addr;
extern unsigned char* g_exch_a_core_base_addr;
extern unsigned char* g_exch_c_core_base_addr;
extern unsigned char* g_exch_m_core_base_addr;
extern char *g_irq_trace_name[NR_CPUS];
extern char *g_task_trace_name[NR_CPUS];
extern unsigned int g_irq_record_len[NR_CPUS];
extern unsigned int g_irq_record_offset[NR_CPUS];
extern unsigned int g_task_record_len[NR_CPUS];
extern unsigned int g_task_record_offset[NR_CPUS];

/*******************************************************************************
  10 函数声明
*******************************************************************************/
extern void systemError(int modId, int arg1, int arg2, char *arg3, int arg3Length);
extern int  exch_file_save(char *file_name, void *address, unsigned int length);
extern int  DRV_EXCH_TASK_INFO_ADD(int taskPid);
extern int  DRV_EXCH_CUST_FUNC_REG(exchCBReg cb);
extern int  check_himntn(int feature);
extern void* exch_get_drv_addr(void);
extern void reg_save(unsigned int* ulRamAddr, unsigned int* ulReg, unsigned int ulSize);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of excDrv.h */

