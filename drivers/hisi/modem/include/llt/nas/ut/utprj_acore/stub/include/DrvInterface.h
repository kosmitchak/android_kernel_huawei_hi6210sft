/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  Drvinterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"Drvinterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/
/*#include <vxWorks.h>
#include "BSP_GLOBAL.h"
#include "BSP_VERSION.h"*/


#ifndef __DRV_INTERFACE_H__
#define __DRV_INTERFACE_H__

#include "TTFMemInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*************************GLOBAL BEGIN*****************************/
/* 基本数据类型定义 */
#ifndef _WIN32_COMPILE
typedef signed long long    BSP_S64;
#else
typedef double              BSP_S64;
#endif
typedef signed int          BSP_S32;
typedef signed short        BSP_S16;
typedef signed char         BSP_S8;
typedef char                BSP_CHAR;

#ifndef _WIN32_COMPILE
typedef unsigned long long  BSP_U64;
#else
typedef double              BSP_U64;
#endif
typedef unsigned int        BSP_U32;
typedef unsigned short      BSP_U16;
typedef unsigned char       BSP_U8;

typedef int                 BSP_BOOL;
typedef void                BSP_VOID;
typedef int                 BSP_STATUS;

#ifndef BSP_VOID
#define BSP_VOID void
#endif

#ifndef _WIN32_COMPILE
typedef signed long long*   BSP_PS64;
#else
typedef double*             BSP_PS64;
#endif
typedef signed int*         BSP_PS32;
typedef signed short*       BSP_PS16;
typedef signed char*        BSP_PS8;

#ifndef _WIN32_COMPILE
typedef unsigned long long* BSP_PU64;
#else
typedef double*             BSP_PU64;
#endif
typedef unsigned int*       BSP_PU32;
typedef unsigned short*     BSP_PU16;
typedef unsigned char*      BSP_PU8;

typedef int*                BSP_PBOOL;
typedef void*               BSP_PVOID;
typedef int*                BSP_PSTATUS;

#ifndef BSP_CONST
#define BSP_CONST               const
#endif

#ifndef BSP_FALSE
#define BSP_FALSE                0
#endif

#ifndef BSP_TRUE
#define BSP_TRUE                 1
#endif

#ifndef BSP_NULL
#define BSP_NULL                 (void*)0
#endif

#ifndef BSP_OK
 #define BSP_OK (0)
#endif

#ifndef BSP_ERROR
 #define BSP_ERROR (-1)
#endif


#ifndef INLINE
#define INLINE __inline__
#endif
#define TTFA9_DRAM_SHARE_PHYS           (0x37000000)
#define TTFA9_DRAM_SHARE_VIRT           (0xF3400000)

#ifndef __UT_CENTER__
#ifdef DMT
#define TTF_VIRT_TO_PHY(var)  (var)
#define TTF_PHY_TO_VIRT(var)  (var)
#else
#define TTF_VIRT_TO_PHY(var)  (((unsigned int)(var) - TTFA9_DRAM_SHARE_VIRT) + TTFA9_DRAM_SHARE_PHYS)
#define TTF_PHY_TO_VIRT(var)  (((unsigned int)(var) - TTFA9_DRAM_SHARE_PHYS) + TTFA9_DRAM_SHARE_VIRT)
#endif
#else
#define TTF_VIRT_TO_PHY(var)  (var)
#define TTF_PHY_TO_VIRT(var)  (var)
#endif

#define PWRCTRL_GET_SUCCESS         (0x00)
#define PWRCTRL_GET_PARAINVALID     (0x01)
#define PWRCTRL_ONORLOCK            (0x10)
#define PWRCTRL_OFFORUNLOCK         (0x20)


/* 模块定义 */
typedef enum tagBSP_MODULE_E
{
    BSP_MODU_NANDF = 0x0, /* 0x0 */
    BSP_MODU_TIMER, /* 0x1 */
    BSP_MODU_VIC,   /* 0x2 */
    BSP_MODU_UART,  /* 0x3 */
    BSP_MODU_YAFFS, /* 0x4 */
    BSP_MODU_ETH,   /* 0x5 */
    BSP_MODU_DMAC,  /* 0x6 */
    BSP_MODU_SPI,   /* 0x7 */
    BSP_MODU_GPIO,  /* 0x8 */
    BSP_MODU_WDT,   /* 0x9 */
    BSP_MODU_SCI,   /* 0xA */
    BSP_MODU_USB,   /* 0xB */
    BSP_MODU_DRX,   /* 0xC */
    BSP_MODU_SDIO,  /* 0xD */
    BSP_MODU_FTP,   /* 0xE */
    BSP_MODU_GMAC,	  /* 0xF */
    BSP_MODU_SEEKER,	/* 0x10 */
    BSP_MODU_UPGRADE,	/* 0x11 */
    BSP_MODU_AXI,	/* 0x12 */
    BSP_MODU_NET,	/* 0x13 */
    BSP_MODU_USB_ACM,	/* 0x14 */
    BSP_MODU_USB_NCM,	/* 0x15 */
    BSP_MODU_AXI_MONITOR,
    BSP_MODU_IFC,
    BSP_MODU_ICC,
    BSP_MODU_IPF,
    BSP_MODU_ACC,	/* 0x1A */
    BSP_MODU_BBP,
    BSP_MODU_MEM,
    BSP_MODU_UDI,
    BSP_MODU_DLOAD,
    BSP_MODU_SOCP,
    BSP_MODU_IFCP,  /* 0x20 */
    BSP_MODU_PWRCTRL,
    BSP_MODU_SERIAL_UPGRADE,
    BSP_MODU_SYNC,
    BSP_MODU_IPC,
    BSP_MODU_MNTN,
    BSP_MODU_ALL,   /* 代表所有的模块 */
    BSP_MODU_MAX    /* 边界值 */
} BSP_MODULE_E;

/*
 * 错误码宏定义
 */
#define BSP_ERR_MODULE_OFFSET (0x1000)    /* 防止和系统的错误码重叠 */
#define BSP_DEF_ERR( mod, errid) \
    (((mod + BSP_ERR_MODULE_OFFSET) << 16) | (errid))

/* 通用错误码，从0x1001开始 */
#define BSP_ERR_COMMON (0x1000)
#define BSP_ERR_MODULE_NOT_INITED (BSP_ERR_COMMON + 1)
#define BSP_ERR_NULL_PTR (BSP_ERR_COMMON + 2)
#define BSP_ERR_INVALID_PARA (BSP_ERR_COMMON + 3)
#define BSP_ERR_RETRY_TIMEOUT (BSP_ERR_COMMON + 4)
#define BSP_ERR_BUF_ALLOC_FAILED (BSP_ERR_COMMON + 5)
#define BSP_ERR_BUF_FREE_FAILED (BSP_ERR_COMMON + 6)

/* 各模块专用错误码，从0x1开始  */
#define BSP_ERR_SPECIAL (0x0)

typedef enum{
     BSP_MODEM_CORE         = 0,
     BSP_APP_CORE
}BSP_CORE_TYPE_E;

typedef enum{
     PV500_CHIP             = 0,
     V7R1_CHIP
}BSP_CHIP_TYPE_E;

/*
 * 调试宏定义
 */

/* 打印级别定义 */
typedef enum tagBSP_LOG_LEVEL_E
{
    BSP_LOG_LEVEL_DEBUG = 0,  /* 0x0:debug-level                                  */
    BSP_LOG_LEVEL_INFO,      /* 0x1:informational                                */
    BSP_LOG_LEVEL_NOTICE,     /* 0x2:normal but significant condition             */
    BSP_LOG_LEVEL_WARNING,    /* 0x3:warning conditions                           */
    BSP_LOG_LEVEL_ERROR,      /* 0x4:error conditions                             */
    BSP_LOG_LEVEL_CRIT,       /* 0x5:critical conditions                          */
    BSP_LOG_LEVEL_ALERT,      /* 0x6:action must be taken immediately             */
    BSP_LOG_LEVEL_FATAL,      /* 0x7:just for compatibility with previous version */
    BSP_LOG_LEVEL_MAX         /* 边界值 */
} BSP_LOG_LEVEL_E;

#undef __BSP_DEBUG__


#ifdef __BSP_DEBUG__
#include <logLib.h>

#define BSP_ASSERT(expr) \
{\
    if (!(expr)) { \
        logMsg("ASSERT failed at:\n  >File name: %s\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n", \
               (BSP_S32)__FILE__, (BSP_S32)__FUNCTION__, (BSP_S32)__LINE__, (BSP_S32) # expr, (BSP_S32)5, (BSP_S32)6); \
    return BSP_ERROR;\
    } \
}

#define BSP_ASSERT_HALT (expr) \
{\
    if (!(expr)) { \
        logMsg("ASSERT failed at:\n  >File name: %s\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n", \
               (BSP_S32)__FILE__, (BSP_S32)__FUNCTION__, (BSP_S32)__LINE__, (BSP_S32) # expr, (BSP_S32)5, (BSP_S32)6); \
        for (;;) ;\
    } \
}


#else
#define BSP_ASSERT(expr)
#define BSP_ASSERT_HALT(expr)
#endif

/*
 * 寄存器操作宏定义
 */
#define BSP_REG(base, reg) (*(volatile BSP_U32 *)((BSP_U32)base + (reg)))
#define BSP_REG8(base, reg) (*(volatile BSP_U8 *)((BSP_U32)base + (reg)))
#define BSP_REG16(base, reg) (*(volatile BSP_U16 *)((BSP_U32)base + (reg)))

/* 将var中[pos, pos + bits-1]比特清零,  pos从0开始编号
   e.g BDOM_CLR_BITS(var, 4, 2) 表示将Bit5~4清零 */
#define BSP_REG_CLRBITS(base, reg, pos, bits) (BSP_REG(base, reg) &= ~((((BSP_U32)1 << (bits)) - 1) << (pos)))

/* 将var中[pos, pos + bits-1]比特设置为val,  pos从0开始编号
   e.g BDOM_SET_BITS(var, 4, 2, 2) 表示将Bit5~4设置为b'10 */
#define BSP_REG_SETBITS(base, reg, pos, bits, val) (BSP_REG(base, reg) = (BSP_REG(base, reg) & (~((((BSP_U32)1 << (bits)) - 1) << (pos)))) \
                                                                         | ((BSP_U32)((val) & (((BSP_U32)1 << (bits)) - 1)) << (pos)))

/* 获取var中[pos, pos + bits-1]比特值,  pos从0开始编号 */
#define BSP_REG_GETBITS(base, reg, pos, bits) ((BSP_REG(base, reg) >> (pos)) & (((BSP_U32)1 << (bits)) - 1))

#define BSP_REG_READ(base, reg, result) \
    ((result) = BSP_REG(base, reg))

#define BSP_REG_WRITE(base, reg, data) \
    (BSP_REG(base, reg) = (data))

#define BSP_REG_WRITEBITS(base, reg, data) \
            (BSP_REG(base, reg) |= (data))
#define BSP_REG8_READ(base, reg, result) \
    ((result) = BSP_REG8(base, reg))

#define BSP_REG16_READ(base, reg, result) \
    ((result) = BSP_REG16(base, reg))

#define BSP_REG_MODIFY(base, reg, clrmsk, setmsk) \
    (BSP_REG(base, reg) = ((BSP_REG(base, reg) & ~clrmsk) | setmsk))

/* 计算最大、最小值 */
#ifndef BSP_MIN
 #define BSP_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef BSP_MAX
 #define BSP_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

//VOID BSP_TRACE(BSP_LOG_LEVEL_E enLogLevel, BSP_U32 enLogModule, const char *fmt, ...);

BSP_VOID BSP_ClearLogModule( BSP_MODULE_E enModule );
BSP_VOID BSP_SetLogModule( BSP_MODULE_E enModule );
BSP_U32  BSP_GetLogModule(BSP_VOID);
BSP_VOID BSP_SetLogLevel(BSP_LOG_LEVEL_E enLogLevel);
BSP_U32  BSP_GetLogLevel(BSP_VOID);
BSP_VOID BSP_SetPrintPos( BSP_BOOL bPrintPos);
BSP_BOOL BSP_GetPrintPos(BSP_VOID);

BSP_S32  BSP_ErrnoGet (BSP_VOID);
BSP_S32  BSP_TaskErrnoGet (BSP_S32 taskId);
BSP_S32  BSP_ErrnoSet (BSP_S32 s32ErrorValue);
BSP_S32  BSP_TaskErrnoSet (BSP_S32 s32TaskId, BSP_S32 s32ErrorValue);
BSP_VOID BSPLogHelp(BSP_VOID);


#define BUILD_DATE_LEN  12
#define BUILD_TIME_LEN  12

typedef struct  tagUE_SW_BUILD_VER_INFO_STRU
{
    BSP_U16 ulVVerNo;        // V部分
    BSP_U16 ulRVerNo;        // R部分
    BSP_U16 ulCVerNo;        // C部分
    BSP_U16 ulBVerNo;        // B部分
    BSP_U16 ulSpcNo;         // SPC部分
    BSP_U16 ulCustomVer;     // 客户需定制部分, 针对硬件接口低8bit PCB号,高8bitHW号
    BSP_U32 ulProductNo;     // such as porting,CPE, ...
    BSP_S8 acBuildDate[BUILD_DATE_LEN];  // build日期,
    BSP_S8 acBuildTime[BUILD_TIME_LEN];  // build时间
} UE_SW_BUILD_VER_INFO_STRU; // 内部版本
/*************************GLOBAL END*****************************/

/*************************TRACE BEGIN*****************************/
void BSP_traceProcessInit();

extern unsigned int g_u32taskTraceBaseAddr;
extern unsigned int g_u32intTraceBaseAddr;

#ifdef BSP_DEBUG
#define PROCESS_TRACE
#endif

#ifdef  PROCESS_TRACE
#define PROCESS_ENTER(pADDRESS) \
    do { \
        *((unsigned int *)pADDRESS) = 1; \
    } while (0)

#define PROCESS_EXIT(pADDRESS) \
    do { \
        *((unsigned int *)pADDRESS) = 0; \
    } while (0)

#else
#define PROCESS_ENTER(pADDRESS)  do { } while (0)
#define PROCESS_EXIT(pADDRESS)   do { } while (0)
#endif

/* 任务对应记录地址定义*/
#define BSP_IPCOM_SYSL_TRACE   (g_u32taskTraceBaseAddr + 0x0 ) /*ipcom_sysl*/
#define BSP_IPCOM_TICK_TRACE   (g_u32taskTraceBaseAddr + 0x04) /*ipcom_tick*/
#define BSP_IPCOM_TELN_TRACE   (g_u32taskTraceBaseAddr + 0x08) /*ipcom_teln*/
#define BSP_SDIO_MONIT_TRACE   (g_u32taskTraceBaseAddr + 0x0c) /*tSdioMonit*/
#define BSP_SDIO_INT_TRACE     (g_u32taskTraceBaseAddr + 0x10) /*tSdioIntrP*/
#define BSP_PMU_INT_TRACE      (g_u32taskTraceBaseAddr + 0x14) /*tPmuIntTas*/
#define BSP_SCISYSNC_TRACE     (g_u32taskTraceBaseAddr + 0x18) /*tSCISYSNC */
#define PS_VOS_TIMER_TRACE     (g_u32taskTraceBaseAddr + 0x1c) /*VOS_TIMER */
#define PS_RTC_TIMER_TRACE     (g_u32taskTraceBaseAddr + 0x20) /*RTC_TIMER */
#define PS_L2_UL_TRACE         (g_u32taskTraceBaseAddr + 0x24) /*MACRLCUL_F*/
#define PS_HPA_TRACE           (g_u32taskTraceBaseAddr + 0x28) /*HPA_FID   */
#define BSP_NET0_TRACE         (g_u32taskTraceBaseAddr + 0x2c) /*tNet0     */
#define BSP_USB_CONTROL_TRACE  (g_u32taskTraceBaseAddr + 0x30) /*uw_Control*/
#define BSP_USB_CORE_TRACE     (g_u32taskTraceBaseAddr + 0x34) /*uw_Core   */
#define MSP_TID005_TRACE       (g_u32taskTraceBaseAddr + 0x38) /*tid005    */
#define PS_L2_DL_TRACE         (g_u32taskTraceBaseAddr + 0x3c) /*MACRLCDL_F*/
#define BSP_USB_CLASS_TRACE    (g_u32taskTraceBaseAddr + 0x40) /*uw_Class  */
#define MSP_SYM_TRACE          (g_u32taskTraceBaseAddr + 0x44) /*mspSYM    */
#define MSP_MUX_TRACE          (g_u32taskTraceBaseAddr + 0x48) /*mspMUX    */
#define MSP_TID006_TRACE       (g_u32taskTraceBaseAddr + 0x4c) /*tid006    */
#define MSP_DIAG_TRACE         (g_u32taskTraceBaseAddr + 0x50) /*mspDIAG   */
#define MSP_SIMM_TRACE         (g_u32taskTraceBaseAddr + 0x54) /*mspSIMM   */
#define MSP_TID013_TRACE       (g_u32taskTraceBaseAddr + 0x58) /*tid013    */
#define MSP_FTM_TRACE          (g_u32taskTraceBaseAddr + 0x5c) /*mspFTM    */
#define MSP_NVIM_TRACE         (g_u32taskTraceBaseAddr + 0x60) /*mspNVIM   */
#define MSP_RAI_TRACE          (g_u32taskTraceBaseAddr + 0x64) /*mspRAI    */
#define BSP_SCIRX_TRACE        (g_u32taskTraceBaseAddr + 0x68) /*tSCIRxTask*/
#define PS_RRC_TRACE           (g_u32taskTraceBaseAddr + 0x6c) /*RRC_FID   */
#define MSP_MUXOM_TRACE        (g_u32taskTraceBaseAddr + 0x70) /*PS_FID_COM*/
#define PS_NAS_MM_TRACE        (g_u32taskTraceBaseAddr + 0x74) /*MM_FID    */
#define PS_NAS_CM_TRACE        (g_u32taskTraceBaseAddr + 0x78) /*CM_FID    */
#define MSP_L4A_TRACE          (g_u32taskTraceBaseAddr + 0x7c) /*mspL4A    */
#define MSP_AT_TRACE           (g_u32taskTraceBaseAddr + 0x80) /*mspAT     */
#define MSP_DRA_TRACE          (g_u32taskTraceBaseAddr + 0x84) /*mspDRA    */
#define MSP_TID012_TRACE       (g_u32taskTraceBaseAddr + 0x88) /*tid012    */
#define PS_OM_TRACE            (g_u32taskTraceBaseAddr + 0x8c) /*OM_FID    */
#define BSP_WATCHDOG_TRACE     (g_u32taskTraceBaseAddr + 0x90) /*WatchDog  */
#define BSP_IPCOM_EGD_TRACE    (g_u32taskTraceBaseAddr + 0x94) /*ipcom_egd */
#define BSP_ARM_SLEEP0_TRACE   (g_u32taskTraceBaseAddr + 0x98) /*tIdletask*/
#define BSP_SDIO_SVC_TRACE     (g_u32taskTraceBaseAddr + 0x9c) /*taskSdioSvc*/
/*modify by lishangfeng for TaskTrace Begin */
#define PS_ERROR_TASK_TRACE    (g_u32taskTraceBaseAddr + 0xa0) /* 公共任务，异常处理时的记录 */
/*modify by lishangfeng for TaskTrace End */


/* 中断对应地址定义*/
#define BSP_WDT_TRACE              (g_u32intTraceBaseAddr + 0x0 ) /*WDT中断                      */
#define BSP_SWINT_TRACE            (g_u32intTraceBaseAddr + 0x04) /*SoftWare INT                 */
#define BSP_COMMRX_TRACE           (g_u32intTraceBaseAddr + 0x08) /*COMMRX中断                   */
#define BSP_COMMTX_TRACE           (g_u32intTraceBaseAddr + 0x0c) /*COMMTX中断                   */
#define BSP_TIMER0_TRACE           (g_u32intTraceBaseAddr + 0x10) /*Timer0中断                   */
#define BSP_TIMER123_TRACE         (g_u32intTraceBaseAddr + 0x14) /*Timer123中断                 */
#define BSP_SCI_TRACE              (g_u32intTraceBaseAddr + 0x18) /*SCI 中断                     */
#define BSP_DMAC_TRACE             (g_u32intTraceBaseAddr + 0x1c) /*DMAC                         */
#define BSP_CIPHER_TRACE           (g_u32intTraceBaseAddr + 0x20) /*CIPHER                       */
#define BSP_USBOTG_TRACE           (g_u32intTraceBaseAddr + 0x24) /*USB OTG                      */
#define BSP_SSP012_TRACE           (g_u32intTraceBaseAddr + 0x28) /*SSP0 |SSP1|SSP2              */
#define BSP_UART01_TRACE           (g_u32intTraceBaseAddr + 0x2c) /*UART0| UART1                 */
#define BSP_RTC_TRACE              (g_u32intTraceBaseAddr + 0x30) /*RTC                          */
#define BSP_NANDC_TRACE            (g_u32intTraceBaseAddr + 0x34) /*NANDC                        */
#define BSP_SDMMC_TRACE            (g_u32intTraceBaseAddr + 0x38) /*SD/MMC 组合                  */
#define BSP_I2C_TRACE              (g_u32intTraceBaseAddr + 0x3c) /*I2C                          */
#define BSP_FE_TRACE               (g_u32intTraceBaseAddr + 0x40) /*FE                           */
#define BSP_GPIO3_TRACE            (g_u32intTraceBaseAddr + 0x44) /*GPIO3组合中断                */
#define BSP_GPIO012_TRACE          (g_u32intTraceBaseAddr + 0x48) /*GPIO0/1/2组合中断            */
#define BSP_RSA_DES_SHA_TRACE      (g_u32intTraceBaseAddr + 0x4c) /*rsa_int | des_int | sha_int  */
#define BSP_IPCM2ARM_TRACE         (g_u32intTraceBaseAddr + 0x50) /*IPCM2ARM中断                 */
#define BSP_PLUSEM0_TRACE          (g_u32intTraceBaseAddr + 0x54) /*PIU_SEM_0                    */
#define BSP_PLUSEM1_TRACE          (g_u32intTraceBaseAddr + 0x58) /*PIU_SEM_1                    */
#define BSP_PLUSEM2_TRACE          (g_u32intTraceBaseAddr + 0x5c) /*PIU_SEM_2                    */
#define BSP_PLUCR_TRACE            (g_u32intTraceBaseAddr + 0x60) /*PIU_CR                       */
#define BSP_PMU01_TRACE            (g_u32intTraceBaseAddr + 0x64) /*pmu0_irq_int_n|pmu1_irq_int_n*/
#define BSP_RESERVED_TRACE         (g_u32intTraceBaseAddr + 0x68) /*保留                         */
#define BSP_BBP_TIMESWITCH_TRACE   (g_u32intTraceBaseAddr + 0x6c) /*BBP时钟切换                  */
#define BSP_BBP_WAKEUP_TRACE       (g_u32intTraceBaseAddr + 0x70) /*BBP 唤醒                     */
#define BSP_BBP_TRACE              (g_u32intTraceBaseAddr + 0x74) /*BBP                          */
#define BSP_BBP_SUBFRAME_TRACE     (g_u32intTraceBaseAddr + 0x78) /*BBP定位中断                  */
#define BSP_BBP_BACKUP_TRACE       (g_u32intTraceBaseAddr + 0x7c) /*BBP 备份合并中断             */

#ifndef UINT8
#define UINT8	unsigned char
#endif
/*************************TRACE END*****************************/


/*************************CLK BEGIN*****************************/

#define BSP_ERR_CLK_NO_FREE_CLK         BSP_DEF_ERR(BSP_MODU_TIMER,(BSP_ERR_SPECIAL + 1))
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Alloc
*
* 功能描述  : 申请用户时钟ID
*
* 输入参数  : BSP_S32 * ps32UsrClkId
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_U32 BSP_USRCLK_Alloc(BSP_S32 * ps32UsrClkId);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Free
*
* 功能描述  : 释放用户时钟
*
* 输入参数  : BSP_S32 s32UsrClkId
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_U32 BSP_USRCLK_Free(BSP_S32 s32UsrClkId);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Connect
*
* 功能描述  : This routine specifies the interrupt service routine to be called
*             at each clock interrupt.  It does not enable usr clock interrupts.
*
* 输入参数  : FUNCPTR routine   routine to be called at each clock interrupt
              BSP_S32 arg	        argument with which to call routine
              BSP_S32 s32UsrClkid      which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the routine cannot be connected to the interrupt.
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
//extern BSP_S32  BSP_USRCLK_Connect(FUNCPTR routine, BSP_S32 arg, BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Disable
*
* 功能描述  : This routine disables user clock interrupts.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32   BSP_USRCLK_Disable (BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Enable
*
* 功能描述  : This routine enables user clock interrupts.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_Enable (BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_RateGet
*
* 功能描述  : This routine returns the interrupt rate of the system aux clock.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : BSP_S32 * pu32ClkRate  clk rate
* 返 回 值  : OK&错误码
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32 BSP_USRCLK_RateGet (BSP_S32 s32UsrClkId, BSP_S32 * pu32ClkRate);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_RateSet
*
* 功能描述  : This routine sets the interrupt rate of the usr clock.
*
* 输入参数  : BSP_S32 ticksPerSecond   number of clock interrupts per second
              BSP_S32 s32UsrClkid         which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32  BSP_USRCLK_RateSet(BSP_S32 ticksPerSecond, BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerStart
*
* 功能描述  : 开启单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
*             u32Cycles   Timer寄存器的计数值
* 输出参数  : 无
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_TimerStart(BSP_S32 s32UsrClkid,BSP_U32 u32Cycles);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerStop
*
* 功能描述  : 关闭单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
* 输出参数  : 无
* 返 回 值  : 无.
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_TimerStop(BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerValue
*
* 功能描述  : 开启单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
* 输出参数  : Value      当前Timer value寄存器的计数值
* 返 回 值  : OK&错误码
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_U32  BSP_USRCLK_TimerValue(BSP_S32 s32UsrClkId, BSP_U32 *pu32Value);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerMilliSecStart
*
* 功能描述  : 以毫秒为单位开启单次Timer定时器.
*
* 输入参数  : s32UsrClkid        Timer Id
*             u32MilliSecond   设置定时器超时的毫秒值
* 输出参数  : 无
* 返 回 值  : OK&其他错误码
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_TimerMilliSecStart(BSP_S32 s32UsrClkid ,BSP_U32 u32MilliSecond);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerMilliSecValue
*
* 功能描述  : 获得当前Timer的毫秒值.
*
* 输入参数  : s32UsrClkid        Timer Id
* 输出参数  : BSP_U32 * pu32Value 当前Timer value寄存器的毫秒值
* 返 回 值  : OK&其他错误码
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_U32 BSP_USRCLK_TimerMilliSecValue(BSP_S32 s32UsrClkId ,BSP_U32 * pu32Value);

/*****************************************************************************
* 函 数 名  : sysClkTicksGet
*
* 功能描述  : 获得系统时钟启动以来的tick数.
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 系统时钟启动以来的tick数.
*
* 修改记录  : 2009年2月24日   liumengcun  creat

*****************************************************************************/
extern BSP_U32 sysClkTicksGet (BSP_VOID);

/*****************************************************************************
* 函 数 名  : sysAuxClkTicksGet
*
* 功能描述  : 获得系统辅助时钟启动以来的tick数.
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 系统辅助时钟启动以来的tick数.
*
* 修改记录  : 2009年2月24日   liumengcun  creat

*****************************************************************************/
BSP_U32 sysAuxClkTicksGet (BSP_VOID);

/*****************************************************************************
* 函 数 名  : sysTimestampRateSet
*
* 功能描述  : This routine sets the interrupt rate of the timestamp clock.  It does
*             not enable system clock interrupts unilaterally, but if the timestamp is currently enabled, the clock is disabled and then
*             re-enabled with the new rate.  Normally it is called by usrRoot()
*             in usrConfig.c.
*
* 输入参数  : int ticksPerSecond   number of clock interrupts per second
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 sysTimestampRateSet(BSP_S32 ticksPerSecond);

/*****************************************************************************
* 函 数 名  : sysTimestampRateGet
*
* 功能描述  : This routine returns the interrupt rate of the timestamp clock.
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : The number of ticks per second of the system clock.
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 sysTimestampRateGet (BSP_VOID);
/*************************CLK END*****************************/


/*************************SCI BEGIN*****************************/
typedef BSP_U32 (*Sci_Event_Func)(BSP_U32 u32Event, BSP_VOID* para);

/*错误码*/
#define BSP_ERR_SCI_NOTINIT              951
#define BSP_ERR_SCI_INVALIDFREQ          952
#define BSP_ERR_SCI_USEDEFAULT           953
#define BSP_ERR_SCI_INVALIDPARA          954
#define BSP_ERR_SCI_NOCARD               955
#define BSP_ERR_SCI_NODATA               956
#define BSP_ERR_SCI_NOTREADY             957
#define BSP_ERR_SCI_DISABLED             958
#define BSP_ERR_SCI_UNSUPPORTED          959
#define BSP_ERR_SCI_INSERROR             960
#define BSP_ERR_SCI_GETAPDU_ERROR        961
#define BSP_ERR_SCI_VLTG_HIGHEST         962
#define BSP_ERR_SCI_CURRENT_STATE_ERR    963
#define BSP_ERR_SCI_CURRENT_VLTG_ERR     964
#define BSP_ERR_SCI_TIMESTOP_TIMEOUT     965


/*回调函数注册事件类型*/
#define SCI_EVENT_CARD_IN                0x1
#define SCI_EVENT_CARD_OUT               0x2
#define SCI_EVENT_CARD_DEACTIVE_SUCCESS  0x4
#define SCI_EVENT_CARD_RESET_SUCCESS     0x8
#define SCI_EVENT_CARD_READ_DATA         0x10
#define SCI_EVENT_CARD_TX_ERR            0x20

#define SCI_DEF_CARD_OK     0x0 /* 卡正常，可以使用*/
#define SCI_DEF_CARD_ERROR  0x1   /* 无卡或者卡异常*/


typedef enum tagSCI_CLK_STOP_TYPE_E
{
    SCI_CLK_STATE_LOW = 0,        /* 协议侧指示停低*/
    SCI_CLK_STATE_HIGH = 1,       /*协议侧指示停高*/
    SCI_CLK_STATE_UNSUPPORTED = 2,/*协议侧指示不支持*/
    SCI_CLK_STATE_BUTT
}SCI_CLK_STOP_TYPE_E;

/*****************************************************************************
* 函 数 名  : BSP_SCI_Reset
*
* 功能描述  : 本接口用于复位SCI（Smart Card Interface）驱动和USIM（Universal
*           Subscriber Identity Module）卡
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : OK  复位成功
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_Reset();

/*****************************************************************************
* 函 数 名  : BSP_SCI_GetCardStatus
*
* 功能描述  : 本接口用于获得卡当前的状态
*
* 输入参数  : 无
* 输出参数  :
*           BSP_U32 *u32CardStatus     当前卡的状态。
*                                       0：卡可用；
*                                       1：卡不可用；
* 返 回 值  : OK
*           BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetCardStatus( BSP_U32 *u32CardStatus);

/*****************************************************************************
* 函 数 名  : BSP_SCI_SendData
*
* 功能描述  : 本接口用于发送一段数据到USIM卡
*
* 输入参数  : BSP_U32 u32DataLength 发送数据的有效长度。取值范围1～256，单位为字节
*             BSP_U8 *pu8DataBuffer 发送数据所在内存的首地址，如是动态分配，调用接口
*                                   后不能立即释放，依赖于硬件发完数据
* 输出参数  : 无
*
* 返 回 值  : OK
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_INVALIDPARA
*           BSP_ERR_SCI_DISABLED
*           BSP_ERR_SCI_NOCARD
*           BSP_ERR_SCI_NODATA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_SendData(BSP_U32 u32DataLength, BSP_U8 *pu8DataBuffer);

/*****************************************************************************
* 函 数 名  : BSP_SCI_ReceiveSync
*
* 功能描述  : 本接口用于USIM Manager读取来自SCI Driver的卡返回数据
*             该接口为阻塞接口，只有SCI接收到足够的数据量后才会返回；
*             该接口的超时门限为1s
*
* 输入参数  : BSP_U32 u32DataLength USIM Manager欲从SCI Driver读取的数据长度。
* 输出参数  : BSP_U8 *pu8DataBuffer USIM Manager指定的Buffer，SCI Driver将数据拷贝到本Buffer。
* 返 回 值  : OK
*             BSP_ERR_SCI_NOTINIT
*             BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_ReceiveSync(BSP_U32 u32Length,BSP_U8 *pu8Data);

/*****************************************************************************
* 函 数 名  : BSP_SCI_ReceiveAll
*
* 功能描述  : 本接口用于USIM Manager在读数超时的时候，调用本函数，读取接收数据缓冲中的所有数据
*
* 输入参数  : 无
* 输出参数  : BSP_U32 *u32DataLength Driver读取的数据长度，返回给USIM Manager。取值范围1～256，单位是字节
*           BSP_U8 * pu8DataBuffer USIM Manager指定的Buffer，SCI Driver将数据拷贝到本Buffer
*
* 返 回 值  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_ReceiveAll(BSP_U32 *u32DataLength,BSP_U8 * pu8DataBuffer);

/*****************************************************************************
* 函 数 名  : BSP_SCI_GetATR
*
* 功能描述  : 本接口用于将Driver层缓存的ATR数据和数据个数返回给USIM Manager层
*
* 输入参数  : BSP_VOID
* 输出参数  : BSP_U8 *u8DataLength  Driver读取的ATR数据长度，返回给USIM Manager。
*                                   取值范围0～32，单位是字节
*           BSP_U8 *pu8ATR          USIM Manager指定的Buffer，SCI Driver将ATR
*                                   数据拷贝到本Buffer。一般为操作系统函数动态分配
*                                   或者静态分配的地址
*
*
* 返 回 值  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetATR (unsigned long  *u8DataLength, BSP_U8 *pu8ATR);

/*****************************************************************************
* 函 数 名  : BSP_SCI_ClassSwitch
*
* 功能描述  : 本接口用于支持PS对卡的电压类型进行切换，从1.8V切换到3V
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  :  OK    当前电压不是最高的，进行电压切换操作
*           BSP_ERR_SCI_CURRENT_STATE_ERR 切换失败 current SCI driver state is ready/rx/tx
*           BSP_ERR_SCI_VLTG_HIGHEST   当前电压已经是最高电压，没有进行电压切换
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_CURRENT_VLTG_ERR 当前电压值异常（非class B或者C）
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_ClassSwitch();

/*****************************************************************************
* 函 数 名  : BSP_SCI_ShowVltgVal
*
* 功能描述  : 本接口用于显示当前SIM卡接口电压
*
* 输入参数  : 无
* 输出参数  : BSP_U32 * pu32Vltgval 当前电压
*
* 返 回 值  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_ShowVltgVal(BSP_U32 * pu32Vltgval);

/*****************************************************************************
* 函 数 名  : BSP_SCI_TimeStop
*
* 功能描述  : 本接口用于支持PS关闭SIM卡时钟
*
* 输入参数  :
*           SCI_CLK_STOP_TYPE_E enTimeStopCfg 时钟停止模式
*
* 输出参数  : 无
*
* 返 回 值  : OK - successful completion
*               ERROR - failed
*               BSP_ERR_SCI_NOTINIT
*               BSP_ERR_SCI_INVALIDPARA - invalid mode specified
*               BSP_ERR_SCI_UNSUPPORTED - not support such a operation
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_S32 BSP_SCI_TimeStop(unsigned long enTimeStopCfg);

/*****************************************************************************
* 函 数 名  : BSP_SCI_Deactive
*
* 功能描述  : 本接口用于对SIM卡的去激活操作
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : OK
*             BSP_ERR_SCI_NOTINIT
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_Deactive(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_SCI_GetClkStatus
*
* 功能描述  : 本接口用于获取当前SIM卡时钟状态
*
* 输入参数  : BSP_U32 *pu32SciClkStatus   变量指针，用于返回SIM卡时钟状态：
*                                       0：时钟已打开；
*                                       1：时钟停止
* 输出参数  : 无
*
* 返 回 值  : OK    操作成功
*          BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetClkStatus(BSP_U32 *pu32SciClkStatus);

/*****************************************************************************
* 函 数 名  : BSP_SCI_RegEvent
*
* 功能描述  : 本接口用于注册回调函数
*
* 输入参数  : BSP_U32 u32Event      事件类型，用掩码区分：
*                                    0x1：插卡；
*                                    0x2：拔卡；
*                                    0x4：卡下电；
*                                    0x8：卡上电；
*                                    0x10：收数据；
*                                    0x20：发送数据错误
*              Sci_Event_Func * pFunc   回调函数
* 输出参数  : 无
*
* 返 回 值  : OK    操作成功
*          BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_RegEvent(BSP_U32 u32Event, Sci_Event_Func pFunc);

/*****************************************************************************
* 函 数 名  : BSP_SCI_GetClkFreq
*
* 功能描述  : 本接口用于获取当前SIM卡时钟频率
*
* 输入参数  : 无
*
* 输出参数  : BSP_U32 *pLen   时钟频率数据的长度
*             BSP_U8 *pBuf    时钟频率数据
* 返 回 值  : OK    操作成功
*             BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 2010年8月24日   zhouluojun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetClkFreq(unsigned long *pLen, unsigned char *pBuf);

/*****************************************************************************
* 函 数 名  : BSP_SCI_GetBaudRate
*
* 功能描述  : 本接口用于获取当前SIM卡时钟频率
*
* 输入参数  : 无
*
* 输出参数  : BSP_U32 *pLen   波特率数据的长度
*             BSP_U8 *pBuf    波特率数据
*
* 返 回 值  : OK    操作成功
*             BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 2010年8月24日   zhouluojun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetBaudRate(unsigned long *pLen, BSP_U8 *pBuf);

/*****************************************************************************
* 函 数 名  : BSP_SCI_GetPCSCParameter
*
* 功能描述  : 本接口用于获取当前SIM卡的PCSC相关参数
*
* 输入参数  : 无
*
* 输出参数  : BSP_U8 *pBuf    PCSC相关参数
*
* 返 回 值  : OK    操作成功
*             BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 2010年8月24日   zhouluojun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetPCSCParameter(BSP_U8 *pBuf);

/*****************************************************************************
* 函 数 名  : BSP_SCI_RecordDataSave
*
* 功能描述  : 本接口用于保存可维可测信息到yaffs中
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : OK    操作成功
*             其他   操作失败
*
* 修改记录  : 2011年5月21日   yangzhi  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_RecordDataSave();

/*************************SCI END*************************************/

/*************************PMU BEGIN*****************************/

/* 错误码 */
#define BSP_ERR_PMU_PARA_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 0x50)
#define BSP_ERR_PMU_ERROR_INIT        BSP_DEF_ERR(BSP_MODU_SPI, 0x51)
#define BSP_ERR_PMU_SPI_NOT_INIT      BSP_DEF_ERR(BSP_MODU_SPI, 0x52)
#define BSP_ERR_PMU_NOT_INIT          BSP_DEF_ERR(BSP_MODU_SPI, 0x53)
/**************************************************************************
  全局变量声明
**************************************************************************/


/**************************************************************************
  枚举定义
**************************************************************************/
/*DVFS type*/
typedef enum tagPMU_DVFS_TYPE_E
{
	DVFS_ARMCORE = 0,
	DVFS_CEVACORE,
	DVFS_BUTTOM
}PMU_DVFS_TYPE_E;

/*DVFS Voltage type*/
typedef enum tagPMU_DVFS_VOLTAGE_TYPE_E
{
	DVFS_VOLTAGE_1000 = 0,
	DVFS_VOLTAGE_1100,
	DVFS_VOLTAGE_1150,
	DVFS_VOLTAGE_1200,
	DVFS_VOLTAGE_1250,
	DVFS_VOLTAGE_1350,
	DVFS_VOLTAGE_1500,
	DVFS_VOLTAGE_1800,
    DVFS_VOLTAGE_BUTTOM
}PMU_DVFS_VOLTAGE_TYPE_E;

 /*DEVICE type*/
typedef enum tagPMU_DEVS_TYPE_E
{
	DEVS_RF = 0,
	DEVS_ABB,
	DEVS_SD,
	DEVS_SIM,
    DEVS_BUTTOM
}PMU_DEVS_TYPE_E;


/*DEVICE CTRL type*/
typedef enum tagPMU_DEVS_CTRL_TYPE_E
{
	CTRL_ON = 0,
	CTRL_OFF,
    CTRL_BUTTOM
}PMU_DEVS_CTRL_TYPE_E;

 /*OCP type*/
typedef enum tagPMU_OCP_TYPE_E
{
	OCP_ONLYINT_NOAUTOOFF = 0,
	OCP_INT_AND_AUTOOFF,
	OCP_AUTOOFF,
    OCP_BUTTOM
}PMU_OCP_TYPE_E;

 /* VOLTAGE ADJUST REG */
typedef enum tagPMU_VOL_SCI_E
{
	PMU_VOL_UP = 0, /* 由1.8伏升级到3.0伏*/
	PMU_VOL_DOWN,   /* 由3.0伏降到1.8伏*/
	PMU_VOL_SCI_BUTTOM
}PMU_VOL_SCI_E;


/* LED MODE */
typedef enum tagPMU_LED_STATE_E
{
	PMU_LED_FLASH_POWER_OFF = 0,    /* 全部不闪 */
	PMU_LED_FLASH_BLUE_ON,          /* 蓝灯长亮 */
	PMU_LED_FLASH_BLUE_SLOW,        /* 蓝灯慢闪 */
	PMU_LED_FLASH_BLUE_FAST,        /* 蓝灯快闪 */
	PMU_LED_FLASH_BLUE_ONCE,        /* 蓝灯单闪 */
	PMU_LED_FLASH_BLUE_TWICE,       /* 蓝灯双闪 */
	PMU_LED_FLASH_BLUE_FOUR,        /* 蓝灯四闪 */
	PMU_LED_FLASH_RED_ON,           /* 红灯长亮 */
	PMU_LED_FLASH_RED_SLOW,         /* 红灯慢闪 */
	PMU_LED_FLASH_RED_FAST,         /* 红灯快闪 */
	PMU_LED_FLASH_RED_ONCE,         /* 红灯单闪 */
	PMU_LED_FLASH_RED_TWICE,        /* 红灯双闪 */
	PMU_LED_FLASH_RED_FOUR,         /* 红灯四闪 */
	PMU_LED_FLASH_GREEN_ON,         /* 绿灯长亮 */
	PMU_LED_FLASH_GREEN_SLOW,       /* 绿灯慢闪 */
	PMU_LED_FLASH_GREEN_FAST,       /* 绿灯快闪 */
	PMU_LED_FLASH_GREEN_ONCE,       /* 绿灯单闪 */
	PMU_LED_FLASH_GREEN_TWICE,      /* 绿灯双闪 */
	PMU_LED_FLASH_GREEN_FOUR,       /* 绿灯四闪 */
	PMU_LED_STATE_BUTT
}PMU_LED_STATE_E;


/**************************************************************************
  STRUCT定义
**************************************************************************/
/*SPI的ID，ASIC上使用3片SPI*/
typedef enum tagSPI_DEV_ID
{
    SPI_ID0,
    SPI_ID1,
    SPI_ID_MAX
}SPI_DEV_ID_E;


/*每个SPI上的片选号，当前每个SPI有4个片选*/
typedef enum tagSPI_DEV_CS
{
    SPI_DEV_CS0,
    SPI_DEV_CS1,
    SPI_DEV_CS2,
    SPI_DEV_CS3,
    SPI_DEV_CS_MAX,
    NO_OWNER = -1
}SPI_DEV_CS_E;


/*数据传输方式，包括DMA方式和轮循方式*/
typedef enum tagSPI_SENDMOD
{
    SPI_SENDMOD_POLLING,
    SPI_SENDMOD_DMA,
    SPI_SENDMOD_MAX
}RXTX_MODE_E;


/* 记录SPI的ID和片选号的结构体*/
typedef struct tagSPI_DEV_STRUCTION
{
    SPI_DEV_ID_E enSpiId;
    SPI_DEV_CS_E enSpiCs;
}SPI_DEV_S;

/* 控制命令字长度 */
typedef enum tagspi_CommandLen
{
    SPI_COMMANDSIZE_1BITS =  0x0,     /*  1 bit command */
    SPI_COMMANDSIZE_2BITS =  0x1,     /*  2 bit command */
    SPI_COMMANDSIZE_3BITS =  0x2,     /*  3 bit command */
    SPI_COMMANDSIZE_4BITS  = 0x3,     /*  4 bit command */
    SPI_COMMANDSIZE_5BITS  = 0x4,     /*  5 bit command */
    SPI_COMMANDSIZE_6BITS  = 0x5,     /*  6 bit command */
    SPI_COMMANDSIZE_7BITS  = 0x6,     /*  7 bit command */
    SPI_COMMANDSIZE_8BITS  = 0x7,     /*  8 bit command */
    SPI_COMMANDSIZE_9BITS  = 0x8,     /*  9 bit command */
    SPI_COMMANDSIZE_10BITS = 0x9,     /* 10 bit command */
    SPI_COMMANDSIZE_11BITS = 0xA,     /* 11 bit command */
    SPI_COMMANDSIZE_12BITS = 0xB,     /* 12 bit command */
    SPI_COMMANDSIZE_13BITS = 0xC,     /* 13 bit command */
    SPI_COMMANDSIZE_14BITS = 0xD,     /* 14 bit command */
    SPI_COMMANDSIZE_15BITS = 0xE,     /* 15 bit command */
    SPI_COMMANDSIZE_16BITS = 0xF,      /* 16 bit command */
    SPI_COMMANDSIZE_MAX
} SPI_COMMAND_LEN_E;

/*数据帧长度*/
typedef enum tagspi_DataLen
{
    SPI_DATASIZE_4BITS  = 0x3,     /*  4 bit data */
    SPI_DATASIZE_5BITS  = 0x4,     /*  5 bit data */
    SPI_DATASIZE_6BITS  = 0x5,     /*  6 bit data */
    SPI_DATASIZE_7BITS  = 0x6,     /*  7 bit data */
    SPI_DATASIZE_8BITS  = 0x7,     /*  8 bit data */
    SPI_DATASIZE_9BITS  = 0x8,     /*  9 bit data */
    SPI_DATASIZE_10BITS = 0x9,     /* 10 bit data */
    SPI_DATASIZE_11BITS = 0xA,     /* 11 bit data */
    SPI_DATASIZE_12BITS = 0xB,     /* 12 bit data */
    SPI_DATASIZE_13BITS = 0xC,     /* 13 bit data */
    SPI_DATASIZE_14BITS = 0xD,     /* 14 bit data */
    SPI_DATASIZE_15BITS = 0xE,     /* 15 bit data */
    SPI_DATASIZE_16BITS = 0xF,     /* 16 bit data */
    SPI_DATASIZE_MAX
} SPI_DATA_LEN_E;

/* SPI支持的协议*/
typedef enum tagSPI_PROT
{
    SPI_PROT_SPI,
    SPI_PROT_SSP,
    SPI_PROT_NSM,
    SPI_PROT_MAX
}SPI_PROT_E;

typedef enum tagSPI_SCPOL
{
	SPI_SCPOL_LOW, //下降沿触发
	SPI_SCPOL_HIGH,//上升沿触发
	SPI_SCPOL_MAX
}SPI_SCPOL_E;

/*SPI外接设备特性结构体，包括设备需要的命令字长度，数据帧长度，使用协议等 */
typedef struct tagSPI_DEV_ATTR
{
    SPI_COMMAND_LEN_E enCommandLen;
    SPI_DATA_LEN_E enDataLen;
    SPI_PROT_E enSpiProt;
	SPI_SCPOL_E enSpiScpol;
    BSP_U16 u16SpiBaud;
}SPI_DEV_ATTR_S;

/*记录与SPI数据传输相关信息的结构体，成员包括要读写的SPI号，片选号，传输模式，
  数据地址，数据长度等*/
typedef struct tagSPI_DATA_HANDLE
{
    SPI_DEV_ID_E enSpiID;
    SPI_DEV_CS_E enCsID;
    RXTX_MODE_E enMode;
    void *pvCmdData;
    BSP_U32 u32length;
}SPI_DATA_HANDLE_S;

/*****************************************************************************
* 函 数 名  : BSP_PMU_ShutDown
*
* 功能描述  : 本接口用于关闭PMU
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  :
* 修改记录  :
*
*****************************************************************************/
BSP_VOID BSP_PMU_ShutDown();
/*****************************************************************************
* 函 数 名  : BSP_PMU_DVFS_VolAdjust
*
* 功能描述  : DVFS 电压调整
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
* 修改记录  :
*
*****************************************************************************/
BSP_S32 BSP_PMU_DVFS_VolAdjust(PMU_DVFS_TYPE_E enDvfsType,PMU_DVFS_VOLTAGE_TYPE_E enVoltage);
/*****************************************************************************
* 函 数 名  : BSP_PMU_OCPConfig
*
* 功能描述  : OCPC配置
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
* 修改记录  :
*
*****************************************************************************/
BSP_S32 BSP_PMU_OCPConfig(PMU_OCP_TYPE_E enOcpType);
/*****************************************************************************
* 函 数 名  : BSP_PMU_DEVS_Ctrl
*
* 功能描述  : DEVS　控制
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
* 修改记录  :
*
*****************************************************************************/
BSP_S32 BSP_PMU_DEVS_Ctrl(PMU_DEVS_TYPE_E enDevsType, PMU_DEVS_CTRL_TYPE_E enCtrlType);
/*****************************************************************************
* 函 数 名  : BSP_PMU_VolAdjust_ForSci
*
* 功能描述  : SCI电压控制
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
* 修改记录  :
*
*****************************************************************************/
BSP_S32 BSP_PMU_VolAdjust_ForSci(PMU_VOL_SCI_E enSCIVolCfg);

/*****************************************************************************
* 函 数 名  : BSP_PMU_CutOff_ForDrx
*
* 功能描述  : 低功耗场景PMU下电
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  :
* 修改记录  : 正确/错误
*
*****************************************************************************/
BSP_S32 BSP_PMU_CutOff_ForDrx();

/*****************************************************************************
* 函 数 名  : BSP_PMU_SetLED
*
* 功能描述  : 设置LTD灯的开关
*
* 输入参数  : enLEDType         LED0
*                               LED1
*                               LED2
*             enPower           PMU_LED_POWER_ON
*                               PMU_LED_POWER_OFF
* 输出参数  : 无
*
* 返 回 值  : OK：配置成功
*             ERROR：配置失败
*****************************************************************************/
BSP_S32 BSP_PMU_SetLedState(PMU_LED_STATE_E eLedState);

/*************************PMU END*****************************/
/*************************NCM BEGIN*****************************/
/* 错误码*/
/*专用错误码*/
#define BSP_ERR_SPECIAL (0x0)
#define BSP_ERR_NCM_NOTXBUFF         BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 1)) /*0x10120001*/
#define BSP_ERR_NCM_NOIDLEDEV        BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 2)) /*0x10120002*/
#define BSP_ERR_NCM_AT_SEND_TIMEOUT  BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 3))/*0x10121003*/
#define BSP_ERR_NCM_WRONG_STATE      BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 4))/*0x10121004*/
#define BSP_ERR_NCM_AT_SEND_FAIL     BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 5))/*0x10121005*/
#define BSP_ERR_NCM_SEND_EXCEED_MAXPKT  BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 6))/*0x10121006*/

typedef struct tagNCM_PKT_S
{
    BSP_U8 *pBuffer;       /* buffer指针*/
    BSP_U32  u32BufLen;      /* buffer长度 */
}NCM_PKT_S;

/* NCM设备类型枚举*/
typedef enum tagNCM_DEV_TYPE_E
{
    NCM_DEV_DATA_TYPE,      /* 数据通道类型，PS使用*/
    NCM_DEV_CTRL_TYPE       /* 控制通道，MSP传输AT命令使用*/
}NCM_DEV_TYPE_E;

/* 上行线路收包函数指针 */
typedef BSP_VOID (*USBUpLinkRxFunc)(BSP_VOID * pPktNode);

/* 包封装释放函数指针 */
typedef BSP_VOID (*USBFreePktEncap)(BSP_VOID *PktEncap, BSP_BOOL bNodeOnly);

/* USB IOCTL枚举 */
typedef enum tagNCM_IOCTL_CMD_TYPE_E
{
    NCM_IOCTL_NETWORK_CONNECTION_NOTIF,      /* 0x0,NCM网络是否连接上*/
    NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF, /* 0x1,NCM设备协商的网卡速度*/
    NCM_IOCTL_SET_PKT_ENCAP_INFO,            /* 0x2,设置包封装格式*/
    NCM_IOCTL_REG_UPLINK_RX_FUNC,            /* 0x3,注册上行收包回调函数*/
    NCM_IOCTL_REG_FREE_PKT_FUNC,             /* 0x4,注册释放包封装回调函数*/
    NCM_IOCTL_FREE_BUFF,                     /* 0x5,释放底软buffer*/
    NCM_IOCTL_GET_USED_MAX_BUFF_NUM,         /* 0x6,获取上层可以最多占用的ncm buffer个数*/
    NCM_IOCTL_GET_DEFAULT_TX_MIN_NUM,        /* 0x7,获取默认发包个数阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_GET_DEFAULT_TX_TIMEOUT,        /* 0x8,获取默认发包超时时间，超过该时间会启动一次NCM传输*/
    NCM_IOCTL_GET_DEFAULT_TX_MAX_SIZE,       /* 0x9,获取默认发包字节阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_SET_TX_MIN_NUM,                /* 0xa,设置发包个数阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_SET_TX_TIMEOUT,                /* 0xb,设置发包超时时间，超过该时间会启动一次NCM传输*/
    NCM_IOCTL_SET_TX_MAX_SIZE,               /* 0xc,该命令字不再使用。设置发包字节阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_GET_RX_BUF_SIZE,               /* 0xd,获取收包buffer大小*/
    NCM_IOCTL_FLOW_CTRL_NOTIF,               /* 0xe,流控控制开关*/
    NCM_IOCTL_REG_AT_PROCESS_FUNC,           /* 0xf,注册AT命令处理回调函数*/
    NCM_IOCTL_AT_RESPONSE,                   /* 0x10,AT命令回应*/
    NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC,     /* 0x11,注册网卡状态改变通知回调函数*/
    NCM_IOCTL_SET_PKT_STATICS,               /* 0x12,配置统计信息*/

    NCM_IOCTL_GET_FLOWCTRL_STATUS,           /* 0x13 查询NCM流控状态*/
    /* END:   Modified by liumengcun, 2011-4-21 */


    NCM_IOCTL_GET_CUR_TX_MIN_NUM,              /* 0x14 获取当前发包个数阈值*/
    NCM_IOCTL_GET_CUR_TX_TIMEOUT,               /* 0x15 获取当前发包超时时间*/
    NCM_IOCTL_IPV6_DNS_NOTIF,              /*0x16 IPV6 DNS主动上报*/
    /* END:   Modified by liumengcun, 2011-6-23 */
    /* BEGIN: Modified by liumengcun, 2011-7-20 支持IPV6 DNS配置*/
    NCM_IOCTL_SET_IPV6_DNS,                     /* 0x16 配置IPV6 DNS*/
    /* END:   Modified by liumengcun, 2011-7-20 */
    /* BEGIN: Modified by liumengcun, 2011-8-10 MSP新需求*/
    NCM_IOCTL_CLEAR_IPV6_DNS,                     /* 0x17 清除IPV6 DNS在板端的缓存,param在此命令字没有意义，不填空指针即可*/
    NCM_IOCTL_GET_NCM_STATUS,                     /* 0x18 获取NCM网卡状态 enable:TRUE(1);disable:FALSE(0) */
    /* END:   Modified by liumengcun, 2011-8-10 */

    NCM_IOCTL_SET_ACCUMULATION_TIME,

    /* BEGIN: Modified by baoxianchun, 2012-5-17 GU PS 新需求*/
	NCM_IOCTL_SET_RX_MIN_NUM,		/*配置收包个数阈值*/
	NCM_IOCTL_SET_RX_TIMEOUT,			/*配置收包超时时间*/
    /* END: Modified by baoxianchun, 2012-5-17 GU PS 新需求*/
    NCM_IOCTL_REG_NDIS_RESP_STATUS_FUNC   /* NDIS通道AT命令状态处理回调函数 */

}NCM_IOCTL_CMD_TYPE_E;

/* NCM连接状态枚举,NCM_IOCTL_NETWORK_CONNECTION_NOTIF命令字对应参数枚举*/
typedef enum tagNCM_IOCTL_CONNECTION_STATUS_E
{
    NCM_IOCTL_CONNECTION_LINKDOWN,      /* NCM网络断开连接*/
    NCM_IOCTL_CONNECTION_LINKUP         /* NCM网络连接*/
}NCM_IOCTL_CONNECTION_STATUS_E;

/* NCM连接速度结构,NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF命令字对应参数结构体*/
typedef struct tagNCM_IOCTL_CONNECTION_SPEED_S
{
    BSP_U32 u32DownBitRate;
    BSP_U32 u32UpBitRate;
}NCM_IOCTL_CONNECTION_SPEED_S;

/* 包封装结构体,NCM_IOCTL_SET_PKT_ENCAP_INFO命令字对应参数结构体*/
typedef struct tagNCM_PKT_ENCAP_INFO_S
{
    BSP_S32 s32BufOft;      /* buf偏移量 */
    BSP_S32 s32LenOft;      /* len偏移量 */
    BSP_S32 s32NextOft;     /* next偏移量 */
}NCM_PKT_ENCAP_INFO_S;

/* AT命令回复数据指针及长度 NCM_IOCTL_AT_RESPONSE*/
typedef struct tagNCM_AT_RSP_S
{
    BSP_U8* pu8AtAnswer;
    BSP_U32 u32Length;
} NCM_AT_RSP_S;

/* AT命令接收函数指针，该函数为同步接口，对应NCM_IOCTL_REG_AT_PROCESS_FUNC命令字*/
typedef BSP_VOID (*USBNdisAtRecvFunc)(BSP_U8 * pu8Buf, BSP_U32 u32Len);

/* NCM流控开关枚举,NCM_IOCTL_NETWORK_CONNECTION_NOTIF命令字对应参数枚举*/
typedef enum tagNCM_IOCTL_FLOW_CTRL_E
{
    NCM_IOCTL_FLOW_CTRL_ENABLE,      /* 打开流控*/
    NCM_IOCTL_FLOW_CTRL_DISABLE      /* 关闭流控*/
}NCM_IOCTL_FLOW_CTRL_E;

/* NCM网卡状态改变通知枚举,NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC命令字对应参数枚举*/
typedef enum tagNCM_IOCTL_CONNECT_STUS_E
{
    NCM_IOCTL_STUS_CONNECT,      /* 建链*/
    NCM_IOCTL_STUS_BREAK         /* 网卡断开,断链*/
}NCM_IOCTL_CONNECT_STUS_E;

typedef enum tagHKADC_TEMP_PROTECT_E
{
    HKADC_TEMP_BATTERY,
    HKADC_TEMP_PA0,
    HKADC_TEMP_PA1,
    HKADC_TEMP_DCXO0,
    HKADC_TEMP_DCXO1,
    HKADC_TEMP_LCD,
    HKADC_TEMP_SIM_CARD,
    HKADC_TEMP_BUTT
}HKADC_TEMP_PROTECT_E;

/* 网卡状态切换通知函数，对应NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC命令字*/
typedef BSP_VOID (*USBNdisStusChgFunc)(NCM_IOCTL_CONNECT_STUS_E enStatus, BSP_VOID * pBuffer);

/*  下传上传、下载包的各种统计信息，
    不发送给PC，被动等待PC的读取,NCM_IOCTL_SET_PKT_STATICS命令字对应参数结构*/
typedef struct tagNCM_IOCTL_PKT_STATISTICS_S
{
    BSP_U32     u32TxOKCount;         /*发送包数*/
    BSP_U32     u32RxOKCount;         /*接收包数*/
    BSP_U32     u32TxErrCount;        /*发送错误*/
    BSP_U32     u32RxErrCount;        /*接收错误*/
    BSP_U32     u32TxOverFlowCount;   /*发送溢出丢包*/
    BSP_U32     u32RxOverFlowCount;   /*接收溢出丢包*/
    BSP_U32     u32CurrentTx;         /*发送速率*/
    BSP_U32     u32CurrentRx;         /*接收速率*/
} NCM_IOCTL_PKT_STATISTICS_S;

/* BEGIN: Modified by liumengcun, 2011-7-20 IPV6 DNS配置结构,NCM_IOCTL_SET_IPV6_DNS对应参数结构*/
#define BSP_NCM_IPV6_DNS_LEN     32
 typedef struct tagNCM_IPV6DNS_S  /* 0x16 配置IPV6 DNS*/
 {
     BSP_U8 * pu8Ipv6DnsInfo;/* 32字节，低16字节表示primaryDNS；高16字节表示SecondaryDNS。*/
     BSP_U32 u32Length;
 } NCM_IPV6DNS_S;
/* END:   Modified by liumengcun, 2011-7-20 */
typedef struct tagNCM_PKT_INFO_S
{
    BSP_U32 u32PsRcvPktNum;              /* 收包送到PS的包个数*/
    BSP_U32 u32RcvUnusedNum;             /* 收包不符合PS要求丢弃包个数*/
    BSP_U32 u32NcmSendPktNum;            /* 发包个数*/
}NCM_PKT_INFO_S;


/*****************************************************************************
* 函 数 名  : BSP_NCM_Open
*
* 功能描述  : 用来获取可以使用NCM设备ID,数据通道类型，PS使用;控制通道类型，
*              MSP传输AT命令使用
*
* 输入参数  : NCM_DEV_TYPE_E enDevType  设备类型
* 输出参数  : pu32NcmDevId              NCM 设备ID
*
* 返 回 值  : BSP_ERR_NET_NOIDLEDEV
*             OK
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Open(NCM_DEV_TYPE_E enDevType, BSP_U32 * pu32NcmDevId);

/*****************************************************************************
* 函 数 名  : BSP_NCM_Write
*
* 功能描述  : 发送数据
*
* 输入参数  : BSP_U32 u32NcmDevId  NCM设备ID
* 输出参数  : BSP_VOID *pPktEncap       包封装首地址
*
* 返 回 值  : BSP_OK
*             BSP_ERR_NET_INVALID_PARA
*             BSP_ERR_NET_BUF_ALLOC_FAILED
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Write(BSP_U32 u32NcmDevId, BSP_VOID *pPktEncap);

/*****************************************************************************
* 函 数 名  : BSP_NCM_Ioctl
*
* 功能描述  : 配置NCM设备属性
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  :
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Ioctl(BSP_U32 u32NcmDevId, NCM_IOCTL_CMD_TYPE_E enNcmCmd, BSP_VOID *param);

/*****************************************************************************
* 函 数 名  : BSP_NCM_Close
*
* 功能描述  : 关闭NCM设备
*
* 输入参数  : NCM_DEV_TYPE_E enDevType   设备ID类型
*             BSP_U32 u32NcmDevId        NCM设备ID
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Close(NCM_DEV_TYPE_E enDevType, BSP_U32 u32NcmDevId);
/*************************NCM END*****************************/

/*************************ACM BEG*****************************/
/* IOCTL CMD 定义 */
#define ACM_IOCTL_SET_WRITE_CB      0x7F001000
#define ACM_IOCTL_SET_READ_CB       0x7F001001
#define ACM_IOCTL_SET_EVT_CB        0x7F001002
#define ACM_IOCTL_SET_FREE_CB       0x7F001003


#define ACM_IOCTL_WRITE_ASYNC       0x7F001010
#define ACM_IOCTL_GET_RD_BUFF       0x7F001011
#define ACM_IOCTL_RETURN_BUFF       0x7F001012
#define ACM_IOCTL_RELLOC_READ_BUFF  0x7F001013
#define ACM_IOCTL_SEND_BUFF_CAN_DMA 0x7F001014

#define ACM_IOCTL_IS_IMPORT_DONE    0x7F001020
#define ACM_IOCTL_WRITE_DO_COPY     0x7F001021

/* Modem 控制命令码 */
#define ACM_MODEM_IOCTL_SET_MSC_READ_CB 0x7F001030
#define ACM_MODEM_IOCTL_MSC_WRITE_CMD   0x7F001031
#define ACM_MODEM_IOCTL_SET_REL_IND_CB  0x7F001032

//qijiwen IOCTL code for HSIC
#define ACM_IOCTL_FLOW_CONTROL  0x7F001035


/* UDI IOCTL 命令ID */
#define UDI_ACM_IOCTL_SET_READ_CB           ACM_IOCTL_SET_READ_CB
#define UDI_ACM_IOCTL_GET_READ_BUFFER_CB    ACM_IOCTL_GET_RD_BUFF
#define UDI_ACM_IOCTL_RETUR_BUFFER_CB       ACM_IOCTL_RETURN_BUFF

#define UART_IOCTL_SET_WRITE_CB      0x7F001000
#define UART_IOCTL_SET_READ_CB       0x7F001001
#define UART_IOCTL_SET_EVT_CB        0x7F001002
#define UART_IOCTL_SET_FREE_CB       0x7F001003
#define UART_IOCTL_WRITE_ASYNC       0x7F001010
#define UART_IOCTL_GET_RD_BUFF       0x7F001011
#define UART_IOCTL_RETURN_BUFF       0x7F001012
#define UART_IOCTL_RELLOC_READ_BUFF  0x7F001013
#define UART_IOCTL_SEND_BUFF_CAN_DMA 0x7F001014
#define UART_IOCTL_SET_WRITE_TIMEOUT 0x7F001015
#define UART_IOCTL_SET_BAUD           0X7F001016
#define UART_IOCTL_SET_WLEN           0X7F001017
#define UART_IOCTL_SET_STP2           0X7F001018
#define UART_IOCTL_SET_EPS            0X7F001019
#define UART_IOCTL_IS_IMPORT_DONE    0x7F001020
#define UDI_UART_IOCTL_SET_READ_CB           UART_IOCTL_SET_READ_CB
#define UDI_UART_IOCTL_GET_READ_BUFFER_CB    UART_IOCTL_GET_RD_BUFF
#define UDI_UART_IOCTL_RETUR_BUFFER_CB       UART_IOCTL_RETURN_BUFF
#define UART_IOCTL_SET_MSC_READ_CB 0x7F001021
#define UART_IOCTL_MSC_WRITE_CMD 0x7F001022
#define UART_IOCTL_SWITCH_MODE_CB 0x7F001023
#define UART_IOCTL_SET_FLOW_CONTROL 0x7F001024
#define UART_IOCTL_SET_AC_SHELL 0x7F001025

/* UART UDI 错误码*/
#define UART_UDI_ERROR_BASE               0x80000000

#define UART_ERR_MODULE_NOT_INITED      ((BSP_S32)(BSP_DEF_ERR(BSP_MODU_UART, (BSP_S32) BSP_ERR_MODULE_NOT_INITED) | UART_UDI_ERROR_BASE))
#define UART_ERR_NULL_PTR      ((BSP_S32)(BSP_DEF_ERR(BSP_MODU_UART, (BSP_S32) BSP_ERR_NULL_PTR) | UART_UDI_ERROR_BASE))
#define UART_ERR_INVALID_PARA      ((BSP_S32)(BSP_DEF_ERR(BSP_MODU_UART, (BSP_S32) BSP_ERR_INVALID_PARA) | UART_UDI_ERROR_BASE))
#define UART_ERR_RETRY_TIMEOUT      ((BSP_S32)(BSP_DEF_ERR(BSP_MODU_UART, (BSP_S32) BSP_ERR_RETRY_TIMEOUT) | UART_UDI_ERROR_BASE))
#define UART_ERR_BUF_ALLOC_FAILED      ((BSP_S32)(BSP_DEF_ERR(BSP_MODU_UART, (BSP_S32) BSP_ERR_BUF_ALLOC_FAILED) | UART_UDI_ERROR_BASE))
#define UART_ERR_BUF_FREE_FAILED      ((BSP_S32)(BSP_DEF_ERR(BSP_MODU_UART, (BSP_S32) BSP_ERR_BUF_FREE_FAILED) | UART_UDI_ERROR_BASE))

/* UART数据位长*/
typedef enum
{
    WLEN_5_BITS,
    WLEN_6_BITS,
    WLEN_7_BITS,
    WLEN_8_BITS,
    WLEN_MAX,
}UART_WLEN_ENUM;

/* UART停止位长*/
typedef enum
{
    STP2_OFF,
    STP2_ON,
    STP2_MAX,
}uart_stp2_enum;

/* UART校验方式*/
typedef enum
{
    PARITY_NO_CHECK,
    PARITY_CHECK_ODD,
    PARITY_CHECK_EVEN,
    PARITY_CHECK_MARK,        /*校验位始终为1*/
    PARITY_CHECK_SPACE,       /*校验位始终为0*/
    PARITY_CHECK_MAX,
}uart_parity_enum;

/* 异步数据收发结构 */
typedef struct tagUART_WR_ASYNC_INFO
{
    char* pBuffer;
    unsigned int u32Size;
    void* pDrvPriv;
}uart_wr_async_info;
/* 读buffer配置信息 */
typedef struct tagUART_READ_BUFF_INFO
{
    unsigned int u32BuffSize;
    unsigned int u32BuffNum;
}uart_read_buff_info;

	extern unsigned long free_mem_size_get(void);
#define FREE_MEM_SIZE_GET() free_mem_size_get()


/* ACM设备事件类型 */
typedef enum tagACM_EVT_E
{
    ACM_EVT_DEV_SUSPEND = 0,        /* 设备不可以进行读写(主要用于事件回调函数的状态) */
    ACM_EVT_DEV_READY = 1,          /* 设备可以进行读写(主要用于事件回调函数的状态) */
    ACM_EVT_DEV_BOTTOM
}ACM_EVT_E;

/* parameters for HSIC ACM flow control command */
typedef enum tagACM_IOCTL_FLOW_CONTROL_E
{
    ACM_IOCTL_FLOW_CONTROL_DISABLE = 0,      /* resume receiving data from ACM port */
    ACM_IOCTL_FLOW_CONTROL_ENABLE      /* stop receiving data from ACM port */
}ACM_IOCTL_FLOW_CONTROL_E;


/* 异步数据收发结构 */
typedef struct tagACM_WR_ASYNC_INFO
{
    char* pVirAddr;
    char* pPhyAddr;
    unsigned int u32Size;
    void* pDrvPriv;
}ACM_WR_ASYNC_INFO;


/* 读buffer信息 */
typedef struct tagACM_READ_BUFF_INFO
{
    unsigned int u32BuffSize;
    unsigned int u32BuffNum;
}ACM_READ_BUFF_INFO;

/* MODEM 管脚信号值定义 */
#define SIGNALNOCH 0
#define SIGNALCH 1
#define RECV_ENABLE 1
#define RECV_DISABLE 0
#define SEND_ENABLE 1
#define SEND_DISABLE 0
#define HIGHLEVEL 1
#define LOWLEVEL 0


/* MODEM 管角信号结构体定义*/
typedef struct tagMODEM_MSC_STRU
{
    BSP_U32 OP_Dtr     :    1;          /*DTR CHANGE FLAG*/
    BSP_U32 OP_Dsr     :    1;          /*DSR CHANGE FLAG*/
    BSP_U32 OP_Cts     :    1;          /*CTSCHANGE FLAG*/
    BSP_U32 OP_Rts     :    1;          /*RTS CHANGE FLAG*/
    BSP_U32 OP_Ri      :    1;          /*RI CHANGE FLAG*/
    BSP_U32 OP_Dcd     :    1;          /*DCD CHANGE FLAG*/
    BSP_U32 OP_Fc      :    1;          /*FC CHANGE FLAG*/
    BSP_U32 OP_Brk     :    1;          /*BRK CHANGE FLAG*/
    BSP_U32 OP_Spare   :    24;         /*reserve*/
    BSP_U8   ucDtr;                     /*DTR  VALUE*/
    BSP_U8   ucDsr;                     /*DSR  VALUE*/
    BSP_U8   ucCts;                     /*DTS VALUE*/
    BSP_U8   ucRts;                     /*RTS  VALUE*/
    BSP_U8   ucRi;                      /*RI VALUE*/
    BSP_U8   ucDcd;                     /*DCD  VALUE*/
    BSP_U8   ucFc;                      /*FC  VALUE*/
    BSP_U8   ucBrk;                     /*BRK  VALUE*/
    BSP_U8   ucBrkLen;                  /*BRKLEN VALUE*/
} MODEM_MSC_STRU, *PMODEM_MSC_STRU,AT_DCE_MSC_STRU;


typedef BSP_VOID (*ACM_WRITE_DONE_CB_T)(char* pDoneBuff, int s32DoneSize);
typedef BSP_VOID (*ACM_READ_DONE_CB_T)(BSP_VOID);
typedef BSP_VOID (*ACM_EVENT_CB_T)(ACM_EVT_E enEvt);
typedef BSP_VOID (*ACM_FREE_CB_T)(char* pBuff);
typedef BSP_VOID (*ACM_MODEM_MSC_READ_CB_T)(MODEM_MSC_STRU* pModemMsc);
typedef BSP_VOID (*ACM_MODEM_REL_IND_CB_T)(BSP_BOOL bEnable);
/*************************ACM END*****************************/

typedef void (*uart_free_cb_t)(char* pBuff);
typedef void (*uart_read_cb_t)(void);
typedef void (*uart_msc_read_cb_t)(MODEM_MSC_STRU* pModemMsc);
typedef void (*uart_switch_mode_cb_t)(void);


/*************************INT BEGIN*****************************/
/*****************************************************************************
* 函 数 名  : BSP_INT_Enable
*
* 功能描述  : 使能某个中断
*
* 输入参数  : INT32 ulLvl 要使能的中断号，取值范围0～40
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   zhanghailun  creat
*****************************************************************************/
BSP_S32 BSP_INT_Enable ( BSP_S32 s32Lvl);

#define DRV_VICINT_ENABLE(ulLvl)    BSP_INT_Enable(ulLvl)

/*****************************************************************************
 * 函 数 名  : BSP_INT_Disable
 *
 * 功能描述  : 去使能某个中断
 *
 * 输入参数  : INT32 ulLvl 要使能的中断号，取值范围0～40
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Disable ( BSP_S32 s32Lvl);

#define  DRV_VICINT_DISABLE(ulLvl)    BSP_INT_Disable(ulLvl)

/*****************************************************************************
 * 函 数 名  : BSP_INT_Lock
 *
 * 功能描述  : 屏蔽所有中断
 *
 * 输入参数  : 无
 *
 * 输出参数  : 无
 *
 * 返 回 值  : 中断锁号
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Lock (BSP_VOID);

/*****************************************************************************
 * 函 数 名  : BSP_INT_UnLock
 *
 * 功能描述  : 打开中断
 *
 * 输入参数  : INT32 ulLocKKey  中断锁号，前一次BSP_INT_Lock的返回值
 *
 * 输出参数  : 无
 *
 * 返 回 值  : 中断锁号
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_VOID BSP_INT_UnLock ( BSP_S32 s32LocKKey);

/*************************INT END*****************************/

/*************************WDT BEGIN*****************************/
#define BSP_HW_WATCHDOG       0
#define BSP_SW_WATCHDOG       1


typedef enum tagWDT_TIMEOUT_E
{
    WDT_TIMEOUT_1   = 0,        /*0xFFFF000/WDT_CLK_FREQ,  about 3657ms*/      /*WDT_CLK_FREQ = ARM_FREQ/6 = 70M*/
    WDT_TIMEOUT_2,              /*0x1FFFE000/WDT_CLK_FREQ, about 7314ms*/
    WDT_TIMEOUT_3,              /*0x3FFFC000/WDT_CLK_FREQ, about 14628ms*/
    WDT_TIMEOUT_4,              /*0x7FFF8000/WDT_CLK_FREQ, about 29257ms*/
    WDT_TIMEOUT_BUTT
}WDT_TIMEOUT_E;


/*看门狗硬件信息数据结构*/
typedef struct tagWDT_ID_INFO_S
{
    BSP_U32 u32PartNum;
    BSP_U32 u32DesignerID;
    BSP_U32 u32Version;
    BSP_U32 u32Config;
    BSP_U32 u32PrimeCellID;
}WDT_ID_INFO_S;

typedef enum
{
    SIM_CARD_OUT = 0,
    SIM_CARD_IN  = 1
} SCI_DETECT_STATE;
typedef BSP_VOID(*wdt_timeout_cb)();


typedef void (*OMSCIFUNCPTR)(unsigned int ulVal);
#if defined (VERSION_V3R2)
void BSP_SCIFuncRegister(OMSCIFUNCPTR omSciFuncPtr);
#define DRV_USIMMSCI_FUNC_REGISTER(omSciFuncPtr)    BSP_SCIFuncRegister(omSciFuncPtr)
#else
#define DRV_USIMMSCI_FUNC_REGISTER(omSciFuncPtr)    DRV_OK
#endif

/*****************************************************************************
* 函 数 名  : BSP_WDT_Init
*
* 功能描述  : 初始化定制看门狗，设置超时时间，挂接中断服务函数
*
* 输入参数  : wdtId         看门狗ID,芯片提供1个看门狗，ID可以为0
*             wdtTimeOuts   看门狗超时时间，单位秒,时间最大不能超过51秒
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_Init(BSP_U8 wdtId , BSP_U32 wdtTimeOuts);

/*****************************************************************************
* 函 数 名  : BSP_WDT_EnableWdt
*
* 功能描述  : 开启看门狗
*
* 输入参数  : wdtId  看门狗ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_Enable(BSP_U8 wdtId);

/*****************************************************************************
* 函 数 名  : BSP_WDT_Disable
*
* 功能描述  : 关闭看门狗
*
* 输入参数  : wdtId  看门狗ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_Disable(BSP_U8 wdtId);

/*****************************************************************************
* 函 数 名  : BSP_WDT_SetTime
*
* 功能描述  : 设置看门狗的超时时间
*
* 输入参数  : BSP_U8 u8WdtId:看门狗ID，芯片提供二个看门狗，但只使用一个，ID为0
*             WDT_TIMEOUT_E enTimeOut:看门狗超时间隔
* 输出参数  : 无
*
* 返 回 值  : OK:    操作成功
*             ERROR: 操作失败
* 其它说明  :
*
*****************************************************************************/
BSP_S32 BSP_WDT_SetTimeOut(BSP_U8 u8WdtId, WDT_TIMEOUT_E enTimeOut);

/*****************************************************************************
* 函 数 名  : BSP_WDT_HardwareFeed
*
* 功能描述  : 重置看门狗计数寄存器（喂狗）
*
* 输入参数  : wdtId  看门狗ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_HardwareFeed(BSP_U8 wdtId);

/*****************************************************************************
* 函 数 名  : BSP_WDT_HardwareFeed_Force
*
* 功能描述  : 重置看门狗计数寄存器（喂狗）
*
* 输入参数  : wdtId  看门狗ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_HardwareFeed_Force(BSP_U8 u8WdtId);

/*****************************************************************************
* 函 数 名  : BSP_WDT_Switch
*
* 功能描述  : 切换喂狗方式
*
* 输入参数  : wdtId    看门狗ID
*           feedMode 喂狗方式(0：驱动喂狗；1：软件喂狗)
*
* 输出参数  : 无
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_Switch(BSP_U8 wdtId, BSP_U8 feedMode);

/*****************************************************************************
* 函 数 名  : BSP_WDT_IntInstall
*
* 功能描述  : 在看门狗中断服务程序中注册超时处理函数
*
* 输入参数  : wdtId  看门狗ID
*		    p      注册的函数指针
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_IntInstall(BSP_U8 wdtId, BSP_VOID * p);

/*****************************************************************************
* 函 数 名  : BSP_WDT_IntInstall
*
* 功能描述  : 在看门狗中断服务程序中卸载超时处理函数
*
* 输入参数  : wdtId  看门狗ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_IntUnInstall(BSP_U8 wdtId);

/*****************************************************************************
* 函 数 名  : BSP_WDT_TimeoutGet
*
* 功能描述  : 获得看门狗剩余超时时间
*
* 输入参数  : wdtId  看门狗ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_TimeoutGet(BSP_U8 u8WdtId, BSP_U32 *u32TimeOut);

/*****************************************************************************
* 函 数 名  : BSP_WDT_reboot
*
* 功能描述  : 看门狗复位系统
*
* 输入参数  : BSP_VOID
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 使用看门狗进行系统复位
*
*****************************************************************************/
BSP_VOID BSP_WDT_reboot(BSP_VOID);

/*************************WDT END*****************************/
/*************************SD BEGIN************************************/
#define WRFlAG     1
#define RDFlAG     0

typedef enum
{
    TFUP_FAIL = 0,  /*打叉*/
    TFUP_OK         /*打勾*/
}TFUP_DISPLAY_TYPE;

enum SD_MMC_OPRT_ENUM
{
    SD_MMC_OPRT_FORMAT = 0,
    SD_MMC_OPRT_ERASE_ALL,
    SD_MMC_OPRT_ERASE_SPEC_ADDR,
    SD_MMC_OPRT_WRITE,
    SD_MMC_OPRT_READ,
    SD_MMC_OPRT_BUTT
};

typedef unsigned int SD_MMC_OPRT_ENUM_UINT32;


/*SD卡设备类型*/
typedef struct _sd_dev_type_str
{
    unsigned long   devNameLen;
    char            devName[16];
}SD_DEV_TYPE_STR;

/*****************************************************************************
 函 数 名  : BSP_SDMMC_ATProcess
 功能描述  : at^sd,SD卡操作，写，擦除，格式化操作
 输入参数  : 操作类型 ulOp:
            0  格式化SD卡
            1  擦除整个SD卡内容；
            2  用于指定地址内容的擦除操作，指定擦除的内容长度为512字节。擦除后的地址中写全1
            3  写数据到SD卡的指定地址中，需要带第二个和第三个参数
            4  读取ulAddr指定的地址(ulAddr*512)的512个字节的内容到pucBuffer中

            ulAddr < address >  地址，以512BYTE为一个单位，用数字n表示

            ulData
             < data >            数据内容，表示512BYTE的内容，每个字节的内容均相同。
             0       字节内容为0x00
             1       字节内容为0x55
             2       字节内容为0xAA
             3       字节内容为0xFF

 输出参数  : pulErr
 返 回 值  : 0 ：OK  非 0 ：Error

            具体的错误值填充在*pulErr中
            0 表示SD卡不在位
            1 表示SD卡初始化失败
            2 表示<opr>参数非法，对应操作不支持(该错误由AT使用,不需要底软使用)
            3 表示<address>地址非法，超过SD卡本身容量
            4 其他未知错误
*****************************************************************************/
extern  unsigned long  BSP_SDMMC_ATProcess(SD_MMC_OPRT_ENUM_UINT32 ulOp,
                unsigned long ulAddr,  unsigned long ulData,unsigned char *pucBuffer,unsigned long *pulErr);
#define DRV_SDMMC_AT_PROCESS(ulOp,ulAddr,ulData,pucBuffer,pulErr)   \
                     BSP_SDMMC_ATProcess (ulOp,ulAddr,ulData,pucBuffer,pulErr)

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
/*****************************************************************************
* 函 数 名  : sd_mmc_blk_w
*
* 功能描述  :写卡接口
*
* 输入参数  : u32StartBlk 起始块
				  pu8DataBuf  写数据缓存
				  u32Len 写数据大小，必须为512字节的整数倍
* 输出参数  : NA
*
* 返 回 值  :  成功返回0
*
* 其它说明  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_blk_w(BSP_U32 u32StartBlk,BSP_U8 *pu8DataBuf,BSP_U32 u32Len);
/*****************************************************************************
* 函 数 名  : sd_mmc_blk_r
*
* 功能描述  : 读卡接口
*
* 输入参数  : u32StartBlk 起始块
				  pu8DataBuf  读数据缓存
				  u32Len  读数据大小，必须为512字节的整数倍
* 输出参数  : NA
*
* 返 回 值  :  成功返回0
*
* 其它说明  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_blk_r(BSP_U32 u32StartBlk,BSP_U8 *pu8DataBuf,BSP_U32 u32Len);
/*****************************************************************************
* 函 数 名  : sd_mmc_blk_erase
*
* 功能描述  : 卡块擦除接口
*
* 输入参数  : u32StartBlk 起始块
				  u32EndBlk  终止块
* 输出参数  : NA
*
* 返 回 值  :  成功返回0
*
* 其它说明  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_blk_erase(BSP_U32 u32StartBlk,BSP_U32 u32EndBlk);
/*****************************************************************************
* 函 数 名  : sd_mmc_get_status
*
* 功能描述  : 卡在位查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 在位；-1: 不在位
*
* 其它说明  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_get_status(BSP_VOID);
#endif

/*****************************************************************************
 函 数 名  : BSP_SDMMC_GetOprtStatus
 功能描述  : at^sd,SD卡当前操作状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
            0: 未操作或操作已经完成;
            1: 操作中
            2: 上次操作失败
*****************************************************************************/
extern unsigned long BSP_SDMMC_GetOprtStatus(void);
#define DRV_SDMMC_GET_OPRT_STATUS()    BSP_SDMMC_GetOprtStatus()

/*****************************************************************************
 函 数 名  : BSP_SDMMC_AddHook
 功能描述  : TF模块钩子函数注册。
 输入参数  : hookType：回调函数类型，
                       0：获得升级文件信息后调用的回调函数；
                       1：TF卡插入通知OM的回调函数；
                       2：TF卡拔出通知OM的回调函数；
                       3； TF卡初始化后通知U盘的回调函数。
             p：回调函数指针。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：对于同一类型回调函数，重复注册会覆盖以前设置。
*****************************************************************************/
//#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern int sdmmcAddHook (int hookType , void * p);/*y00186965 for sd_update*/
#define DRV_SDMMC_ADD_HOOK(hookType,p)    sdmmcAddHook(hookType,p)
//#else
//extern int BSP_SDMMC_AddHook(int hookType, void * p);
//#define DRV_SDMMC_ADD_HOOK(hookType,p)    BSP_SDMMC_AddHook(hookType,p)
//#endif

/*****************************************************************************
 函 数 名  : BSP_SDMMC_UsbGetStatus
 功能描述  : 返回SD卡状态
 输入参数  : 无。
 输出参数  : 无。
 返回值：   0  为可用
           非0 不可用

*****************************************************************************/
extern unsigned int BSP_SDMMC_UsbGetStatus(void);
#define DRV_SDMMC_USB_STATUS()    BSP_SDMMC_UsbGetStatus();

/*****************************************************************************
* 函 数 名  : sdmmc_ClearWholeScreen
* 功能描述  : SD卡升级NV恢复后的显示，清屏
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  :无
* 其它说明  : 无
*****************************************************************************/
extern void sdmmc_ClearWholeScreen(void);
#define DRV_SDMMC_CLEAR_WHOLE_SCREEN()    sdmmc_ClearWholeScreen()

/*****************************************************************************
* 函 数 名  : sdmmc_UpdateDisplay
* 功能描述  : SD卡升级NV恢复后的显示，显示
* 输入参数  : BOOL UpdateStatus
* 输出参数  : 无
* 返 回 值  :无
* 其它说明  : 升级成功，NV备份完成后调用该函数打勾
              升级失败后调用该函数打叉
*****************************************************************************/
extern void sdmmc_UpdateDisplay(int UpdateStatus);
#define DRV_SDMMC_UPDATE_DISPLAY(state)    sdmmc_UpdateDisplay(state)

/*****************************************************************************
* 函 数 名  : DRV_SD_GET_STATUS
*
* 功能描述  : 卡在位查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 在位；-1: 不在位
*
* 其它说明  : NA
*
*****************************************************************************/
extern int DRV_SD_GET_STATUS(void);

/*****************************************************************************
* 函 数 名  : DRV_SD_GET_CAPACITY
*
* 功能描述  : 卡容量查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 失败；>0: 卡容量
*
* 其它说明  : NA
*
*****************************************************************************/
extern int DRV_SD_GET_CAPACITY(void);

/*****************************************************************************
* 函 数 名  : DRV_SD_TRANSFER
*
* 功能描述  : 数据传输
*
* 输入参数  : struct scatterlist *sg    待传输数据结构体指针
                            unsigned dev_addr   待写入的SD block 地址
                            unsigned blocks    待写入的block个数
                            unsigned blksz      每个block的大小，单位字节
                            int wrflags    读写标志位，写:WRFlAG ; 读:RDFlAG
* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
extern int DRV_SD_TRANSFER(struct scatterlist *sg, unsigned dev_addr,unsigned blocks, unsigned blksz, int wrflags);
#endif
/*****************************************************************************
* 函 数 名  : DRV_SD_SG_INIT_TABLE
*
* 功能描述  : SD多块数据传输sg list初始化
*
* 输入参数  : const void *buf		待操作的buffer地址
				  unsigned int buflen	待操作的buffer大小，小于32K, 大小为512B的整数倍
				  					大于32K, 大小为32KB的整数倍，最大buffer为128K
* 输出参数  : NA
*
* 返 回 值  :  0 : 成功;  其它:失败
* 其它说明  : NA
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
extern int DRV_SD_SG_INIT_TABLE(const void *buf,unsigned int buflen);
#endif
/*****************************************************************************
* 函 数 名  : DRV_SD_MULTI_TRANSFER
*
* 功能描述  : SD多块数据传输
*
* 输入参数  : unsigned dev_addr	待写入的SD block地址
				  unsigned blocks		待写入的block 个数
				  unsigned blksz		每个block 的大小，单位字节
				  int write			读写标志位，写:1;	读:0
* 输出参数  : NA
*
* 返 回 值  :  0 : 成功;  其它:失败
* 其它说明  : NA
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
extern int DRV_SD_MULTI_TRANSFER(unsigned dev_addr,unsigned blocks,unsigned blksz,int write);
#endif
/*************************SD END**************************************/

/*************************SD BEGIN*****************************/

/*****************************************************************************
* 函 数 名  : SDIO_check_card
*
* 功能描述  :
*
* 输入参数  : BSP_S32 s32CardNo
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 SDIO_check_card(BSP_S32 s32CardNo);

/*****************************************************************************
* 函 数 名  : read_write_blkdata
*
* 功能描述  :

* 输入参数  :

* 输出参数  :
* 返 回 值  :
*
* 修改记录  :

*****************************************************************************/
BSP_S32  read_write_blkdata(BSP_S32 s32CardNo, BSP_U32 u32Addr,
                            BSP_U32 u32DataSize,
                            BSP_U8  *pu8DataBuff,
                            BSP_U8  *pu8CmdRespBuff,
                            BSP_U8  *pu8ErrRespBuff,
                            BSP_VOID (*callback)(BSP_U32 u32ErrData),
                            BSP_S32 s32NoOfRetries,
                            BSP_S32 s32Flags);
/*************************SD END*****************************/

/*************************DLOAD BEGIN*****************************/
/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/
/* 错误码 */
#define DLOAD_OK                    BSP_OK
#define DLOAD_ERROR                 BSP_ERROR
#define DLOAD_ERR_NOT_INITED        BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_MODULE_NOT_INITED)
#define DLOAD_ERR_NULL_PTR          BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_NULL_PTR)
#define DLOAD_ERR_INVALID_PARA      BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_INVALID_PARA)
#define DLOAD_ERR_ALLOC_FAILED      BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_BUF_ALLOC_FAILED)
#define DLOAD_ERR_FREE_FAILED       BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_BUF_FREE_FAILED)
#define DLOAD_ERR_RETRY_TIMEOUT     BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_RETRY_TIMEOUT)

#define DLOAD_ERR_NO_BACKUP         BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 1)  /* 无备份版本 */
#define DLOAD_ERR_OPEN_FAILED       BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 2)  /* 虚拟串口打开失败 */
#define DLOAD_ERR_SEM_CREAT         BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 3)  /* 信号量创建失败 */
#define DLOAD_ERR_ABORT             BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 4)  /* 升级终止 */
#define DLOAD_ERR_MODE_MISMATCH     BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 5)  /* 工作模式不匹配 */
#define DLOAD_ERR_INVALID_ATSTRING  BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 6)  /* AT字符串非法 */



/*--------------------------------------------------------------*
 * 枚举定义                                                     *
 *--------------------------------------------------------------*/

/* 下载模式枚举 */
typedef enum tagDLOAD_MODE_E
{
    DLOAD_MODE_DOWNLOAD = 0,
    DLOAD_MODE_NORMAL,
    DLOAD_MODE_DATA,
    DLOAD_MODE_MAX
}DLOAD_MODE_E;
typedef enum
{
    NORMAL_DLOAD = 0,   /*正常升级模式*/
    FORCE_DLOAD = 1    /*强制升级模式*/
}DLOAD_TYPE;

extern BSP_S32 BSP_DLOAD_GetDloadInfo(unsigned char atCmdBuf[], unsigned int dloadType);
#define DRV_GET_DLOAD_INFO(atCmdBuf, dloadType)    BSP_DLOAD_GetDloadInfo(atCmdBuf, dloadType)
/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetAuthorityId
 功能描述  : 获取下载鉴权协议Id
 输入参数  : unsigned char *buf
                          int len
 输出参数  : 无
 返 回 值  : 0:  操作成功；
                      -1：操作失败。
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_GetAuthorityId(unsigned char *buf, BSP_S32 len);
#define DRV_GET_AUTHORITY_ID(buf,len) BSP_DLOAD_GetAuthorityId(buf,len)

/*****************************************************************************
 函 数 名  : drvShutdown
 功能描述  : 单板关机
 输入参数  : 无
 输出参数  : eReason：        关机原因
 返 回 值  : 无
*****************************************************************************/
typedef enum DRV_SHUTDOWN_REASON_tag_s
{
    E5_DRV_SHUTDOWN_LOW_BATTERY,           /* 电池电量低                 */
    E5_DRV_SHUTDOWN_BATTERY_ERROR,         /* 电池异常                   */
    E5_DRV_SHUTDOWN_POWER_KEY,             /* 长按 Power 键关机          */
    E5_DRV_SHUTDOWN_TEMPERATURE_PROTECT,   /* 过温保护关机               */
    E5_DRV_SHUTDOWN_RESET,                 /* 系统软复位                 */
    E5_DRV_SHUTDOWN_CHARGE_REMOVE,         /* 关机充电模式下，拔除充电器 */
    E5_DRV_SHUTDOWN_UPDATE,                /* 关机并进入升级模式         */
    E5_DRV_SHUTDOWN_BUTT
}E5_DRV_SHUTDOWN_REASON_ENUM;

typedef enum DRV_SHUTDOWN_REASON_tag
{
    DRV_SHUTDOWN_LOW_BATTERY,           /* 电池电量低           */
    DRV_SHUTDOWN_BATTERY_ERROR,         /* 电池异常            */
    DRV_SHUTDOWN_POWER_KEY,             /* 长按 Power 键关机    */
    DRV_SHUTDOWN_TEMPERATURE_PROTECT,   /* 过温保护关机         */
    DRV_SHUTDOWN_RESET,                 /* 系统软复位           */
    DRV_SHUTDOWN_CHARGE_REMOVE,         /* 关机充电模式下，拔除充电器 */
    DRV_SHUTDOWN_UPDATE,                /* 关机并进入升级模式 */
    DRV_SHUTDOWN_BUTT
}DRV_SHUTDOWN_REASON_ENUM;

void drvShutdown( DRV_SHUTDOWN_REASON_ENUM eReason );
#define DRV_SHUT_DOWN(eReason) drvShutdown(eReason)

/*****************************************************************************
 函 数 名  : BSP_DFS_GetCurCpuLoad
 功能描述  : 查询当前CPU
 输入参数  : pu32AcpuLoad ACPUload指针
             pu32CcpuLoad CCPUload指针
 输出参数  : pu32AcpuLoad ACPUload指针
             pu32CcpuLoad CCPUload指针
 返 回 值  : 0:  操作成功；
            -1：操作失败。
*****************************************************************************/
extern BSP_U32 BSP_DFS_GetCurCpuLoad(BSP_U32 *pu32AcpuLoad,BSP_U32 *pu32CcpuLoad);
#define DRV_GET_CUR_CPU_LOAD(pu32AcpuLoad,pu32CcpuLoad) BSP_DFS_GetCurCpuLoad(pu32AcpuLoad,pu32CcpuLoad)
/*************************************************
 函 数 名   : BSP_HKADC_BatVoltGet
 功能描述   : 返回当前电池电压值
 输入参数   : pslData : 电池电压值
 输出参数   : pslData : 电池电压值
 返 回 值   :0:获取成功
            -1:获取失败
*************************************************/
extern BSP_S32 BSP_HKADC_BatVoltGet(BSP_S32 *ps32Data);
#define DRV_HKADC_BAT_VOLT_GET(ps32Data)   BSP_HKADC_BatVoltGet(ps32Data)

/*************************************************
 函 数 名   : BSP_MNTN_ProductTypeGet
 功能描述   : 返回当前产品类型
 输入参数   : 无
 输出参数   : 无
 返 回 值   :0:STICK
             1:MOD
             2:E5
             3:CPE
*************************************************/
extern BSP_U32 BSP_MNTN_ProductTypeGet(void);
#define DRV_PRODUCT_TYPE_GET()   BSP_MNTN_ProductTypeGet()

/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetAuthorityVer
 功能描述  : Get Authority version
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_GetAuthorityVer(BSP_U8 *str, BSP_S32 len);
#define DRV_GET_AUTHORITY_VERSION(str, len )   BSP_DLOAD_GetAuthorityVer(str, len )



/*--------------------------------------------------------------*
 * 函数指针类型定义                                             *
 *--------------------------------------------------------------*/
typedef BSP_U32 (*BSP_DLOAD_AtCallBack)( BSP_VOID* pDataIn, BSP_U32 ulLen);
typedef BSP_U32 (*BSP_DLOAD_NVGetInfoCb)( BSP_VOID* pDataOut, BSP_U32 ulLen);
typedef BSP_U32 (*BSP_DLOAD_NVSetInfoCb)( BSP_VOID* pDataIn, BSP_U32 ulLen);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetDloadVer
*
* 功能描述  : 查询下载协议版本号。该信息BSP固定写为2.0。
*
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
*
* 返 回 值  : DLOAD_ERROR   :输入参数非法
*             其它          :返回字符串长度
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetDloadVer(BSP_CHAR *str, BSP_U32 len);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetISOVer
*
* 功能描述  : 获取后台版本号字符串
*
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
*
* 返 回 值  : DLOAD_ERROR   :输入参数非法
*             其它          :返回字符串长度
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetISOVer(BSP_CHAR *str, BSP_U32 len);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetProductId
*
* 功能描述  : 获取产品名称字符串
*
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
*
* 返 回 值  : DLOAD_ERR_INVALID_PARA    :输入参数非法
*             DLOAD_ERROR               :失败
*             其它                      :返回字符串长度
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetProductId(BSP_CHAR *str, BSP_U32 len);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetDloadType
*
* 功能描述  : 获取下载类型
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : 0 :正常模式
*             1 :强制模式
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetDloadType(BSP_VOID);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetCurMode
*
* 功能描述  : 设置当前工作模式
*
* 输入参数  : BSP_VOID
* 输出参数  : DLOAD_MODE_NORMAL     :正常模式
*             DLOAD_MODE_DATA       :数据模式
*             DLOAD_MODE_DOWNLOAD   :下载模式
*
* 返 回 值  : 无
*
* 其它说明  : AT模块调用
*             正常模式支持的有:
*                 BSP_DLOAD_GetCurMode(DLOAD_MODE_DOWNLOAD)
*             下载模式支持的有:
*                 BSP_DLOAD_GetCurMode(DLOAD_MODE_DATA)
*
*****************************************************************************/
BSP_VOID BSP_DLOAD_SetCurMode(DLOAD_MODE_E eDloadMode);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetCurMode
*
* 功能描述  : 获取当前工作模式
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : DLOAD_MODE_NORMAL     :正常模式
*             DLOAD_MODE_DATA       :数据模式
*             DLOAD_MODE_DOWNLOAD   :下载模式
*
* 其它说明  : 无
*
*****************************************************************************/
DLOAD_MODE_E BSP_DLOAD_GetCurMode(BSP_VOID);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_SetSoftLoad
*
* 功能描述  : 设置启动标志
*
* 输入参数  : BSP_BOOL bSoftLoad  :
*             BSP_FALSE :从vxWorks启动
*             BSP_TRUE  :从bootrom启动
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_VOID BSP_DLOAD_SetSoftLoad (BSP_BOOL bSoftLoad);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetSoftLoad
*
* 功能描述  : 获取启动标志
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : BSP_TRUE  :从bootrom启动
*             BSP_FALSE :从vxWorks启动
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_BOOL BSP_DLOAD_GetSoftLoad (BSP_VOID);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_SetCdromMarker
*
* 功能描述  : 设置设备形态标志
*
* 输入参数  : BSP_BOOL bCdromMarker  :
*             BSP_TRUE  :设置正常设备形态
*             BSP_FALSE :设置bootrom设备形态
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 当升级后台文件时，设置为上报正常设备形态，不恢复NV；
*             当升级前台文件时，设置为上报bootrom设备形态，恢复NV；
*
*****************************************************************************/
BSP_VOID BSP_DLOAD_SetCdromMarker(BSP_BOOL bCdromMarker);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetCdromMarker
*
* 功能描述  : 获取设备形态标志
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : BSP_TRUE  :上报正常设备形态
*             BSP_FALSE :上报bootrom设备形态
*
* 其它说明  : 当升级后台文件时，设置为上报正常设备形态，不恢复NV；
*             当升级前台文件时，设置为上报bootrom设备形态，恢复NV；
*
*****************************************************************************/
BSP_BOOL BSP_DLOAD_GetCdromMarker (BSP_VOID);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_SendData
*
* 功能描述  : 通过虚拟串口向PC端发送数据
*
* 输入参数  : pBuf      :数据缓冲区
*             u32Len    :数据缓冲区长度
* 输出参数  : pBuf      :数据缓冲区
*
* 返 回 值  : 无
*
* 其它说明  : pBuf必须保证cache line(32字节)对齐
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_SendData(BSP_CHAR *pBuf, BSP_U32 u32Len);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_AtProcReg
*
* 功能描述  : 注册AT处理函数
*
* 输入参数  : pFun
* 输出参数  : 无
*
* 返 回 值  : DLOAD_OK:成功
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_AtProcReg (BSP_DLOAD_AtCallBack pFun);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetSoftwareVer
*
* 功能描述  : 获取软件版本号
*
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
*
* 返 回 值  : DLOAD_ERROR   :输入参数非法
*             其它          :返回字符串长度
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetSoftwareVer(BSP_CHAR *str, BSP_U32 len);

/*************************DLOAD END*****************************/


/*************************GPIO START*****************************/

/* 中断触发类型枚举*/
typedef enum tagGPIO_INT_TRIG_E
{
    GPIO_HIGH_LEVEL          	=  1,/* 高电平触发*/
    GPIO_LOW_LEVEL           	=  2,/* 低电平触发*/
    GPIO_BOTH_EDGE           	=  3,/* 双沿触发*/
    GPIO_RISING_EDGE        	=  4,/* 上升沿触发*/
    GPIO_DECENDING_EDGE    	    =  5 /* 下降沿触发*/
} GPIO_INT_TRIG_E;

#ifdef BSP_CPE
typedef enum tagCPE_SIGNAL_INTENSITY_E
{
	SIGNALNULL,
    SIGNALLOW,
    SIGNALMIDDLE,
    SIGNALHIGH,
    SIGNALMAX
}CPE_SIGNAL_INTENSITY_E;

typedef enum tagCPE_SIGNAL_STATUS_E
{
     SIGNAL_STATUS_ABNORMAL,    //信号异常
     SIGNAL_STATUS_NORMAL,      //信号正常
     SIGNAL_STATUS_MAX
}CPE_SIGNAL_STATUS_E;

typedef enum tagGPIO_CTRL_E
{
    GPIO_ON,
    GPIO_OFF,
    GPIO_MAX
}GPIO_CTRL_E;

#endif


/* PV500上有2个GPIO */
typedef enum tagGPIO_GPIO_NUM_E
{
    GPIO_DEF_GPIO0          	=  0,
    GPIO_DEF_GPIO1           	=  1,
    GPIO_DEF_MAX
} GPIO_GPIO_NUM_E;

/* 查询模式 */
typedef enum tagGPIO_GPIO_GETMODE_NUM_E
{
    LINE_STATE          	=  0,
    FORCE_LOAD_CTRL       =  1
} GPIO_GETMODE_NUM_E;


/*****************************************************************************
 函 数 名  : USB_NDIS_GetMaxTxNum
 功能描述  : 提供给协议栈获取下行一次发送最大IP包个数
 输入参数  :
 输出参数  : unsigned long
 返 回 值  : IP包个数
*****************************************************************************/
extern unsigned long USB_NDIS_GetMaxTxNum(void);
#define DRV_USB_NDIS_GETMAXTXNUM()     USB_NDIS_GetMaxTxNum()
/*****************************************************************************
* 函 数 名  : BSP_GPIO_Init
*
* 功能描述  : GPIO初始化接口，挂接中断服务程序
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_VOID  BSP_GPIO_Init (BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_DrcSet
*
* 功能描述  : 设置GPIO引脚的方向
*
* 输入参数  : BSP_U32 u32Mask        指定待设置的GPIO位
*             BSP_U32 u32Directions  待设置的方向值,0：输入；1：输出
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32  BSP_GPIO_DrcSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, BSP_U32 u32Directions);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_DrcGet
*
* 功能描述  : 查询GPIO 引脚的方向
*
* 输入参数  : 无
* 输出参数  : BSP_U32 * pu32Directions  保存返回的方向值
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_DrcGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 * pu32Directions);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_ValSet
*
* 功能描述  : 设置GPIO 引脚的电平值
*
* 输入参数  : BSP_U32 u32Mask  指定待设置的GPIO位
*             BSP_U32 u32Data  待设置的电平值
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_ValSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, BSP_U32 u32Data);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_ValGet
*
* 功能描述  : 查询GPIO引脚的电平值
*
* 输入参数  : 无
* 输出参数  : BSP_U32 * pu32Data  保存返回的电平值
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_ValGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 * pu32Data);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntMaskSet
*
* 功能描述  : 屏蔽中断位
*
* 输入参数  : BSP_U32 u32Mask  指定屏蔽中断的GPIO位
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntMaskSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntUnMaskSet
*
* 功能描述  : 使能中断位
*
* 输入参数  : BSP_U32 u32Mask  指定使能中断的GPIO位
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntUnMaskSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntStateGet
*
* 功能描述  : 查询中断状态位
*
* 输入参数  : BSP_U32 *pu32Stat
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntStateGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 *pu32Stat);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_RawIntStateGet
*
* 功能描述  : 查询原始中断状态位
*
* 输入参数  : 无
* 输出参数  : BSP_U32 *pu32RawStat  保存返回的原始中断状态
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_RawIntStateGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 *pu32RawStat);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntStateClear
*
* 功能描述  : 清除中断状态位，置1表示对应中断位清除中断
*
* 输入参数  : BSP_U32 u32Mask  指定待清除的GPIO位
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntStateClear(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntTriggerSet
*
* 功能描述  : 设置中断触发方式，上升沿、下降沿、双边沿、高电平、低电平
*
* 输入参数  : BSP_U32 u32Mask             指定待设置的GPIO位
*             GPIO_INT_TRIG enTrigMode  中断触发模式
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntTriggerSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, GPIO_INT_TRIG_E enTrigMode);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntInstall
*
* 功能描述  : 其他驱动模块注册GPIO中断处理程序的接口
*
* 输入参数  : BSP_U32 u32Mask  指定GPIO位ID
*             BSP_VOID* routine  待注册的回调函数指针
*             BSP_S32 para       回调函数参数
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntInstall(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, BSP_VOID* routine,BSP_S32 para);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntUnInstall
*
* 功能描述  : 其他驱动模块卸载GPIO中断处理程序的接口
*
* 输入参数  : BSP_U32 u32Mask  指定GPIO位ID
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntUnInstall (GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);
/*************************GPIO END *****************************/



/*************************TIMER BEGIN*****************************/
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Connect
*
* 功能描述  : This routine specifies the interrupt service routine to be called
*             at each clock interrupt.  It does not enable usr clock interrupts.
*
* 输入参数  : FUNCPTR routine   routine to be called at each clock interrupt
              BSP_S32 arg	        argument with which to call routine
              BSP_S32 s32UsrClkid      which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the routine cannot be connected to the interrupt.
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
//BSP_S32  BSP_USRCLK_Connect(FUNCPTR routine, BSP_S32 arg, BSP_S32 s32UsrClkid);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerStart
*
* 功能描述  : 开启单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
*             u32Cycles   Timer寄存器的计数值
* 输出参数  : 无
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
BSP_S32    BSP_USRCLK_TimerStart(BSP_S32 s32UsrClkid,BSP_U32 u32Cycles);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerStop
*
* 功能描述  : 关闭单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
* 输出参数  : 无
* 返 回 值  : 无.
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
BSP_S32    BSP_USRCLK_TimerStop(BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerValue
*
* 功能描述  : 开启单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
* 输出参数  : Value      当前Timer value寄存器的计数值
* 返 回 值  : OK&错误码
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
BSP_U32  BSP_USRCLK_TimerValue(BSP_S32 s32UsrClkid, BSP_U32 *pu32Value);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Disable
*
* 功能描述  : This routine disables user clock interrupts.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
BSP_S32   BSP_USRCLK_Disable (BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Enable
*
* 功能描述  : This routine enables user clock interrupts.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
BSP_S32    BSP_USRCLK_Enable (BSP_S32 s32UsrClkid);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_RateGet
*
* 功能描述  : This routine returns the interrupt rate of the system aux clock.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : BSP_S32 * pu32ClkRate  clk rate
* 返 回 值  : OK&错误码
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
BSP_S32     BSP_USRCLK_RateGet(BSP_S32 s32UsrClkId, BSP_S32 * pu32ClkRate);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_RateSet
*
* 功能描述  : This routine sets the interrupt rate of the usr clock.
*
* 输入参数  : BSP_S32 ticksPerSecond   number of clock interrupts per second
              BSP_S32 s32UsrClkid         which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
BSP_S32  BSP_USRCLK_RateSet(BSP_S32 ticksPerSecond, BSP_S32 s32UsrClkid);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerMilliSecValue
*
* 功能描述  : 获得当前Timer的毫秒值.
*
* 输入参数  : s32UsrClkid        Timer Id
* 输出参数  : BSP_U32 * pu32Value 当前Timer value寄存器的毫秒值
* 返 回 值  : OK&其他错误码
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
BSP_U32  BSP_USRCLK_TimerMilliSecValue(BSP_S32 s32UsrClkId ,BSP_U32 * pu32Value);
/*****************************************************************************
* 函 数 名  : sysClkTicksGet
*
* 功能描述  : 获得系统时钟启动以来的tick数.
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 系统时钟启动以来的tick数.
*
* 修改记录  : 2009年2月24日   liumengcun  creat

*****************************************************************************/
BSP_U32 sysClkTicksGet (BSP_VOID);

/*****************************************************************************
* 函 数 名  : sysAuxClkTicksGet
*
* 功能描述  : 获得系统辅助时钟启动以来的tick数.
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 系统辅助时钟启动以来的tick数.
*
* 修改记录  : 2009年2月24日   liumengcun  creat

*****************************************************************************/
BSP_U32 sysAuxClkTicksGet (BSP_VOID);
/*****************************************************************************
* 函 数 名  : sysTimestampRateSet
*
* 功能描述  : This routine sets the interrupt rate of the timestamp clock.  It does
*             not enable system clock interrupts unilaterally, but if the timestamp is currently enabled, the clock is disabled and then
*             re-enabled with the new rate.  Normally it is called by usrRoot()
*             in usrConfig.c.
*
* 输入参数  : int ticksPerSecond   number of clock interrupts per second
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 sysTimestampRateSet(BSP_S32 ticksPerSecond);

/*****************************************************************************
* 函 数 名  : sysTimestampRateGet
*
* 功能描述  : This routine returns the interrupt rate of the timestamp clock.
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : The number of ticks per second of the system clock.
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 sysTimestampRateGet (BSP_VOID);
/*************************TIMER END*****************************/

/*************************DRX BEGIN*****************************/
/*PS注册函数类型*/
typedef BSP_U8   (*BSPRegDRXFunc)(BSP_VOID);

typedef enum tagDRX_SLEEP_MODE_E
{
    DRX_WEEK_SLEEP_MODE_E = 0x0,
    DRX_DEEP_SLEEP_MODE_E = 0x1,
    DRX_WEEK2_SLEEP_MODE_E = 0x2,
	DRX_NO_SLEEP_MODE_E = 0x3
}DRX_SLEEP_MODE_E;


typedef enum tagDRX_TIMER_ID_E
{
    DRX_LTESYS_TIMER = 0x0,
    DRX_LTET3412_TIMER,
    DRX_LTEVPLMN_TIMER,
    DRX_LTETSRHC_TIMER,
    DRX_LTETA_TIMER,
    DRX_DEEP_SLEEP_TIMER,
    DRX_LTE_PS32K_TIMER,
    DRX_LTE_PS32K_BASETIMER,
    DRX_LTE_NO_TIMER
}DRX_TIMER_ID_E;

typedef enum tagDRX_TIMER_MODE_E
{
    DRX_TIMER_FREE_MODE = 0x0,
    DRX_TIMER_USER_MODE
}DRX_TIMER_MODE_E;

typedef enum tagDRX_REG_CB_E
{
    DRX_PS_GET_SLEEP_INFO = 0x0,
    DRX_RTC_TIME_START = 0x1,
	DRX_RTC_TIME_END = 0x2
}DRX_REG_CB_E;

/*****************************************************************************
* 函 数 名  : set32KHwTimerMode
*
* 功能描述  : 查询32K硬件时钟计数器值
*
* 输入参数  :
* 输出参数  :
* 返 回 值   :
*
* 修改记录  :

*****************************************************************************/
BSP_S32 set32KHwTimerMode(DRX_TIMER_MODE_E eMode);
/*****************************************************************************
* 函 数 名  : get32KHwTimerMode
*
* 功能描述  : 查询32K硬件时钟计数器的工作模式
*
* 输入参数  :
* 输出参数  :
* 返 回 值   :
*
* 修改记录  :

*****************************************************************************/
DRX_TIMER_MODE_E get32KHwTimerMode();
/*****************************************************************************
* 函 数 名  : BSP_DRX_RegCb
*
* 功能描述  : PS注册函数
*
* 输入参数  :
* 输出参数  :
* 返 回 值   :
*
* 修改记录  :

*****************************************************************************/
BSP_VOID BSP_DRX_RegCb (DRX_REG_CB_E enType, BSPRegDRXFunc pFunc);
/*****************************************************************************
* 函 数 名  : usrTimer4Start
*
* 功能描述  : 启动32K硬件时钟定时器
*
* 输入参数  :
* 输出参数  :
* 返 回 值   :
*
* 修改记录  :

*****************************************************************************/
BSP_S32 usrTimer4Start(DRX_TIMER_ID_E eTimerId, BSP_U32 u32Count);
/*****************************************************************************
* 函 数 名  : usrTimer4Stop
*
* 功能描述  : 停止32K硬件时钟定时器
*
* 输入参数  :
* 输出参数  :
* 返 回 值   :
*
* 修改记录  :

*****************************************************************************/
BSP_S32 usrTimer4Stop(DRX_TIMER_ID_E eTimerId);
/*****************************************************************************
* 函 数 名  : usrTimer4Value
*
* 功能描述  : 查询32K BBP 时钟定时器剩余值
*
* 输入参数  :
* 输出参数  :
* 返 回 值   :
*
* 修改记录  :

*****************************************************************************/
BSP_S32 usrTimer4Value(DRX_TIMER_ID_E eTimerId);
/*************************DRX END*****************************/

/*************************IPCONFIG BEGIN************************/

typedef enum tagBSP_NET_IPVER_E
{
  IPV4 = 0,
  IPV6
}BSP_NET_IPVER_E;

typedef struct tagBSP_NET_NETMASK_S
{
  BSP_U8 au8Mask[4];   //子网掩码
}BSP_NET_NETMASK_S;

typedef struct tagBSP_NET_MAC_S
{
  BSP_U8 au8MAC[6];   // MAC地址
}BSP_NET_MAC_S;

typedef union tagBSP_NET_IP_U
{
  BSP_U8 au8IPV4[4];    // 标明传入的地址为IPV4的地址
  BSP_U8 au8IPV6[16];   //标明传入的地址为IPV6的地址
}BSP_NET_IP_U;

BSP_U8* BSP_GetMacAddr();
//typedef VOID (*PFN_DEVICE_NOTIFY_HOOK)(ENUM_DEVICE_NOTIFY_EVENT ulEvent, ENUM_DEVICE_STATSU ulStatus);


/* 流控开关结构 */
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved1 : 14;/* bit[0-13] : reversed */
        unsigned int  rtsen         : 1;/* bit[14] : rts enable, up stream flow ctrl  */
        unsigned int  ctsen         : 1;/* bit[15] : cts enable, down stream flow ctrl */
        unsigned int  reserved2 : 16;/* bit[16-31] : reversed */
    } reg;
} uart_flow_ctrl_union;




/*****************************************************************************
* 函 数 名  : BSP_SetPsMacAddr
*
* 功能描述  : 修改PS mac地址
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : VOID
*
* 修改记录  : 2009年9月2日   liumengcun  creat
*****************************************************************************/
BSP_S32 BSP_SetPsMacAddr(BSP_U8 u8MacAddr0,BSP_U8 u8MacAddr1,BSP_U8 u8MacAddr2,
		BSP_U8 u8MacAddr3,BSP_U8 u8MacAddr4,BSP_U8 u8MacAddr5);

/*****************************************************************************
* 函 数 名  : BSP_SetOsMacAddr
*
* 功能描述  : 修改vxWorks mac地址
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : VOID
*
* 修改记录  : 2009年9月2日   liumengcun  creat
*****************************************************************************/
BSP_S32 BSP_SetOsMacAddr(BSP_U8 u8MacAddr0,BSP_U8 u8MacAddr1,BSP_U8 u8MacAddr2,
		BSP_U8 u8MacAddr3,BSP_U8 u8MacAddr4,BSP_U8 u8MacAddr5);

/*****************************************************************************
* 函 数 名  : BSP_NET_OMIpMacSet
*
* 功能描述  : 设置vxWorks的 IP/NetMask/Mac地址,并保存到Flash文件系统中
*
* 输入参数  : u32IPVer: IP版本: V4/V6
*             pOMIP:    IP地址
*             pstNetMask:NetMask
*             pOMMac:   Mac地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_NET_OMIpMacSet(BSP_NET_IPVER_E u32IPVer, BSP_NET_IP_U* pOMIP, BSP_NET_NETMASK_S* stNetMask, BSP_NET_MAC_S* pOMMac);

/*****************************************************************************
* 函 数 名  : BSP_NET_OMIpMacGet
*
* 功能描述  : 获取vxWorks的 IP/NetMask/Mac地址
*
* 输入参数  : u32IPVer: IP版本: V4/V6
*             pOMIP:    IP地址
*             pstNetMask:NetMask
*             pOMMac:   Mac地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_NET_OMIpMacGet(BSP_NET_IPVER_E u32IPVer, BSP_NET_IP_U* pOMIP, BSP_NET_NETMASK_S* pstNetMask, BSP_NET_MAC_S* pOMMac);

/*****************************************************************************
* 函 数 名  : BSP_NET_PsMacSet
*
* 功能描述  : 设置协议栈Mac地址,并保存到Flash文件系统中
*
* 输入参数  : u32IPVer: IP版本: V4/V6
*             pOMIP:    IP地址
*             pstNetMask:NetMask
*             pOMMac:   Mac地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_NET_PsMacSet(BSP_NET_MAC_S* pLteMac);

/*****************************************************************************
* 函 数 名  : BSP_NET_PsMacGet
*
* 功能描述  : 获取协议栈Mac地址
*
* 输入参数  : pLteMac:   获取的Mac地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_NET_PsMacGet(BSP_NET_MAC_S* pLteMac);
/*************************IPCONFIG END************************/

/*************************SPI BEGIN************************/
/*错误码定义*/
#define BSP_ERR_SPI_MODULE_NOT_INITED BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_MODULE_NOT_INITED)
#define BSP_ERR_SPI_INVALID_PARA      BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_INVALID_PARA)
#define BSP_ERR_SPI_RETRY_TIMEOUT     BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_RETRY_TIMEOUT)

#define BSP_ERR_SPI_SEM_CREATE        BSP_DEF_ERR(BSP_MODU_SPI, 1)
#define BSP_ERR_SPI_SEM_LOCK          BSP_DEF_ERR(BSP_MODU_SPI, 2)
#define BSP_ERR_SPI_SEM_UNLOCK        BSP_DEF_ERR(BSP_MODU_SPI, 3)
#define BSP_ERR_SPI_DATASIZE_INVALID  BSP_DEF_ERR(BSP_MODU_SPI, 4)
#define BSP_ERR_SPI_ATTR_NOTSET       BSP_DEF_ERR(BSP_MODU_SPI, 5)
#define BSP_ERR_SPI_PROT_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 6)
#define BSP_ERR_SPI_TMOD_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 7)
#define BSP_ERR_SPI_RECEIVE_POLL      BSP_DEF_ERR(BSP_MODU_SPI, 8)



/*****************************************************************************
* 函 数 名  : BSP_SPI_SetAttr
*
* 功能描述  : 配置SPI的寄存器，设置控制命令字长度、数据帧长度等。
*
* 输入参数  : enSpiID        需要设置的SPI号，以及根据哪片片选进行配置的片选号。
*             pstSpiDevAttr  记录SPI外接设备特性的结构体指针，结构体成员包括设备
                             需要的命令字长度，数据帧长度，使用协议，波特率等。
*
* 输出参数  : 无
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_SetAttr(SPI_DEV_S *enSpiID,SPI_DEV_ATTR_S *pstSpiDevAttr);

/*****************************************************************************
* 函 数 名  : BSP_SPI_Write
*
* 功能描述  : 通过SPI向设备写入数据
*
* 输入参数  : pstWriteData 记录与SPI数据传输有关信息的结构体指针，成员包括
                           要读写的SPI号，片选号，传输模式等
*             pSendData    存储接收的数据缓冲区指针
*             u32Length    待接收的数据长度
*
* 输出参数  : 无
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_Write(SPI_DATA_HANDLE_S *pstWriteData,BSP_VOID *pSendData, BSP_U32 u32Length);

/*****************************************************************************
* 函 数 名  : BSP_SPI_Read
*
* 功能描述  : 通过SPI读取设备数据
*
* 输入参数  : pstReadData  记录与SPI数据传输有关信息的结构体指针，成员包括
                           要读写的SPI号，片选号，传输模式等。
*             u32Length    待接收的数据长度
*
* 输出参数  : pRecData     存储接收的数据缓冲区指针。
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_Read(SPI_DATA_HANDLE_S *pstReadData,BSP_VOID *pRecData, BSP_U32 u32Length);

/*****************************************************************************
* 函 数 名  : BSP_SPI_GetAttr
*
* 功能描述  : 获取当前SPI设置的属性
*
* 输入参数  : enSpiId    要查询的SPI号。
*
* 输出参数  : pstDevAttr 存放SPI属性的结构体指针。
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_GetAttr(SPI_DEV_ID_E enSpiId, SPI_DEV_ATTR_S *pstDevAttr);
/*************************SPI END************************/

/*************************IPCM BEGIN************************/
#ifdef BSP_ASIC_V7R1
/* 处理器类型*/
typedef enum tagIPC_INT_CORE_E
{
    IPC_CORE_COMARM = 0x1,
    IPC_CORE_APPARM = 0x2,
    IPC_CORE_DSP    = 0x3,
    IPC_CORE_BUTTOM
}IPC_INT_CORE_E;
#else

/* 处理器类型*/
typedef enum tagIPC_INT_CORE_E
{
    IPC_CORE_ARM11 = 0x0,   /* P500上的IPC */
    IPC_CORE_A9,
    IPC_CORE_CEVA,
    IPC_CORE_TENS0,
    IPC_CORE_TENS1,
    IPC_CORE_DSP,
    IPC_CORE_APPARM = 0x0,  /* V7R1/V3R2上的IPC */
    IPC_CORE_COMARM,
    IPC_CORE_LTEDSP,
    IPC_CORE_VDSP,
    IPC_CORE_ZSP,
    IPC_CORE_DSP_GU,
    IPC_CORE_ACPU = 0x0,/* V9R1 SFT上的IPC */
    IPC_CORE_CCPU,
    IPC_CORE_MCU,
    IPC_CORE_HIFI,
    IPC_CORE_BBE16,


    /* !!!!新增元素请添加到最后  */
    IPC_CORE_BUTTOM
}IPC_INT_CORE_E;

#endif
typedef enum tagIPC_INT_LEV_E
{
    IPC_INT_DSP_MODEM,
    IPC_INT_DSP_APP,
    IPC_INT_MODEM_APP,
    IPC_ACPU_INT_SRC_CCPU_NVIM,
    IPC_INT_DICC_USRDATA = 6,
    IPC_INT_DICC_RELDATA = 7,
    IPC_INT_BUTTOM
}IPC_INT_LEV_E;

typedef enum tagIPC_SEM_ID_E
{
    IPC_SEM_ICC,
    IPC_SEM_NAND,
    IPC_SEM_MEM,
    IPC_SEM_DICC,
    IPC_SEM_RFILE_LOG,          /*用于flashless可维可测*/
    IPC_SEM_EMMC,
    IPC_SEM_NVIM,
    IPC_SEM_SMP_CPU0 = 21,
    IPC_SEM_SMP_CPU1 = 22,
    IPC_SEM_SMP_CPU2 = 23,
    IPC_SEM_SMP_CPU3 = 24,
    IPC_SEM_SYNC = 25,
    IPC_SEM_SYSCTRL = 30,
    IPC_SEM_ZSP_HALT = 31,
    IPC_SEM_BUTTOM
}IPC_SEM_ID_E;

/*****************************************************************************
* 函 数 名  : BSP_DRV_IPCIntInit
*
* 功能描述  : IPC模块初始化
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   wangjing  creat
*****************************************************************************/
BSP_S32 BSP_DRV_IPCIntInit();

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntEnable
*
* 功能描述  : 使能某个中断
*
* 输入参数  :
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntEnable (IPC_INT_LEV_E ulLvl);
#define DRV_IPC_INTENABLE(var)		BSP_IPC_IntEnable(var)

/*****************************************************************************
 * 函 数 名  : BSP_INT_Disable
 *
 * 功能描述  : 去使能某个中断
 *
 * 输入参数  :
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
BSP_S32 BSP_IPC_IntDisable (IPC_INT_LEV_E ulLvl);
#define DRV_IPC_INTDISABLE(var)     BSP_IPC_IntDisable(var)


 /*****************************************************************************
* 函 数 名  : BSP_IPC_IntSend
*
* 功能描述  : 发送中断
*
* 输入参数  :
                IPC_INT_CORE_E enDstore 要接收中断的core
                BSP_U32 ulLvl 要发送的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/

BSP_S32 BSP_IPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);
#define DRV_IPC_INTSEND(a,b)		BSP_IPC_IntSend(a,b)

 /*****************************************************************************
 * 函 数 名  : BSP_IPC_SemTake
 *
 * 功能描述  : 获取信号量
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
 BSP_S32 BSP_IPC_SemTake(BSP_U32 u32SignalNum);
 /*****************************************************************************
 * 函 数 名  : BSP_IPC_SemGive
 *
 * 功能描述  : 释放信号量
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
 BSP_VOID BSP_IPC_SemGive(BSP_U32 u32SignalNum);

 /*****************************************************************************
  * 函 数 名  : BSP_IPC_SpinLock
  *
  * 功能描述  : 获取信号量
  *
  * 输入参数  : 无
  * 输出参数  : 无
  *
  * 返 回 值  : 无
  *
  * 修改记录  : 2011年4月11日 wangjing creat
  *****************************************************************************/

 BSP_VOID BSP_IPC_SpinLock (BSP_U32 u32SignalNum);
#define DRV_SPIN_LOCK()		BSP_IPC_SpinLock(IPC_SEM_ICC)

 /*****************************************************************************
   * 函 数 名  : BSP_IPC_SpinUnLock
   *
   * 功能描述  : 释放信号量
   *
   * 输入参数  : 无
   * 输出参数  : 无
   *
   * 返 回 值  : 无
   *
   * 修改记录  : 2011年4月11日 wangjing creat
   *****************************************************************************/
  BSP_VOID BSP_IPC_SpinUnLock (BSP_U32 u32SignalNum);

#define DRV_SPIN_UNLOCK()		BSP_IPC_SpinUnLock(IPC_SEM_ICC)
 /*****************************************************************************
  * 函 数 名  : BSP_IPC_DebugShow
  *
  * 功能描述  : 可维可测接口
  *
  * 输入参数  : 无
  * 输出参数  : 无
  *
  * 返 回 值  : 无
  *
  * 修改记录  : 2011年4月11日 wangjing creat
  *****************************************************************************/
 BSP_VOID BSP_IPC_DebugShow();


/*************************IPCM END************************/

/*************************BBP INT BEGIN************************/

/* BBP子帧中断处理函数,此中断只对应Modem核*/
typedef BSP_VOID (*BSPBBPIntTimerFunc)(BSP_VOID);
/* 下行业务数据DMA搬移完成中断,此中断只对应Modem核*/
typedef BSP_VOID (*BSPBBPIntDlTbFunc)(BSP_VOID);




/*****************************************************************************
* 函 数 名  : BSP_BBPGetCurTime
*
* 功能描述  : 被PS调用，用来获取系统精确时间
*
* 输入参数  : 无
* 输出参数  : BSP_U32 u32CurTime:当前时刻
*
* 返 回 值  : BSP_OK : 获取成功
*                        BSP_ERR_INVALID_PARA  :获取失败
*
* 修改记录  : 2011年3月7日  wangjing  creat
*****************************************************************************/
BSP_U32 BSP_BBPGetCurTime(BSP_U64 *pCurTime);


/*****************************************************************************
* 函 数 名  : BSP_BBPIntTimerRegCb
*
* 功能描述  : 被PS调用，用来向底软注册1ms定时中断的回调
*
* 输入参数  :
* 输出参数  :无
*
* 返 回 值  : VOID
*
* 修改记录  : 2011年3月7日  wangjing  creat
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerRegCb(BSPBBPIntTimerFunc pFunc);

/*****************************************************************************
* 函 数 名  : BSP_BBPIntDlTbRegCb
*
* 功能描述  : 被PS调用，用来向底软注册下行数据DMA搬移完成中断的回调
*
* 输入参数  :
* 输出参数  :无
*
* 返 回 值  : VOID
*
* 修改记录  : 2011年3月7日  wangjing  creat
*****************************************************************************/
BSP_VOID BSP_BBPIntDlTbRegCb(BSPBBPIntDlTbFunc pFunc);

/*****************************************************************************
* 函 数 名  : BSP_BBPIntTimerClear
*
* 功能描述  : 被PS调用，用来清除1ms定时中断
*
* 输入参数  : 无
* 输出参数  :无
*
* 返 回 值  : VOID
*
* 修改记录  : 2011年5月31日  wangjing  creat
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerClear();

/*****************************************************************************
* 函 数 名  : BSP_BBPIntTimerEnable
*
* 功能描述  : 被PS调用，用来打开1ms定时中断
*
* 输入参数  : 无
* 输出参数  :无
*
* 返 回 值  : VOID
*
* 修改记录  : 2011年3月7日  wangjing  creat
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerEnable();


/*****************************************************************************
* 函 数 名  : BSP_BBPIntTimerDisable
*
* 功能描述  : 被PS调用，用来关闭1ms定时中断
*
* 输入参数  : 无
* 输出参数  :无
*
* 返 回 值  : VOID
*
* 修改记录  : 2011年3月7日  wangjing  creat
*****************************************************************************/
BSP_VOID BSP_BBPIntTimerDisable();


/*************************BBP INT END************************/

/*************************CIPHER BEGIN************************/
/**************************************************************************
  宏定义
**************************************************************************/

/* 打印级别 */
#define CIPHER_PRNT_NOT           BSP_LOG_LEVEL_MAX       /* 不打印任何信息 */
#define CIPHER_PRNT_ERR           BSP_LOG_LEVEL_ERROR     /* 只打印错误信息 */
#define CIPHER_PRNT_WAR           BSP_LOG_LEVEL_WARNING   /* 只打印错误, 告警信息*/
#define CIPHER_PRNT_LOG           BSP_LOG_LEVEL_DEBUG     /* 打印调试, 告警, 错误信息 */

/* 错误码 */
#define CIPHER_SUCCESS                  0

#define CHN_BUSY                    0
#define CHN_FREE                    1
/* 注意CIPHER错误码要为负值 */
#define CIPHER_ERROR_BASE               0x80000800

typedef enum tagCIPHER_ERR_CODE_E
{
    CIPHER_ERR_CODE_NULL_PTR = 1,
    CIPHER_ERR_CODE_NO_MEM ,
    CIPHER_ERR_CODE_NOT_INIT ,
    CIPHER_ERR_CODE_FIFO_FULL ,
    CIPHER_ERR_CODE_INVALID_CHN ,
    CIPHER_ERR_CODE_INVALID_OPT ,
    CIPHER_ERR_CODE_ALIGN_ERROR ,
    CIPHER_ERR_CODE_PURGING ,
    CIPHER_ERR_CODE_TIME_OUT,
    CIPHER_ERR_CODE_INVALID_ENUM,
    CIPHER_ERR_CODE_INVALID_RD,
    CIPHER_ERR_CODE_RD_NULL,
    CIPHER_ERR_CODE_INVALID_KEY,
    CIPHER_ERR_CODE_CHECK_ERROR,
    CIPHER_ERR_CODE_BDLEN_ERROR,
    CIPHER_ERR_CODE_INVALID_NUM
}CIPHER_ERR_CODE_E;

#define CIPHER_NULL_PTR      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NULL_PTR))
#define CIPHER_NO_MEM        ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NO_MEM))
#define CIPHER_NOT_INIT      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NOT_INIT))
#define CIPHER_FIFO_FULL     ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_FIFO_FULL))
#define CIPHER_INVALID_CHN   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_CHN))
#define CIPHER_INVALID_OPT   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_OPT))
#define CIPHER_ALIGN_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_ALIGN_ERROR))
#define CIPHER_PURGING       ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_PURGING))
#define CIPHER_TIME_OUT      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_TIME_OUT))
#define CIPHER_INVALID_ENUM  ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_ENUM))
#define CIPHER_INVALID_RD    ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_RD))
#define CIPHER_RDQ_NULL      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_RD_NULL))
#define CIPHER_INVALID_KEY   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_KEY))
#define CIPHER_CHECK_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_CHECK_ERROR))
#define CIPHER_BDLEN_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_BDLEN_ERROR))
#define CIPHER_INVALID_NUM   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_NUM))

#define CIPHER_UNKNOWN_ERROR ((BSP_S32)(CIPHER_ERROR_BASE|0xff))

/**************************************************************************
  枚举定义
**************************************************************************/
enum CIPHER_SECURITY_CHANNEL_ENUM
{
    CIPHER_SECURITY_CHANNEL_0            = 0,                /*通道0*/
    CIPHER_SECURITY_CHANNEL_1            = 1,                /*通道1*/
    CIPHER_SECURITY_CHANNEL_2            = 2,                /*通道2*/
    CIPHER_SECURITY_CHANNEL_3            = 3,                /*通道3*/

    CIPHER_SECURITY_CHANNEL_BUTT
};

/* 通道号*/
#define LTE_SECURITY_CHANNEL_NAS    CIPHER_SECURITY_CHANNEL_2
#define LTE_SECURITY_CHANNEL_RRC    CIPHER_SECURITY_CHANNEL_2
#define LTE_SECURITY_CHANNEL_UL_SRB CIPHER_SECURITY_CHANNEL_1
#define LTE_SECURITY_CHANNEL_DL_SRB CIPHER_SECURITY_CHANNEL_1
#define LTE_SECURITY_CHANNEL_UL_DRB CIPHER_SECURITY_CHANNEL_0
#define LTE_SECURITY_CHANNEL_DL_DRB CIPHER_SECURITY_CHANNEL_3

/* CIPHER单独操作分类 */
typedef enum tagCIPHER_SINGLE_OPT_E
{
    /* 单独操作类型,单独枚举值与寄存器设置值一致 */
    CIPHER_OPT_ENCRYPT = 0x0,   /* 加密 */
    CIPHER_OPT_DECRYPT = 0x1,   /* 解密 */
    CIPHER_OPT_PROTECT_SMAC = 0x2,   /* 保护操作，输出MAC长度为2Byte */
    CIPHER_OPT_PROTECT_LMAC = 0x3,   /* 保护操作，输出MAC长度为4Byte */
    CIPHER_OPT_CHECK_PRE_SMAC  = 0x4,   /* 检查操作，MAC长度为2Byte，位置在数据前面 */
    CIPHER_OPT_CHECK_POST_SMAC = 0x5,   /* 检查操作，MAC长度为2Byte，位置在数据后面 */
    CIPHER_OPT_CHECK_PRE_LMAC  = 0x6,   /* 检查操作，MAC长度为4Byte，位置在数据前面 */
    CIPHER_OPT_CHECK_POST_LMAC = 0x7,   /* 检查操作，MAC长度为4Byte，位置在数据后面 */
    CIPHER_SINGLE_OPT_BUTTOM
}CIPHER_SINGLE_OPT_E;


/* CIPHER关联操作分类 */     //PDCP 关联操作随意组合
typedef enum tagCHPHER_RELA_OPT_E
{
    /*  关联操作类型 */
    CIPHER_PDCP_PRTCT_ENCY,     /*PDCP保护加密(上行)*/
    CIPHER_PDCP_DECY_CHCK,      /*PDCP解密检查(下行)*/

    CIPHER_NAS_ENCY_PRTCT,      /* NAS层加密保护(上行) */
    CIPHER_NAS_CHCK_DECY,       /* NAS层检查解密(下行) */

	CIPHER_RELA_OPT_BUTTOM
}CIPHER_RELA_OPT_E;

/* CIPHER支持的算法种类,注意枚举值与寄存器设置值一致 */
typedef enum tagCIPHER_ALGORITHM_E
{
    CIPHER_ALG_NULL = 0,        /* 无算法 */
    CIPHER_ALG_SNOW3G  = 1,     /* SNOW3G算法 */
    CIPHER_ALG_AES_128 = 2,     /* AES128算法 */
    CIPHER_ALG_AES_192 = 3,     /* AES192算法 */
    CIPHER_ALG_AES_256 = 4,     /* AES256算法 */
    CIPHER_ALG_BUTTOM
}CIPHER_ALGORITHM_E;

/* CIPHER密钥长度, 注意枚举值与寄存器设置值一致 */
typedef enum tagCIPHER_KEY_LEN_E
{
    CIPHER_KEY_L128 = 0,        /* 密钥长度128 bit */
    CIPHER_KEY_L192 = 1,        /* 密钥长度192 bit */
    CIPHER_KEY_L256 = 2,        /* 密钥长度256 bit */
    CIPHER_KEY_LEN_BUTTOM
}CIPHER_KEY_LEN_E;

/* CIPHER操作提交类型 */
typedef enum tagCIPHER_SUBMIT_TYPE_E
{
    CIPHER_SUBM_NONE = 0,           /* 不作任何通知 */
    CIPHER_SUBM_BLK_HOLD = 1,       /* 循环查询等待完成 */
    CIPHER_SUBM_CALLBACK = 2,       /* 回调函数通知 */
    CIPHER_SUBM_BUTTOM
}CIPHER_SUBMIT_TYPE_E;

/* CIPHER通知类型 */
typedef enum tagCIPHER_NOTIFY_STAT_E
{
    CIPHER_STAT_OK = 0,           /* 成功完成 */
    CIPHER_STAT_CHECK_ERR = 1,    /* 完整性检查错误 */
    CIPHER_STAT_BUTTOM
}CIPHER_NOTIFY_STAT_E;

typedef enum CIPHER_HDR_BIT_ENUM
{
    CIPHER_HDR_BIT_TYPE_0              = 0,     /*对应的头或附加头字节长度为0，即不启动此功能*/
    CIPHER_HDR_BIT_TYPE_5              = 1,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低5位，参与保护*/
    CIPHER_HDR_BIT_TYPE_7              = 2,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低7位，参与保护*/
    CIPHER_HDR_BIT_TYPE_12             = 3,     /*对应的头或附加头字节长度为2，即启动此功能，对应附加头的内容是Count值的低12位，参与保护*/

    CIPHER_APPEND_HDR_BIT_TYPE_5       = 4,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低5位，不参与保护，只搬移*/
    CIPHER_APPEND_HDR_BIT_TYPE_7       = 5,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低7位，不参与保护，只搬移*/
    CIPHER_APPEND_HDR_BIT_TYPE_12      = 6,     /*对应的头或附加头字节长度为2，即启动此功能，对应附加头的内容是Count值的低12位，不参与保护，只搬移*/

    CIPHER_HDR_BIT_TYPE_BUTT
}CIPHER_HDR_E;

/* 完成回调函数类型定义 */
/*
u32ChNum   :  通道号;
u32SourAddr:  源地址
u32DestAddr:  目的地址
enStatus   :  通知的状态,比如: 完成 / 数据传输出错 / 数据完成性检查出错
u32Private   :  用户私有数据;
*/
typedef BSP_VOID (*CIPHER_NOTIFY_CB_T)(BSP_U32 u32ChNum, BSP_U32 u32SourAddr,
              BSP_U32 u32DestAddr, CIPHER_NOTIFY_STAT_E enStatus, BSP_U32 u32Private);

typedef BSP_VOID (*CIPHER_FREEMEM_CB_T)(BSP_VOID* pMemAddr);

/**************************************************************************
  STRUCT定义
**************************************************************************/

/* 算法配置信息 */
typedef struct tagCIHPER_ALG_INFO_S
{
    BSP_U32 u32KeyIndexSec;
    BSP_U32 u32KeyIndexInt;
    CIPHER_ALGORITHM_E enAlgSecurity;          /* 安全操作算法选择，AEC算法还是SNOW3G算法 */
    CIPHER_ALGORITHM_E enAlgIntegrity;         /* 完整性操作算法选择，AEC算法还是SNOW3G算法 */
}CIHPER_ALGKEY_INFO_S;

/* CIPHER配置信息设置 */
typedef struct tagCIHPER_SINGLE_CFG_S
{
    CIPHER_SINGLE_OPT_E enOpt;          /* 单独操作类型(纯DMA操作忽略下面的算法配置) */
    BSP_U8 u8BearId;
    BSP_U8 u8Direction;
    CIPHER_HDR_E enAppdHeaderLen;
    CIPHER_HDR_E enHeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* 是否是单内存块 */
	BSP_U32 u32BlockLen;                /* 如果是单块内存，需要知道长度*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* 单独操作的算法密钥配置信息 */
} CIHPER_SINGLE_CFG_S;

typedef struct tagCIHPER_RELA_CFG_S
{
    CIPHER_RELA_OPT_E enOpt;             /* 关联操作类型(纯DMA操作忽略下面的算法配置) */
    BSP_U8 u8BearId;
    BSP_U8 u8Direction;
    CIPHER_HDR_E enAppdHeaderLen;
    CIPHER_HDR_E enHeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* 是否是单内存块 */
	BSP_U32 u32BlockLen;                /* 如果是单块内存，需要知道长度*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* 关联操作第一步的算法密钥配置信息 */

} CIHPER_RELA_CFG_S;


/*组包加速配置信息*/
typedef struct tagACC_SINGLE_CFG_S
{
    BSP_U32 u32BearId;
    BSP_U32 u32AppdHeaderLen;
    BSP_U32 u32HeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* 是否是单内存块 */
    BSP_U32 u32BlockLen;                /* 如果是单块内存，需要知道长度*/
    BSP_U32 u32Offset;                  /* 使用数据距离数据包起始地址偏移*/
    BSP_U32 u32OutLen;                  /* 使用到的数据长度*/
    BSP_U32 u32Aph;                      /*附加包头域*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* 单独操作的算法密钥配置信息 */
} ACC_SINGLE_CFG_S;

/**************************************************************************
  函数声明
**************************************************************************/

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_RegistNotifyCB
*
* 功能描述  : 注册通知回调函数
*
* 输入参数  : pFunNotifyCb: 通知回调函数
*
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*****************************************************************************/
BSP_S32 BSP_CIPHER_RegistNotifyCB(CIPHER_NOTIFY_CB_T pFunNotifyCb);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_FreeMemCB
*
* 功能描述  : 注册Memory释放回调函数
*
* 输入参数  : u32Chn: 通道号
*             bSrc:   0:输入Buffer / 1:输出Bufffer
*             pFunFreeMemCb: Memory释放回调函数
*
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*****************************************************************************/
BSP_S32 BSP_CIPHER_RegistFreeMemCB(BSP_U32 u32Chn, BSP_BOOL bSrc, CIPHER_FREEMEM_CB_T pFunFreeMemCb);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_Purge
*
* 功能描述  : 清除所有当前指定通道的CIPHER任务(阻塞接口,完成purge后返回)
*
* 输入参数  : u32Chn: 要清除的通道号
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*           :      CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_Purge(BSP_U32 u32Chn);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_SingleSubmitTask
*
* 功能描述  : 开始CIPHER 指定通道的工作，用于单独操作。
*
* 输入参数  : u32Chn: 通道号
*             pInMemMgr: 输入buffer内存管理结构指针
*             pOutMemMgr:输出buffer内存管理结构指针
*             pstCfg: Cipher操作配置属性
*             pstSubmAttr:提交的属性
*             u32Private:私有数据
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*             失败:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_SingleSubmitTask(BSP_U32 u32Chn, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
                    CIHPER_SINGLE_CFG_S *pstCfg, CIPHER_SUBMIT_TYPE_E enSubmAttr, BSP_U32 u32Private);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_RelaSubmitTask
*
* 功能描述  : 开始CIPHER 指定通道的工作，用于关联操作。
*
* 输入参数  : u32Chn: 通道号
*             pInMemMgr: 输入buffer内存管理结构指针
*             pOutMemMgr:输出buffer内存管理结构指针
*             pstCfg: Cipher操作配置属性
*             enSubmAttr:提交的属性
*             u32Private:私有数据
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*             失败:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_RelaSubmitTask(BSP_U32 u32Chn, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
                    CIHPER_RELA_CFG_S *pstCfg, CIPHER_SUBMIT_TYPE_E enSubmAttr, BSP_U32 u32Private);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_SetKeyIndex
*
* 功能描述  : 设置密钥值
*
* 输入参数  : pKeyAddr   : 密钥地址
*             enKeyLen   : 密钥长度
*             u32KeyIndex: 密钥索引
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_NULL_PTR
*           :      CIPHER_INVALID_ENUM
*           :      CIPHER_INVALID_KEY
*****************************************************************************/
BSP_S32 BSP_CIPHER_SetKey(const BSP_VOID* pKeyAddr,CIPHER_KEY_LEN_E enKeyLen,BSP_U32 u32KeyIndex);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_GetKey
*
* 功能描述  : 根据KeyIndex获取Key的地址
*
* 输入参数  : u32KeyIndex: 密钥索引
* 输出参数  : 无
* 返 回 值  : 成功:根据KeyIndex得到的Key的地址
*           : 失败:获取Key值所在地址失败,返回0
*****************************************************************************/
BSP_U32 BSP_CIPHER_GetKey(BSP_U32 u32KeyIndex);

/*****************************************************************************
* 函 数 名  : CIPHER_DataBuffPara
*
* 功能描述  : 获取上层数据信息并设置信息
*
* 输入参数  : u32Chn        : 通道号
*             bSrc          : TRUE:源地址的属性配置 / FALSE:目的地址属性配置
*             u32BufOft     : 数据buffer指针偏移
*             u32LenOft     : Buffer长度偏移
*             NextOft       : 下一节点偏移
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*****************************************************************************/
BSP_S32 BSP_CIPHER_SetDataBufPara(BSP_U32 u32Chn, BSP_BOOL bSrc, BSP_U32 u32BufOft,
                                  BSP_U32 u32LenOft, BSP_U32 u32NextOft);

/*****************************************************************************
* 函 数 名  : CIPHER_GetCmplSrcBuff
*
* 功能描述  : 获取已经操作完成的源数据地址
*
* 输入参数  : u32ChNum     : 通道号
* 输出参数  : pu32SourAddr : 操作完成的源地址;
*             pu32DestAddr : 操作完成的目的地址
*             penStatus    : 操作完成的状态
*             pu32Private  : 返回的私有数据
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*           :      CIPHER_NULL_PTR
*           :      CIPHER_INVALID_RD
*****************************************************************************/
BSP_S32 BSP_CIPHER_GetCmplSrcBuff(BSP_U32 u32Chn, BSP_U32 *pu32SourAddr,
              BSP_U32 *pu32DestAddr, CIPHER_NOTIFY_STAT_E *penStatus, BSP_U32 *pu32Private);

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_GetBdFifoAddr
*
* 功能描述  : 用于获取当前可用的BDFIFO首地址
*
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值  : BDFIFO首地址
*****************************************************************************/
BSP_U32 BSP_Accelerator_GetBdFifoAddr();

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_DMA
*
* 功能描述  : 为指定通道配置描述符，用于单纯的DMA搬移
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*             pInMemMgr    : 输入buffer首地址
*             pOutMemMgr   : 输出buffer首地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_Accelerator_DMA(BSP_U32 u32BdFifoAddr, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,ACC_SINGLE_CFG_S *pstCfg);

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_Cipher
*
* 功能描述  : 为指定通道配置描述符，用于需要Cipher进行加密的操作
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*             pInMemMgr    : 输入buffer首地址
*             pOutMemMgr   : 输出buffer首地址
*             pstCfg       : Cipher配置信息
* 输出参数   : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_Accelerator_Cipher(BSP_U32 u32BdFifoAddr, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
               ACC_SINGLE_CFG_S *pstCfg);

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_GetStatus
*
* 功能描述  : 获取当前通道状态
*
* 输入参数  :  无
* 输出参数   : 无
* 返 回 值  : 通道忙/空闲
*****************************************************************************/
BSP_S32 BSP_Accelerator_GetStatus();

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_Enable
*
* 功能描述  : 使能组包加速
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_Accelerator_Enable(BSP_U32 u32BdFifoAddr);
/*************************CIPHER END************************/

/*************************DMA BEGIN************************/
/**************************************************************************
  全局变量声明
**************************************************************************/

/**************************************************************************
  枚举定义
**************************************************************************/
/*DMA transport type*/
typedef enum tagDMA_TRANS_TYPE_E
{
    MEM2MEM_DMA = 0,    /* 内存到内存，DMA流控*/
    MEM2PRF_DMA,        /* 内存到外设，DMA流控*/
    PRF2MEM_DMA,        /* 外设到内存，DMA流控*/
    PRF2PRF_DMA,        /* 外设到外设，DMA流控*/
    PRF2MEM_PRF,        /* 外设到内存，外设流控*/
    PRF2PRF_SRCPRF = 5, /* 外设到外设，源外设流控*/
    MEM2PRF_PRF,        /* 内存到外设，外设流控*/
    PRF2PRF_DSTPRF,     /* 外设到外设，目的外设流控*/
    CTL_TT_FC_BUTTOM    /* 最大值，判断用*/
} DMA_TRANS_TYPE_E;

/**************************************************************************
  宏定义
**************************************************************************/

/*错误码定义*/
#define DMA_SUCCESS                    BSP_OK

#define BSP_ERR_DMA_MODULE_NOT_INITED   BSP_DEF_ERR(BSP_MODU_DMAC, BSP_ERR_MODULE_NOT_INITED)
#define BSP_ERR_DMA_NULL_PTR            BSP_DEF_ERR(BSP_MODU_DMAC, BSP_ERR_NULL_PTR)
#define BSP_ERR_DMA_INVALID_PARA        BSP_DEF_ERR(BSP_MODU_DMAC, BSP_ERR_INVALID_PARA)

#define BSP_ERR_DMA_CHANNEL_BUSY        BSP_DEF_ERR(BSP_MODU_DMAC, 1)
#define BSP_ERR_DMA_ALL_CHANNEL_BUSY    BSP_DEF_ERR(BSP_MODU_DMAC, 2)
#define BSP_ERR_DMA_STOP_FAIL           BSP_DEF_ERR(BSP_MODU_DMAC, 3)

/**************************************************************************
  枚举定义
**************************************************************************/

/* 通道逻辑状态*/
typedef enum tagDMA_CHN_USR_STATUS_E
{
    DMA_CHN_IDLE = 0, /* 通道空闲*/
    DMA_CHN_ALLOCED = 1 /* 通道已经被申请*/
}DMA_CHN_USR_STATUS_E;

/* 通道属性*/
typedef enum tagDMA_CHN_TYPE_E
{
    DMA_FIXED_MODE = 0,   /* 固定通道，使用完成之后不会自动释放，但是可以通过释放通道接口释放*/
    DMA_COMMON_MODE = 1,   /* 普通通道，使用完成之后会自动释放，也可以通过释放通道接口释放*/
    DMA_SPECIAL_MODE = 2   /*
特殊通道，使用完成之后不会自动释放，通道释放接口也无法释放，且该通道需要某些特殊处理，目前只有协议栈使用*/
}DMA_CHN_TYPE_E;

/* 通道状态 */
typedef enum tagDMA_CHN_STATUS_E
{
    DMA_CHN_FREE = 0,   /* 通道空闲 */
    DMA_CHN_BUSY = 1   /* 通道忙 */
}DMA_CHN_STATUS_E;



/* DESCRIPTION
 * This data type is used for selecting the transfer width for the
 *  source and/or destination on a DMA channel when using the specified
 *  driver API functions. This data type maps directly to the AMBA AHB
 *  HSIZE parameter.
 * NOTES
 *  This data type relates directly to the following DMA Controller
 *  register(s) / bit field(s): (x = channel number)
 *    - CTLx.SRC_TR_WIDTH, CTLx.DST_TR_WIDTH
 */

typedef enum tagDMA_TRANS_WIDTH_E {
    DMA_TRANS_WIDTH_8   = 0x0,/* 8bit位宽*/
    DMA_TRANS_WIDTH_16  = 0x1,/* 16bit位宽*/
    DMA_TRANS_WIDTH_32  = 0x2,/* 32bit位宽*/
    DMA_TRANS_WIDTH_64  = 0x3,/* 64bit位宽*/
    DMA_TRANS_WIDTH_128 = 0x4,/* 128bit位宽*/
    DMA_TRANS_WIDTH_256 = 0x5 /* 256bit位宽*/
}DMA_TRANS_WIDTH_E;

/* DESCRIPTION
 *  This data type is used for selecting the burst transfer length
 *  on the source and/or destination of a DMA channel when using the
 *  specified driver API functions. These transfer length values do
 *  not relate to the AMBA HBURST parameter.
 * NOTES
 *  This data type relates directly to the following DMA Controller
 *  register(s) / bit field(s): (x = channel number)
 *    - CTLx.SRC_MSIZE, CTLx.DEST_MSIZE
 */
typedef enum tagDMA_BURST_TRANS_LENGTH_E
{
    DMA_MSIZE_1   = 0x0,/* burst长度，即一次传输的个数为1个*/
    DMA_MSIZE_4   = 0x1,/* burst长度，即一次传输的个数为4个*/
    DMA_MSIZE_8   = 0x2,/* burst长度，即一次传输的个数为8个*/
    DMA_MSIZE_16  = 0x3,/* burst长度，即一次传输的个数为16个*/
    DMA_MSIZE_32  = 0x4,/* burst长度，即一次传输的个数为32个*/
    DMA_MSIZE_64  = 0x5,/* burst长度，即一次传输的个数为64个*/
    DMA_MSIZE_128 = 0x6,/* burst长度，即一次传输的个数为128个*/
    DMA_MSIZE_256 = 0x7 /* burst长度，即一次传输的个数为256个*/
}DMA_BURST_TRANS_LENGTH_E;

/* DMA 硬件握手接口*/
typedef enum tagDMA_HS_HK_INTERFACE_E
{
    DMA_UART0_RX = 0x0,
    DMA_UART0_TX = 0x1,
    DMA_SSI2_RX = 0x2,
    DMA_SSI2_TX = 0x3,
    DMA_SSI1_RX = 0x4,
    DMA_SSI1_TX = 0x5,
    DMA_SSI0_RX = 0x6,
    DMA_SSI0_TX = 0x7,
    DMA_MMC     = 0x8,
    DMA_UART1_RX = 0x9,
    DMA_UART1_TX = 0xa,
    DMA_SCI_RX = 0xb,
    DMA_SCI_TX = 0xc
}DMA_HS_HK_INTERFACE_E;


/**************************************************************************
  STRUCT定义
**************************************************************************/
/*链表传输参数结构体*/
typedef struct tagDMA_LLI_S
{
    BSP_U32 u32Reserved[2]; /* reserved 8 bytes  PS要求格式，使用时保留即可*/
    struct  tagDMA_LLI_S *pNextLLI; /* next node */
    BSP_U32 TransLength;  /* transfer length */
    BSP_U32 SrcAddr; /*source address*/
    BSP_U32 DstAddr; /*destination address，PS可以不关注这个参数*/
} DMA_LLI_S;

/*底软需要构建的符合dma硬件要求的数据结构*/
typedef struct tagDMA_LLI_PHY_S
{
    BSP_U32 SrcAddr; /*source address*/
    BSP_U32 DstAddr; /*destination address*/
    BSP_U32 NextLLI; /*pointer to next LLI  the pSrcAddr of next node*/
    BSP_U32 ulTransferCtrl_l; /*control word low 32 bits */
    BSP_U32 ulTransferCtrl_h; /*control word high 32 bits */
    BSP_U32 s_stat; /*source state*/
    BSP_U32 d_stat; /*dest state*/
} DMA_LLI_PHY_S;

/*传输通道参数结构*/
typedef struct tagDMA_CHN_PARA_S
{
    BSP_U32 u32Chan; /*通道号*/
    BSP_VOID * pfuncDmaTcIsr;/* 完成中断回调函数*/
    BSP_S32 s32DmaTcPara; /* 完成中断回调函数参数*/
    BSP_VOID * pfuncDmaErrIsr;/* 错误中断回调函数*/
    BSP_S32 s32DmaErrPara; /* 错误中断回调函数参数*/
} DMA_CHN_PARA_S;

/*sturcture for single block*/
typedef struct tagDMA_SINGLE_BLOCK_S
{
    DMA_CHN_PARA_S stChnPara;/*传输通道参数结构*/
    DMA_TRANS_TYPE_E enTransType; /*传输流控类型*/
    DMA_HS_HK_INTERFACE_E enSrcHSHKInterface;/* 硬件握手接口*/
    DMA_HS_HK_INTERFACE_E enDstHSHKInterface;/* 硬件握手接口*/
    DMA_TRANS_WIDTH_E enSrcWidth; /*源位宽*/
    DMA_TRANS_WIDTH_E enDstWidth; /*目的位宽 */
    DMA_BURST_TRANS_LENGTH_E enSrcBurstLength; /*源burst长度 */
    DMA_BURST_TRANS_LENGTH_E enDstBurstLength; /*目的burst长度*/
    BSP_U32 u32SrcAddr;/* 源地址*/
    BSP_U32 u32DstAddr;/* 目的地址*/
    BSP_U32 u32TransLength;/* 传输总长度*/
} DMA_SINGLE_BLOCK_S;

/**************************************************************************
  UNION定义
**************************************************************************/

/**************************************************************************
  OTHERS定义
**************************************************************************/

/**************************************************************************
  函数声明
**************************************************************************/

/*****************************************************************************
* 函 数 名  : BSP_DMA_Init
*
* 功能描述  : DMA初始化
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2009年1月20日   吴振  创建

*****************************************************************************/
BSP_S32 BSP_DMA_Init(BSP_VOID);
/*****************************************************************************
* 函 数 名  : BSP_DMA_GetIdleChannel
*
* 功能描述  : 获取一个当前处于空闲状态的DMA通道，当前块或者链表传输完数据后会
*             自动释放，下次再使用时需再获取当前空闲通道,目前DMA_SPECIAL_MODE
*             仅供协议栈使用，其他用户申请不到此种通道
*
* 输入参数  : DMA_CHN_TYPE_E enChnType 通道类型
*
* 输出参数  : BSP_U32 *pulChan      存放申请到的通道号
* 返 回 值  :  DMA_SUCCESS       安装成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*             BSP_ERR_DMA_ALL_CHANNEL_BUSY 所有通道忙
*
* 修改记录  :2009年10月15日   liumengcun  创建

*****************************************************************************/
BSP_S32  BSP_DMA_GetIdleChannel(DMA_CHN_TYPE_E enChnType, BSP_U32 *pulChan);

/*****************************************************************************
* 函 数 名  : BSP_DMA_ChannelRelease
*
* 功能描述  : DMA传输通道释放,正常情况下不需要调用此接口释放通道，传送完成后
*             底软会自动释放通道，本接口为了处理某些异常情况下，取得了通道但
*             是却没有开始数据传输，无法自动释放通道的时候调用此接口手动释放通道
*             通道正在传输时无法释放通道，通道0给协议栈固定使用无法释放。
*
* 输入参数  : BSP_U32 ulChan  通道号
* 输出参数  : 无
* 返 回 值  : DMA_SUCCESS       释放成功
*             DMA_NOT_INIT      DMA未初始化
*             DMA_PARA_INVALID  参数错误
*             DMA_CHANNEL_BUSY  通道忙
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_U32  BSP_DMA_ChannelRelease(BSP_U32 u32Chan);

/*****************************************************************************
* 函 数 名  : BSP_DMA_AllChannelCheck
*
* 功能描述  : DMA全部通道是否空闲检测函数
*
* 输入参数  : 无
* 输出参数  : pChannelInfo   所有通道使用状态，用bit掩码表示，正在传输数据的通道bit位置1，
*               bit0对应0通道，bit1对应1通道，以此类推
* 返 回 值  : DMA_ERROR
*             DMA_SUCCESS
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_S32  BSP_DMA_AllChannelCheck(BSP_U32 *pChannelInfo);

/*TCP/IP协议栈可维可测捕获的消息标识*/
enum IPS_MNTN_TRACE_IP_MSG_TYPE_ENUM
{
    /* IP 数据包可维可测上报 */
    ID_IPS_TRACE_IP_ADS_UL                  = 0xD030,
    ID_IPS_TRACE_IP_ADS_DL                  = 0xD031,
    ID_IPS_TRACE_IP_USB_UL                  = 0xD032,
    ID_IPS_TRACE_IP_USB_DL                  = 0xD033,

    ID_IPS_TRACE_IP_MSG_TYPE_BUTT
};
/********************************************************
函数说明：协议栈注册USB将SKB关键信息上报到SDT回调函数
函数功能:
输入参数：pFunc: SKB关键信息上报到SDT回调函数指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*USB_IPS_MNTN_TRACE_CB_T)(struct sk_buff *skb,unsigned short usType);
extern unsigned int BSP_USB_RegIpsTraceCB(USB_IPS_MNTN_TRACE_CB_T pFunc);
#define DRV_USB_REG_IPS_TRACECB(x) BSP_USB_RegIpsTraceCB(x)

/********************************************************
函数说明：协议栈注册USB使能通知回调函输
函数功能:
输入参数：pFunc: USB使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*USB_UDI_ENABLE_CB_T)(void);
extern unsigned int BSP_USB_RegUdiEnableCB(USB_UDI_ENABLE_CB_T pFunc);
#define DRV_USB_REGUDI_ENABLECB(x) BSP_USB_RegUdiEnableCB(x)

/********************************************************
函数说明：协议栈注册USB去使能通知回调函输
函数功能:
输入参数：pFunc: USB使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*USB_UDI_DISABLE_CB_T)(void);
extern unsigned int BSP_USB_RegUdiDisableCB(USB_UDI_DISABLE_CB_T pFunc);
#define DRV_USB_REGUDI_DISABLECB(x) BSP_USB_RegUdiDisableCB(x)

/********************************************************
函数说明：协议栈注册HSIC使能通知回调函输
函数功能:
输入参数：pFunc: HSIC使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*HSIC_UDI_ENABLE_CB_T)(void);
extern unsigned int BSP_HSIC_RegUdiEnableCB(HSIC_UDI_ENABLE_CB_T pFunc);
#define DRV_HSIC_REGUDI_ENABLECB(x) BSP_HSIC_RegUdiEnableCB(x)

/********************************************************
函数说明：协议栈注册HSIC去使能通知回调函输
函数功能:
输入参数：pFunc: HSIC去使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*HSIC_UDI_DISABLE_CB_T)(void);
extern unsigned int BSP_HSIC_RegUdiDisableCB(HSIC_UDI_DISABLE_CB_T pFunc);
#define DRV_HSIC_REGUDI_DISABLECB(x) BSP_HSIC_RegUdiDisableCB(x)
/********************************************************
函数说明：协议栈查询HSIC枚举状态
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  1：枚举完成
          0：枚举未完成
********************************************************/
extern unsigned int BSP_GetHsicEnumStatus(void);
#define DRV_GET_HSIC_ENUM_STATUS() BSP_GetHsicEnumStatus()

/*****************************************************************************
* 函 数 名  : BSP_DMA_OneChannelCheck
*
* 功能描述  : DMA单个通道具体状态检查
*
* 输入参数  : BSP_U32 ulChan   待检查的通道号
* 输出参数  : 无
* 返 回 值  : DMA_SUCCESS    该通道传输完成
*          DMA_TRXFER_ERROR   该通道传输错误
*          DMA_NOT_FINISHED   该通道传输未完成
*          DMA_ERROR    未知状态
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_S32 BSP_DMA_OneChannelCheck(BSP_U32 ulChan, BSP_U32 *pChannelStatus);

/*****************************************************************************
* 函 数 名  : BSP_DMA_SingleBlockStart
*
* 功能描述  : 单块数据传输启动
*
* 输入参数  : DMA_SINGLE_BLOCK_S stDMASingleBlock   单块传输使用的参数结构体
*
* 输出参数  : 无
* 返 回 值  : DMA_SUCCESS    传输启动完成
*          DMA_CHANNEL_INVALID   传入通道号错误
*          DMA_TRXFERSIZE_INVALID   传入传输长度非法
*          DMA_CHANNEL_BUSY         通道忙
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_S32 BSP_DMA_SingleBlockStart(DMA_SINGLE_BLOCK_S * stDMASingleBlock);

/*****************************************************************************
* 函 数 名  : BSP_DMA_SetDstAddr
*
* 功能描述  : 底层提供给协议栈的配置目的地址函数，协议栈目的地址固定不会变化
*
* 输入参数  : BSP_U32 u32Addr  待传输的目的地址
* 输出参数  : 无
* 返 回 值  : DMA_SUCCESS  设置成功
*
* 修改记录  :2009年7月24日   吴振  创建

*****************************************************************************/
BSP_U32 BSP_DMA_SetDstAddr(BSP_U32 u32Addr);

/*****************************************************************************
* 函 数 名  : BSP_DMA_LliBuild
*
* 功能描述  : 链表构建函数
*
* 输入参数  : BSP_U32 ulChan        通道号
*             DMA_LLI_S *pFirstLLI  资源链表的第一个节点指针
* 输出参数  : 无
* 返 回 值  : BSP_VOID *  指向底层建立好的传输链表的头节点的指针
*
* 修改记录  :2009年7月24日   吴振  创建

*****************************************************************************/
BSP_VOID * BSP_DMA_LliBuild(BSP_U32 ulChan, DMA_LLI_S *pFirstLLI);

/*****************************************************************************
* 函 数 名  : BSP_DMA_LliStart
*
* 功能描述  : 链表数据传输启动专用函数
*
* 输入参数  : BSP_U32 ulChan        通道号
*             BSP_VOID *pu32Head：   调用BSP_DMA_LliBuild获得的待传输链表头指针
* 输出参数  : 无
* 返 回 值  : DMA_SUCCESS    传输启动完成
*          DMA_LLIHEAD_ERROR   传入的链表头错误
*          DMA_CHANNEL_INVALID   传入通道号错误
*          DMA_ERROR    通道不空闲或者DMA未正确初始化
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_U32 BSP_DMA_LliStart(DMA_CHN_PARA_S * stChnPara, BSP_VOID *pu32Head);

/*****************************************************************************
* 函 数 名  : BSP_DMA_GetDar
*
* 功能描述  : 外设使用该函数获取DMA的DAR寄存器的值
*
* 输入参数  : BSP_U32 u32Chan   通道号
*
* 输出参数  : BSP_U32 *u32Dar  该通道对应的目的地址寄存器的值
* 返 回 值  : DMA_SUCCESS       获取成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*
* 修改记录  :2010年12月1日   鲁婷  创建

*****************************************************************************/
BSP_S32 BSP_DMA_GetDar(BSP_U32 u32Chan, BSP_U32 *u32Dar);

/*****************************************************************************
* 函 数 名  : BSP_DMA_DisableChannel
*
* 功能描述  : 外设使用该函数去使能DMAC通道
*
* 输入参数  : BSP_U32 u32Chan   通道号
*
* 输出参数  : 无
* 返 回 值  : DMA_SUCCESS       获取成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*
* 修改记录  :2010年12月1日   鲁婷  创建

*****************************************************************************/
BSP_S32 BSP_DMA_DisableChannel(BSP_U32 u32Chan);

/*************************DMA END************************/

/*************************NANDF BEGIN************************/
/**************************************************************************
  宏定义
**************************************************************************/
/*错误码*/
#define NANDF_OK              BSP_OK
#define NANDF_ERROR_INIT      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_MODULE_NOT_INITED)  /*没有初始化*/
#define NANDF_ERROR_ARGS      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_INVALID_PARA)  /*参数错误*/
#define NANDF_ERROR_MALLOC    BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_BUF_ALLOC_FAILED)  /*内存空间分配失败*/
#define NANDF_ERROR_TIMEOUT   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_RETRY_TIMEOUT)  /*循环次数达到最大*/
#define NANDF_BAD_BLOCK       BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 1)  /*坏块*/
#define NANDF_GOOD_BLOCK      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 2)  /*好块*/
#define NANDF_ERROR_READ      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 3)  /*读操作失败*/
#define NANDF_ERROR_WRITE     BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 4)  /*写操作失败*/
#define NANDF_ERROR_ERASE     BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 5)  /*擦除操作失败*/
#define NANDF_ERROR_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 6)  /*标志坏块失败*/
#define NANDF_ERROR_ERASE_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 7)  /*擦除失败并且标志坏块失败*/
#define NANDF_ERROR_WRITE_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 8)  /*写入失败并且标志坏块失败*/
#define NANDF_ERROR_STATUS_CHECK   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 9)  /*读取状态寄存器，状态不正确*/
#define NANDF_ERROR_SEMTAKE   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 10)  /*申请信号量失败*/
#define NANDF_ERROR_SEMGIVE   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 11)  /*释放信号量失败*/
#define NANDF_ERROR_ADDR_OVERFLOW BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 12)  /* 地址越界 */
#define NANDF_ERROR_MODULE_LOCKED  BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 13)

/******************************************************************************
* Function     :   BSP_NANDF_Read
*
* Description  :   读取Nand Flash中指定地址和长度内容到指定空间中
*
* Input        :   u32FlashAddr  读取数据的源地址
*              :   u32NumByte    读取数据长度，单位为字节
*
* Output       :   pRamAddr      读取的数据存放的地址
*
* return       :   读操作成功与否
******************************************************************************/
BSP_S32 BSP_NANDF_Read(BSP_VOID* pRamAddr, BSP_U32 u32FlashAddr, BSP_U32 u32NumByte);
/******************************************************************************
* Function     :   BSP_NANDF_Write
*
* Description  :   将指定地址和长度内容到写到指定FLASH地址中
*
* Input        :   pRamAddr     写操作源地址
*              :   u32NumByte   数据长度，单位为字节
*
* Output       :   u32FlashAddr 写操作目的地址
*
* return       :   写操作成功与否
******************************************************************************/
BSP_S32 BSP_NANDF_Write(BSP_U32 u32FlashAddr, BSP_VOID *pRamAddr,  BSP_U32 u32NumByte);
/******************************************************************************
* Function     :   BSP_NANDF_Erase
*
* Description  :   擦除指定Flash地址所在块
*
* Input        :   u32address   要擦除块的ID
*
* Output       :   无
*
* return       :   擦除操作成功与否
******************************************************************************/
BSP_S32 BSP_NANDF_Erase(BSP_U32 u32BlockID);
/*************************NANDF END************************/

/*************************OM BEGIN************************/
typedef struct tagBSP_OM_NET_S
{
    BSP_U32 u32NetRxStatOverFlow;       /* 接收FIFO溢出统计计数 */
    BSP_U32 u32NetRxStatPktErr;         /* 接收总错包计数 */
    BSP_U32 u32NetRxStatCrcErr;         /* 接收CRC错包计数 */
    BSP_U32 u32NetRxStatLenErr;         /* 接收无效长度包计数 */
    BSP_U32 u32NetRxNoBufInt;           /* 接收没有BUFFER中断计数 */
    BSP_U32 u32NetRxStopInt;            /* 接收停止中断计数 */
    BSP_U32 u32NetRxDescErr;            /* 接收描述符错误 */

    BSP_U32 u32NetTxStatUnderFlow;      /* 发送FIFO下溢统计计数 */
    BSP_U32 u32NetTxUnderFlowInt;       /* 发送FIFO下溢中断计数 */
    BSP_U32 u32NetTxStopInt;            /* 发送停止中断计数 */
    BSP_U32 u32NetTxDescErrPs;          /* 发送描述符错误(Ps) */
    BSP_U32 u32NetTxDescErrOs;          /* 发送描述符错误(Os) */
    BSP_U32 u32NetTxDescErrMsp;         /* 发送描述符错误(Msp) */

    BSP_U32 u32NetFatalBusErrInt;      /* 总线错误*/
}BSP_OM_NET_S;

typedef enum{
     ADDRTYPE8BIT,
     ADDRTYPE16BIT,
     ADDRTYPE32BIT
}ENADDRTYPE;

typedef enum{
     MEMCHKTYPE16BIT,
     MEMCHKTYPE32BIT
}ENMEMCHKTYPE;


typedef enum{
     BOARD_TYPE_LTE_ONLY,
     BOARD_TYPE_GUL
}BOARD_TYPE_E;

/*****************************************************************************
* 函 数 名  : BSP_OM_GetFlashSpec
*
* 功能描述  : 获取FLASH规格
*
* 输入参数  : 无
*
* 输出参数  : BSP_U32 *pu32FlashTotalSize  : FLASH总大小
*             BSP_U32 *pu32FlashBlkSize     : FLASH块大小
*             BSP_U32 *pu32FlashPageSize    : FLASH页大小
*
* 返 回 值  : BSP_ERROR:获取失败
*             BSP_OK:获取成功
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 BSP_OM_GetFlashSpec(BSP_U32 *pu32FlashTotalSize, BSP_U32 *pu32FlashBlkSize, BSP_U32 *pu32FlashPageSize);
/*****************************************************************************
* 函 数 名  : BSP_OM_GetVerTime
*
* 功能描述  : 获得ver时间
*
* 输入参数  : 无
*
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_VOID BSP_OM_GetVerTime(BSP_S8* strVerTime);

/******************************************************************************
* Function     :   BSP_OM_RegRead
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_RegRead(BSP_U32 u32RegAddr, ENADDRTYPE enAddrType, BSP_U32 *pu32Value);
/******************************************************************************
* Function     :   BSP_OM_RegWrite
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_RegWrite(BSP_U32 u32RegAddr, ENADDRTYPE enAddrType, BSP_U32 u32Value);
/******************************************************************************
* Function     :   BSP_OM_MemRead
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_MemRead(BSP_U32 u32MemAddr, ENADDRTYPE enAddrType, BSP_U32 *pu32Value);
/******************************************************************************
* Function     :   BSP_OM_MemWrite
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_MemWrite(BSP_U32 u32MemAddr, ENADDRTYPE enAddrType, BSP_U32 u32Value);
/******************************************************************************
* Function     :   BSP_OM_MemNWrite
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_MemNWrite(BSP_U32 u32MemAddr, BSP_U32 u32Value, BSP_U32 u32Count);
/******************************************************************************
* Function     :   BSP_OM_MemPrint
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_MemPrint(BSP_U32 u32MemAddr,ENADDRTYPE enAddrType);
/******************************************************************************
* Function     :   BSP_OM_MemChk
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_MemChk(BSP_U32 u32StartAddr, BSP_U32 u32EndAddr, ENMEMCHKTYPE enChkType);
/******************************************************************************
* Function     :   BSP_OM_GetVersion
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_VOID BSP_OM_GetVersion();
/******************************************************************************
* Function     :   BSP_OM_SoftReboot
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_VOID BSP_OM_SoftReboot();
/******************************************************************************
* Function     :   BSP_OM_GetFPGAVer
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_GetFPGAVer();
/******************************************************************************
* Function     :   BSP_OM_NET
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_VOID BSP_OM_NET(BSP_OM_NET_S *pstNetOm);
/******************************************************************************
* Function     :   BSP_OM_ShellLock
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_ShellLock(BSP_BOOL bRequest);

/*****************************************************************************
* 函 数 名  : BSP_OM_GetBoardType
*
* 功能描述  : 获取单板类型
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP单板类型枚举
*
* 其它说明  : 无
*
*****************************************************************************/
BOARD_TYPE_E BSP_OM_GetBoardType(void);

/*************************OM END************************/

/*************************GMAC BEGIN************************/
/* 上行线路收包函数指针 */
typedef BSP_VOID (*UpLinkRxFunc)(BSP_U8 *buf, BSP_U32 len);

/* 包封装释放函数指针 */
typedef BSP_VOID (*FreePktEncap)(BSP_VOID *PktEncap);

/* GMAC调用者枚举 */
typedef enum tagGMAC_OWNER_E
{
    GMAC_OWNER_VXWORKS = 0, /* Vxworks网络协议栈 */
    GMAC_OWNER_PS,          /* LTE网络协议栈 */
    GMAC_OWNER_MSP,         /* MSP */
    GMAC_OWNER_MAX          /* 边界值 */
}GMAC_OWNER_E;

/******************************************************************************
* Function     :   BSP_GMAC_PowerDown
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_GMAC_PowerDown(BSP_VOID);
/******************************************************************************
* Function     :   BSP_GMAC_PowerUp
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_GMAC_PowerUp(BSP_VOID);
/******************************************************************************
* Function     :   BSP_GMAC_RegisterUpLinkRxCb
*
* Description  :   注册上行链路接收回调函数
*
* Input        :  GMAC_OWNER_E eOwner,     调用者
                  UpLinkRxFunc pfunc       上行链路接收回调函数
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_GMAC_RegisterUpLinkRxCb(GMAC_OWNER_E eOwner,   UpLinkRxFunc pfunc );
/******************************************************************************
* Function     :   BSP_GMAC_RegisterFreePktEncapCb
*
* Description  :    注册包封装释放回调函数
*
* Input        :  GMAC_OWNER_E eOwner,     调用者
                  FreePktEncap pfunc      包封装释放回调函数
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_GMAC_RegisterFreePktEncapCb( GMAC_OWNER_E eOwner, FreePktEncap pfunc );
/******************************************************************************
* Function     :   BSP_GMAC_SetPktEncapInfo
*
* Description  :    设置包封装结构信息
*
* Input        :  GMAC_OWNER_E eOwner,    调用者
                  BSP_S32 s32BufOft,      buf偏移量
                  BSP_S32 s32LenOft,      len偏移量
                  BSP_S32 s32NextOft      next偏移量
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_GMAC_SetPktEncapInfo(GMAC_OWNER_E eOwner, BSP_S32 s32BufOft, BSP_S32 s32LenOft,BSP_S32 s32NextOft);
/******************************************************************************
* Function     :   BSP_GMAC_DownLinkTx
*
* Description  :    下行数据发送函数
*
* Input        :  GMAC_OWNER_E eOwner,    调用模块
                  BSP_VOID *pPktEncap     包封装首地址
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_GMAC_DownLinkTx(GMAC_OWNER_E eOwner, BSP_VOID *pPktEncap );
/*************************GMAC END************************/

/*************************CCID BEGIN************************/
#define _DEBUG_JUNGO_CCID

typedef BSP_U32     ICC_HANDLE;
#define ICC_INVALID_HANDLE              (-1)

#define ICC_EVENT_INSERT                0x0
#define ICC_EVENT_REMOVE                0x1

#define ICC_HW_ERROR_OVERCURRENT        0x1

#define ICC_TRANS_PROTOCOL_T0           0x0
#define ICC_TRANS_PROTOCOL_T1           0x1

#define ICC_TRANS_LEVEL_CHAR_FRAME      0x0
#define ICC_TRANS_LEVEL_TPDU            0x1
#define ICC_TRANS_LEVEL_SHORT_APDU      0x2
#define ICC_TRANS_LEVEL_EXTENDED_APDU   0x4

#define ICC_VOL_CLASS_A                 0x0
#define ICC_VOL_CLASS_B                 0x1
#define ICC_VOL_CLASS_C                 0x2

#define ICC_AUTO_PARAM_CFG              0x2
#define ICC_AUTO_ACTIVATION             0x4
#define ICC_AUTO_SELECT_VOL             0x8
#define ICC_AUTO_CHANGE_FREQ            0x10
#define ICC_AUTO_CHANGE_BAUD_RATE       0x20
#define ICC_AUTO_PARAM_NEGOTIATION      0x40
#define ICC_AUTO_PARAM_PPS              0x80
#define ICC_AUTO_STOP_CLOCK             0x100

#define ICC_ACTIVE                      0x0
#define ICC_PRESENT                     0x1
#define ICC_ABSENT                      0x2

#define ICC_CLOCK_STATE_RUNNING         0x0
#define ICC_CLOCK_STATE_STOP_L          0x1
#define ICC_CLOCK_STATE_STOP_H          0x2
#define ICC_CLOCK_STATE_STOP_UNKNOW     0x3


#define ICC_HANDLE_NONE                 0x0
#define ICC_HANDLE_DATA_EXCHANGE        0x1
#define ICC_HANDLE_ABORT                0x2
#define ICC_HANDLE_ACTIVE               0x3
#define ICC_HANDLE_DEACTIVE             0x4


/*错误码*/
#define BSP_ERR_CCID_NOTINIT             951

typedef struct
{
    BSP_U32         u32VoltageLevel;
    BSP_U32         u32TransProtocol;
    BSP_U32         u32TransLevel;
    BSP_U32         u32Automation;
    BSP_U32         u32ClkFrq;
    BSP_U32         u32BaudRate;
    BSP_BOOL        bReactive;
}BSP_CCID_capability_t;


typedef struct
{

    /* reset icc */
    BSP_U32 (*bsp_ccid_active_cb)(BSP_U8 u8Seq);

    /* deactive icc */
    BSP_U32 (*bsp_ccid_deactive_cb)();

    /* change voltage class */
    BSP_U32 (*bsp_ccid_change_vol_cb)();

    /* get FI DI*/
    BSP_U32 (*bsp_ccid_get_fi_di_cb)(BSP_U8 *pu8FI, BSP_U8 *pu8DII);

    /* get icc state */
    BSP_U32 (*bsp_ccid_get_state_cb)(BSP_U32 *pu32IccState, BSP_U32 *pu32ClockState);

    /* send apdu */
    BSP_U32 (*bsp_ccid_send_apdu_cb)(BSP_U8 u8Seq, BSP_U32 u32DateLen, BSP_U8 *pu8Data);

    /* send tpdu */
    BSP_U32 (*bsp_ccid_send_tpdu_cb)(BSP_U8 u8Seq, BSP_U32 u32DateLen, BSP_U8 *pu8Data);

    /* send character bytes */
    BSP_U32 (*bsp_ccid_send_char_cb)(BSP_U8 u8Seq, BSP_U32 u32DateLen, BSP_U8 *pu8Data);

    /* abort command */
    BSP_U32 (*bsp_ccid_abort_cb)();

} BSP_CCID_callbacks_t;

/*****************************************************************************
* 函 数 名  : BSP_CCID_StatusChange
*
* 功能描述  : ICC管理模块使用此接口注册到CCID，CCID借助于回调函数取得ICC的访问权
*
* 输入参数  : handle     设备句柄
*             u32Event   ICC的状态变化
*                ICC_EVENT_INSERT    卡插入
*                ICC_EVENT_REMOVE    卡移出
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2010.04.19   z67193  create
*
*****************************************************************************/
BSP_VOID BSP_CCID_StatusChange(ICC_HANDLE handle);


/*****************************************************************************
* 函 数 名  : BSP_CCID_HardwareError
*
* 功能描述  : ICC管理模块使用此接口注册到CCID，CCID借助于回调函数取得ICC的访问权
*
* 输入参数  : handle     设备句柄
*             u32Error   ICC的硬件错误
*                ICC_HW_ERROR_OVERCURRENT    电流过载
*
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2010.04.19   z67193  create
*
*****************************************************************************/
BSP_VOID BSP_CCID_HardwareError(ICC_HANDLE handle, BSP_U32 u32Error);


/*****************************************************************************
* 函 数 名  : BSP_CCID_HandleFinished
*
* 功能描述  : ICC管理模块使用此接口注册到CCID，CCID借助于回调函数取得ICC的访问权
*
* 输入参数  : handle     设备句柄
*             u8Type     请求类型
*             u8Seq      请求序列号
*             u32Result  处理结果
*             u32Len     返回数据长度
*             pu8Data    返回数据
*
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2010.04.19   z67193  create
*
*****************************************************************************/
BSP_U32 BSP_CCID_Complete(ICC_HANDLE handle, BSP_U8 u8Type, BSP_U8 u8Seq,
                          BSP_U32 u32Result, BSP_U32 u32Len, BSP_U8 *pu8Data);


/*****************************************************************************
* 函 数 名  : BSP_CCID_HandleDelay
*
* 功能描述  : ICC管理模块使用此接口注册到CCID，CCID借助于回调函数取得ICC的访问权
*
* 输入参数  : handle     设备句柄
*             u8Type     请求类型
*
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2010.04.19   z67193  create
*
*****************************************************************************/
BSP_U32 BSP_CCID_HandleDelay(ICC_HANDLE handle, BSP_U8 u8Type);
/*************************CCID END************************/

/*************************FTP BEGIN************************/
/**************************************************************************
  宏定义
**************************************************************************/

/**************************************************************************
  错误码定义
**************************************************************************/
#define BSP_ERR_FTP_NOT_INIT            BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_MODULE_NOT_INITED)    /*没有初始化*/
#define BSP_ERR_FTP_NULL_PTR            BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_NULL_PTR)             /*参数错误*/
#define BSP_ERR_FTP_INVALID_PARAM       BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_INVALID_PARA)         /*内存空间分配失败*/
#define BSP_ERR_FTP_NO_MEM              BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_BUF_ALLOC_FAILED)     /*循环次数达到最大*/

#define BSP_ERR_FTP_NET                 BSP_DEF_ERR(BSP_MODU_FTP, 0x10)                         /*网络错误*/
#define BSP_ERR_FTP_XFER                BSP_DEF_ERR(BSP_MODU_FTP, 0x11)                         /*FTP建议传输失败*/
#define BSP_ERR_FTP_DATA_TRANSFER       BSP_DEF_ERR(BSP_MODU_FTP, 0x12)                         /*数据传输失败*/
#define BSP_ERR_FTP_CMD                 BSP_DEF_ERR(BSP_MODU_FTP, 0x13)                         /*命令发送失败*/
#define BSP_ERR_FTP_READ                BSP_DEF_ERR(BSP_MODU_FTP, 0x14)                         /*读失败*/
#define BSP_ERR_FTP_WRITE               BSP_DEF_ERR(BSP_MODU_FTP, 0x15)                         /*写失败*/

/**************************************************************************
  枚举定义
**************************************************************************/

/*FTP transport type*/
typedef enum tagFTP_LOAD_TYPE_E
{
    FTP_LOADTYPE_VXWORKS_IMAGE = 0,
	FTP_LOADTYPE_TENCILICA_TEXT,
	FTP_LOADTYPE_TENCILICA_BOOTCODE,
    FTP_LOADTYPE_BOOTSECT,
    FTP_LOADTYPE_COMMONFILE,
    FTP_LOADTYPE_IMPORT,
    FTP_LOADTYPE_IPCFG,
    FTP_LOADTYPE_WITHSPARE,
    FTP_LOADTYPE_BUTTOM
} FTP_LOAD_TYPE_E;


/*****************************************************************************
* 函 数 名  : LOADF
*
* 功能描述  :
*
* 输入参数  : BSP_U8 * pu8FileName, BSP_U32 u32DestAddr, FTP_LOAD_TYPE_E enLoadType
*
* 输出参数  : 无
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 LOADF(BSP_U8 * pu8FileName, BSP_U32 u32DestAddr, FTP_LOAD_TYPE_E enLoadType);

/*****************************************************************************
* 函 数 名  : LOADB
*
* 功能描述  :
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 LOADB();

/*****************************************************************************
* 函 数 名  : BSP_LoadDsp
*
* 功能描述  :
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 BSP_LoadDsp();

/*****************************************************************************
* 函 数 名  : BSP_RunDsp
*
* 功能描述  :
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_VOID BSP_RunDsp();

/*****************************************************************************
* 函 数 名  : BSP_BootDsp
*
* 功能描述  :
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 BSP_BootDsp();

/*************************FTP END************************/

/*************************ICC BEGIN************************/
/**************************************************************************
  宏定义
**************************************************************************/
#define ICC_CHAN_NUM_MAX            (32)

#define ICC_CHAN_PRIORITY_HIGH      (0)
#define ICC_CHAN_PRIORITY_LOW       (255)
#define ICC_CHAN_PRIORITY_DEFAULT   ICC_CHAN_PRIORITY_LOW

#define ICC_CHAN_FIFO_MIN           (32)
#define ICC_CHAN_FIFO_MAX           (65536)
#define ICC_CHAN_FIFO_SIZE1         (1024)
#define ICC_CHAN_FIFO_SIZE2         (2048)
#define ICC_CHAN_FIFO_SIZE3         (4096)

/**************************************************************************
  结构定义
**************************************************************************/
typedef enum tagICC_CHAN_STATE_E
{
    ICC_CHAN_STATE_CLOSED   = 0,
    ICC_CHAN_STATE_OPENED,
    ICC_CHAN_STATE_CLOSEING,
    ICC_CHAN_STATE_BUTT
}ICC_CHAN_STATE_E;

typedef enum tagICC_CHAN_MODE_E{
    ICC_CHAN_MODE_STREAM    = 0,
    ICC_CHAN_MODE_PACKET,
    ICC_CHAN_MODE_BUTT
}ICC_CHAN_MODE_E;

typedef enum tagICC_IOCTL_CMD_E{
    ICC_IOCTL_SET_WRITE_CB  = 0,
    ICC_IOCTL_SET_READ_CB,
    ICC_IOCTL_SET_EVENT_CB,
    ICC_IOCTL_GET_STATE,
    ICC_IOCTL_SET_BUTT
}ICC_IOCTL_CMD_E;


typedef enum tagICC_EVENT_E{
    ICC_EVENT_CLOSE         = 0,
    ICC_EVENT_RESET,
    ICC_EVENT_BUTT
}ICC_EVENT_E;


typedef BSP_U32 (*icc_event_cb)(BSP_U32 u32ChanID, BSP_U32 u32Event, BSP_VOID* Param);
typedef BSP_U32 (*icc_write_cb)(BSP_U32 u32ChanID);
typedef BSP_U32 (*icc_read_cb)(BSP_U32 u32ChanID, BSP_S32 u32Size);

typedef struct tagICC_CHAN_ATTR_S{
    BSP_U32             u32FIFOInSize;
    BSP_U32             u32FIFOOutSize;
    BSP_U32             u32Priority;
    ICC_CHAN_MODE_E     enChanMode;
    BSP_U32             u32TimeOut;
    icc_event_cb        event_cb;
    icc_write_cb        write_cb;
    icc_read_cb         read_cb;
}ICC_CHAN_ATTR_S;

/**************************************************************************
  函数声明
**************************************************************************/
BSP_S32 BSP_ICC_Open(BSP_U32 u32ChanId, ICC_CHAN_ATTR_S *pChanAttr);
BSP_S32 BSP_ICC_Close(BSP_U32 u32ChanId);
BSP_S32 BSP_ICC_Write(BSP_U32 u32ChanId, BSP_U8* pData, BSP_S32 s32Size);
BSP_S32 BSP_ICC_Read(BSP_U32 u32ChanId, BSP_U8* pData, BSP_S32 s32Size);
BSP_S32 BSP_ICC_Ioctl(BSP_U32 u32ChanId, BSP_U32 cmd, BSP_VOID *param);


/**************************************************************************
  错误码定义
**************************************************************************/
#define BSP_ERR_ICC_BASE                (BSP_S32)(0x80000000 | (BSP_DEF_ERR(BSP_MODU_ICC, 0)))
#define BSP_ERR_ICC_NOT_INIT            (BSP_ERR_ICC_BASE + 0x1)
#define BSP_ERR_ICC_NOT_CONNECT         (BSP_ERR_ICC_BASE + 0x2)
#define BSP_ERR_ICC_NULL                (BSP_ERR_ICC_BASE + 0x3)
#define BSP_ERR_ICC_NO_MEM              (BSP_ERR_ICC_BASE + 0x4)
#define BSP_ERR_ICC_INVALID_CHAN        (BSP_ERR_ICC_BASE + 0x5)
#define BSP_ERR_ICC_INVALID_PARAM       (BSP_ERR_ICC_BASE + 0x6)
#define BSP_ERR_ICC_EXIST               (BSP_ERR_ICC_BASE + 0x7)
#define BSP_ERR_ICC_NOT_EXIST           (BSP_ERR_ICC_BASE + 0x8)
#define BSP_ERR_ICC_OPENED              (BSP_ERR_ICC_BASE + 0x9)
#define BSP_ERR_ICC_NOT_OPEN            (BSP_ERR_ICC_BASE + 0xa)
#define BSP_ERR_ICC_NO_SEM              (BSP_ERR_ICC_BASE + 0xb)
#define BSP_ERR_ICC_NEGOTIATE_FAIL      (BSP_ERR_ICC_BASE + 0xc)
#define BSP_ERR_ICC_BUFFER_FULL         (BSP_ERR_ICC_BASE + 0xd)
#define BSP_ERR_ICC_BUFFER_EMPTY        (BSP_ERR_ICC_BASE + 0xe)
#define BSP_ERR_ICC_INVALID_PACKET      (BSP_ERR_ICC_BASE + 0xf)
#define BSP_ERR_ICC_PACKET_SIZE         (BSP_ERR_ICC_BASE + 0x10)
#define BSP_ERR_ICC_USER_BUFFER_SIZE    (BSP_ERR_ICC_BASE + 0x11)
/*************************ICC END************************/

/*************************IPM START************************/
/**************************************************************************
  宏定义
**************************************************************************/

#define MAC_HEADER_LEN      14
#define IPM_SUCCESS                       BSP_OK
#define IPM_ERROR                         BSP_ERROR

/**************************************************************************
  STRUCT定义
**************************************************************************/

/* 核间结点条目域结构体 */
typedef struct tagIPM_DESC_ITEM_S
{
	BSP_U32 Addr;
	BSP_U16 Len;
	BSP_U16 Invalid;  /* 0表示配置BD,1表示不配BD */
} IPM_DESC_ITEM_S;

/* 核间描述结点结构体 */
typedef struct tagIPM_DESC_NODE_S
{
    BSP_U32 StartAddr;
    BSP_U16 ItemNum;  /* 总的Item数目 */
    BSP_U16 TailIdx;  /* 最后一个有效的Item ID, 从0开始 */
    BSP_U16 ValidNum; /* 有效的Item数目 */
    IPM_DESC_ITEM_S *pstDescItem;
}IPM_DESC_NODE_S;



/**************************************************************************
  UNION定义
**************************************************************************/

/**************************************************************************
  OTHERS定义
**************************************************************************/


/**************************************************************************
  函数声明
**************************************************************************/

unsigned long USB_ETH_DrvSetHostAssembleParam(unsigned long ulHostOutTimeout);
unsigned long USB_ETH_DrvSetDeviceAssembleParam(unsigned long ulEthTxMinNum,
										  unsigned long ulEthTxTimeout,
										  unsigned long ulEthRxMinNum,
										  unsigned long ulEthRxTimeout);


/******************************IP数据管理模块的接口***************************/
/*****************************************************************************
* 函 数 名  : BSP_IPM_GetDescNode
*
* 功能描述  : 获取信息结点指针接口
*
* 输入参数  : BSP_U32 u32ItemNum    IP包的数量
*
* 输出参数  : 无
* 返 回 值  : 空 --- 获取失败
*             非空 ---  获取的信息结点指针
*
* 修改记录  :2011年1月27日   鲁婷  创建
*****************************************************************************/
IPM_DESC_NODE_S *BSP_IPM_GetDescNode(BSP_U32 u32ItemNum);

/*****************************************************************************
* 函 数 名  : BSP_IPM_FreeDescNode
*
* 功能描述  : 释放信息结点指针接口
*
* 输入参数  : IPM_DESC_NODE_S *pstDescNode
*             BSP_BOOL bNodeOnly
*             true只释放结点      false 释放结点和组织内存
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月27日   鲁婷  创建
*****************************************************************************/
BSP_S32 BSP_IPM_FreeDescNode(IPM_DESC_NODE_S *pstDescNode, BSP_BOOL bNodeOnly);

/*****************************************************************************
* 函 数 名  : BSP_IPM_GetBspBuf
*
* 功能描述  : 申请内存接口
*
* 输入参数  : BSP_U32 u32len    需要申请的buf长度
*
* 输出参数  : 无
* 返 回 值  : 获取的buf指针
*
* 修改记录  :2011年1月27日   鲁婷  创建
*****************************************************************************/
BSP_U8 *BSP_IPM_GetBspBuf(BSP_U32 u32len);

/*****************************************************************************
* 函 数 名  : BSP_IPM_FreeBspBuf
*
* 功能描述  : 释放内存接口
*
* 输入参数  : BSP_U8 *pBuf 需要释放的指针
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月27日   鲁婷  创建
*****************************************************************************/
BSP_VOID BSP_IPM_FreeBspBuf(BSP_U8 *pBuf);

/*************************IPM END************************/


/*************************IPF BEGIN************************/
/**************************************************************************
  宏定义
**************************************************************************/



/**************************************************************************
  枚举定义
**************************************************************************/


/**************************************************************************
  STRUCT定义
**************************************************************************/


#if 0
/* TTF结构体 */
/* below for debug structure */
enum TTF_BLK_MEM_STATE_ENUM
{
    TTF_BLK_MEM_STATE_FREE,       /*该内存未申请过或已释放*/
    TTF_BLK_MEM_STATE_ALLOC,      /*该内存已申请*/
    TTF_BLK_MEM_STATE_BUTT
};
typedef unsigned int TTF_BLK_MEM_STATE_ENUM_UINT32;

typedef struct
{
    TTF_BLK_MEM_STATE_ENUM_UINT32   enMemStateFlag;
    unsigned int                    ulAllocTick;        /* CPU tick when alloc or free */
    unsigned int                    ulAllocFileID;      /* File ID when alloc or free */
    unsigned int                    ulAllocLineNum;     /* File Line when alloc or free */
    unsigned int                    ulTraceFileID;      /* File ID when traced */
    unsigned int                    ulTraceLineNum;     /* File Line when traced */
    unsigned int                    ulTraceTick;        /* CPU tick when traced */
} TTF_BLK_MEM_DEBUG_ST;

typedef struct
{
    unsigned int    aulGroupCnt[4];    /* 将 TTF_BLK_MEM_CNT 片 TTF_MEM 内存均分为 4 份,
                                        分别记录当前块数落在 [0, TTF_BLK_MEM_CNT/4-1],
                                        [TTF_BLK_MEM_CNT/4, TTF_BLK_MEM_CNT/2-1],
                                        [TTF_BLK_MEM_CNT/2, 3*TTF_BLK_MEM_CNT/4-1],
                                        [3*TTF_BLK_MEM_CNT/4, TTF_BLK_MEM_CNT] 范围的次数 */
    unsigned int    ulMaxCnt;    /* 记录使用的 TTF_MEM 峰值 */
} TTF_BLK_MEM_CNT_ST;

typedef struct _TTF_NODE_ST
{
    struct _TTF_NODE_ST    *pNext;
    struct _TTF_NODE_ST    *pPrev;
} TTF_NODE_ST;

typedef struct _TTF_MEM_ST
{
    TTF_NODE_ST                     stNode;
    unsigned long                   ulPoolId;    /*本内存属于哪一个内存池 */
    unsigned long                   ulClusterId;  /*本内存是属于哪一个级别*/
    struct _TTF_MEM_ST              *pNext;       /* 该数据的下一段 */
    unsigned short                  usType;      /* 内存类型，DYN、BLK、EXT类型 */
    unsigned short                  usLen;       /* 申请数据的总长度，但不一定全部被使用 */
    unsigned short                  usUsed;      /* 已经使用的数据长度   */
    unsigned short                  usApp;       /* 使用者可以使用的区域 */

    TTF_BLK_MEM_DEBUG_ST           *pstDbgInfo;

    unsigned char                  *pOrigData;          /* 记录数据的原始指针 */
    unsigned char                  *pData;          /* 存放数据的指针，物理上指向结构体的连续内存 */
    void                           *pExtBuffAddr;    /* 保存extern类型的内存地址用于释放 */
    unsigned long                   ulForLte[6];     /*预留给LTE使用，GU模不用初始化*/
} TTF_MEM_ST;
#endif

/*升级Flash信息*/
typedef struct
{
    unsigned int ulblockCount;    /*Block个数*/
    unsigned int ulpageSize;    /*page页大小*/
    unsigned int ulpgCntPerBlk;    /*一个Block中的page个数*/
}DLOAD_FLASH_STRU;


/**************************************************************************
  UNION定义
**************************************************************************/

/**************************************************************************
  OTHERS定义
**************************************************************************/



/**************************************************************************
  函数声明
**************************************************************************/



/*************************IPF END************************/



/*************************IFC BEGIN************************/
/* 错误码*/
/*专用错误码*/
#define BSP_ERR_IFC_SEND_FAIL         BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 1)) /*0x10120001*/
#define BSP_ERR_IFC_SEND_TIMEOUT      BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 2)) /*0x10120002*/
#define BSP_ERR_IFC_RCV_FAIL          BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 3)) /*0x10120003*/

#define IFC_ICC_CHAN_IN_FIFOSIZE  1024
#define IFC_ICC_CHAN_OUT_FIFOSIZE 1024
/*IFC发送最大长度*/
#define BSP_IFC_SEND_MAX_LEN  IFC_ICC_CHAN_IN_FIFOSIZE
/* 模块ID枚举*/
typedef enum tagIFC_MODULE_E
{
    IFC_MODULE_BSP=  0,
    IFC_MODULE_LPS=  1,
    IFC_MODULE_GUPS= 2,
    IFC_MODULE_LMSP = 3,
    IFC_MODULE_GUOM = 4,
    IFC_MODULE_BUTT
} IFC_MODULE_E;

/* IFC回调函数结构*/
/* pMsgBody:该函数ID对应函数的参数,前四字节需保证为moduleId*/
/* u32Len:pMsgBody长度*/
typedef BSP_S32 (*BSP_IFC_REG_FUNC)(BSP_VOID *pMsgBody,BSP_U32 u32Len);

/*****************************************************************************
* 函 数 名  : BSP_IFC_RegFunc
*
* 功能描述  : 注册IFC回调函数接口
*
* 输入参数  : IFC_MODULE_E enModuleId    模块ID
*             BSP_IFC_REG_FUNC pFunc     回调函数指针
* 输出参数  : 无
*
* 返 回 值  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_U32 BSP_IFC_RegFunc(IFC_MODULE_E enModuleId, BSP_IFC_REG_FUNC pFunc);


/*****************************************************************************
* 函 数 名  : BSP_IFC_Send
*
* 功能描述  : IFC发送消息接口
*
* 输入参数  : BSP_VOID * pMspBody           发送消息内容
*             BSP_U32 u32Len               pMspBody的长度
* 输出参数  : 无
*
* 返 回 值  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*             BSP_ERR_IFC_SEND_FAIL
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_U32 BSP_IFC_Send(BSP_VOID * pMspBody, BSP_U32 u32Len);

/*************************IFC END************************/

/*************************UDI BEGIN************************/
/**************************************************************************
  宏定义
**************************************************************************/
#define UDI_INVALID_HANDLE     (-1)

#define UDI_CAPA_BLOCK_READ    ((BSP_U32)(1<<0)) /* 该设备的read接口为阻塞接口 */
#define UDI_CAPA_BLOCK_WRITE   ((BSP_U32)(1<<1)) /* 该设备的write接口为阻塞接口 */
#define UDI_CAPA_READ_CB       ((BSP_U32)(1<<2)) /* 该设备支持read 回调函数 */
#define UDI_CAPA_WRITE_CB      ((BSP_U32)(1<<3)) /* 该设备支持write 回调函数 */
#define UDI_CAPA_BUFFER_LIST   ((BSP_U32)(1<<4)) /* 该设备的读写buffer为内存链表结构(默认为普通内存块) */
#define UDI_CAPA_CTRL_OPT      ((BSP_U32)(1<<5)) /* 该设备支持控制操作 */


/**************************************************************************
  结构定义
**************************************************************************/
#define UDI_BUILD_DEV_ID(dev, type) (((BSP_U32)(dev) << 8) | ((BSP_U32)(type) & 0x00ff))
#define UDI_BUILD_CMD_ID(dev, cmd) (((BSP_U32)(dev) << 8) | ((BSP_U32)(cmd) & 0xffff))


typedef BSP_S32 UDI_HANDLE;

/* 主设备定义 */
typedef enum tagUDI_DEVICE_MAIN_ID
{
    UDI_DEV_USB_ACM = 0,
    UDI_DEV_USB_NCM,
    UDI_DEV_ICC,
    UDI_DEV_UART,
    UDI_DEV_HSUART,

    UDI_DEV_MAX                 /* 必须在最后, 用于边界值 */
}UDI_DEVICE_MAIN_ID;

/* 各设备类型定义(要和 usb 多设备形态统一起来) */
/* ACM */
typedef enum tagUDI_ACM_DEV_TYPE
{
    UDI_USB_ACM_CTRL,
    UDI_USB_ACM_AT,
    UDI_USB_ACM_SHELL,
    UDI_USB_ACM_LTE_DIAG,
    UDI_USB_ACM_OM,
    UDI_USB_ACM_MODEM,
    UDI_USB_ACM_HSIC_MODEM,
    UDI_USB_ACM_GPS,      /*HISO*/
    UDI_USB_ACM_3G_GPS,   /*ashell*/
    UDI_USB_ACM_3G_PCVOICE, /*预留*/
    UDI_USB_ACM_PCVOICE,    /*预留*/
    /*UDI_USB_ACM_3G_AT,    //3G AT*/
    /*UDI_USB_ACM_3G_MODEM, //3G PPP*/
    UDI_USB_HSIC_ACM0,
    UDI_USB_HSIC_ACM1,
    UDI_USB_HSIC_ACM2,
    UDI_USB_HSIC_ACM3,
    UDI_USB_HSIC_ACM4,
    UDI_USB_HSIC_ACM5,
    UDI_USB_HSIC_ACM6,
    UDI_USB_HSIC_ACM7,
    UDI_USB_HSIC_ACM8,
    UDI_USB_HSIC_ACM9,
    UDI_USB_HSIC_ACM10,
    UDI_USB_HSIC_ACM11,
    UDI_USB_HSIC_ACM12,
    UDI_USB_HSIC_ACM13,
	UDI_USB_HSIC_ACM14,
    UDI_USB_ACM_MAX                /* 必须在最后, 用于边界值 */
}UDI_ACM_DEV_TYPE;

/* NCM */
typedef enum tagUDI_NCM_DEV_TYPE
{
    UDI_USB_NCM_NDIS,
    UDI_USB_NCM_CTRL,
    UDI_USB_HSIC_NCM0,
    UDI_USB_HSIC_NCM1,
    UDI_USB_HSIC_NCM2,

    UDI_USB_NCM_MAX                /* 必须在最后, 用于边界值 */
}UDI_NCM_DEV_TYPE;

/* ICC */
typedef enum tagUDI_ICC_DEV_TYPE
{
    UDI_ICC_GUOM0 = 16,
    UDI_ICC_GUOM1,
    UDI_ICC_GUOM2,
    UDI_ICC_GUOM3,
    UDI_ICC_GUOM4,

    UDI_ICC_MAX                /* 必须在最后, 用于边界值 */
}UDI_ICC_DEV_TYPE;

/* 设备ID号定义 */
typedef enum tagUDI_DEVICE_ID
{
    /* USB ACM */
    UDI_ACM_CTRL_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_CTRL),
    UDI_ACM_AT_ID =    UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_AT),
    UDI_ACM_SHELL_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_SHELL),
    UDI_ACM_LTE_DIAG_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_LTE_DIAG),
    UDI_ACM_OM_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_OM),
    UDI_ACM_MODEM_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_MODEM),
    UDI_ACM_HSIC_MODEM0_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_HSIC_MODEM),
    UDI_ACM_GPS_ID      = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_GPS),     /*HISO*/
    UDI_ACM_3G_GPS_ID   = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_GPS),  /*ashell*/
    UDI_ACM_3G_PCVOICE_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_PCVOICE), /*预留*/
    UDI_ACM_PCVOICE_ID    = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_PCVOICE),    /*预留*/
    /*UDI_ACM_3G_AT_ID      = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_AT),    //3G AT*/
    /*UDI_ACM_3G_MODEM_ID   = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_MODEM), //3G PPP*/
    UDI_ACM_HSIC_ACM0_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM0),
    UDI_ACM_HSIC_ACM1_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM1),
    UDI_ACM_HSIC_ACM2_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM2),
    UDI_ACM_HSIC_ACM3_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM3),
    UDI_ACM_HSIC_ACM4_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM4),
    UDI_ACM_HSIC_ACM5_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM5),
    UDI_ACM_HSIC_ACM6_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM6),
    UDI_ACM_HSIC_ACM7_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM7),
    UDI_ACM_HSIC_ACM8_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM8),
    UDI_ACM_HSIC_ACM9_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM9),
    UDI_ACM_HSIC_ACM10_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM10),
    UDI_ACM_HSIC_ACM11_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM11),
    UDI_ACM_HSIC_ACM12_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM12),
    UDI_ACM_HSIC_ACM13_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM13),
	UDI_ACM_HSIC_ACM14_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM14),

    /* USB NCM */
    UDI_NCM_NDIS_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_NCM_NDIS),
    UDI_NCM_CTRL_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_NCM_CTRL),
    UDI_NCM_HSIC_NCM0_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_HSIC_NCM0),
    UDI_NCM_HSIC_NCM1_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_HSIC_NCM1),
    UDI_NCM_HSIC_NCM2_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_HSIC_NCM2),

    /* UART */
    UDI_UART_0_ID =  UDI_BUILD_DEV_ID(UDI_DEV_UART, 0),
    UDI_UART_1_ID =  UDI_BUILD_DEV_ID(UDI_DEV_UART, 1),

    /* HSUART */
    UDI_HSUART_0_ID =  UDI_BUILD_DEV_ID(UDI_DEV_HSUART, 0),
    UDI_HSUART_1_ID =  UDI_BUILD_DEV_ID(UDI_DEV_HSUART, 1),

    /* ICC */
    UDI_ICC_IFC_ID  =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, 0),
    UDI_ICC_IPM_ID  =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, 1),
    UDI_ICC_LMSP_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, 2),

    UDI_ICC_GUOM0_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM0),
    UDI_ICC_GUOM1_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM1),
    UDI_ICC_GUOM2_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM2),
    UDI_ICC_GUOM3_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM3),
    UDI_ICC_GUOM4_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM4),

    /* MUST BE LAST */
    UDI_INVAL_DEV_ID = 0xFFFF
} UDI_DEVICE_ID;


/* 设备的打开参数 */
typedef struct tagUDI_OPEN_PARAM
{
    UDI_DEVICE_ID devid;                        /* 设备ID */
    void   *pPrivate;                            /* 模块特有的数据 */
} UDI_OPEN_PARAM;

/* IOCTL 命令码,需要的命令码在此添加 */
typedef enum tagUDI_IOCTL_CMD_TYPE
{
    UDI_IOCTL_SET_WRITE_CB = 0xF001,            /* 设置一个起始码值防止与系统定义冲突 */
    UDI_IOCTL_SET_READ_CB,

    UDI_IOCTL_INVAL_CMD = 0xFFFFFFFF
} UDI_IOCTL_CMD_TYPE;

/**************************************************************************
  函数声明
**************************************************************************/
/*****************************************************************************
* 函 数 名  : udi_get_capability
*
* 功能描述  : 根据设备ID获取当前设备支持的特性
*
* 输入参数  : devId: 设备ID
* 输出参数  : 无
* 返 回 值  : 支持的特性值
*****************************************************************************/
BSP_S32 udi_get_capability(UDI_DEVICE_ID devId);

/*****************************************************************************
* 函 数 名  : udi_open
*
* 功能描述  : 打开设备(数据通道)
*
* 输入参数  : pParam: 设备的打开配置参数
* 输出参数  : 无
* 返 回 值  : -1:失败 / 其他:成功
*****************************************************************************/
UDI_HANDLE udi_open(UDI_OPEN_PARAM *pParam);

/*****************************************************************************
* 函 数 名  : udi_close
*
* 功能描述  : 关闭设备(数据通道)
*
* 输入参数  : handle: 设备的handle
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 udi_close(UDI_HANDLE handle);

/*****************************************************************************
* 函 数 名  : udi_write
*
* 功能描述  : 数据写
*
* 输入参数  : handle:  设备的handle
*             pMemObj: buffer内存 或 内存链表对象
*             u32Size: 数据写尺寸 或 内存链表对象可不设置
* 输出参数  :
*
* 返 回 值  : 完成字节数 或 成功/失败
*****************************************************************************/
BSP_S32 udi_write(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size);

/*****************************************************************************
* 函 数 名  : udi_read
*
* 功能描述  : 数据读
*
* 输入参数  : handle:  设备的handle
*             pMemObj: buffer内存 或 内存链表对象
*             u32Size: 数据读尺寸 或 内存链表对象可不设置
* 输出参数  :
*
* 返 回 值  : 完成字节数 或 成功/失败
*****************************************************************************/
BSP_S32 udi_read(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size);

/*****************************************************************************
* 函 数 名  : udi_ioctl
*
* 功能描述  : 数据通道属性配置
*
* 输入参数  : handle: 设备的handle
*             u32Cmd: IOCTL命令码
*             pParam: 操作参数
* 输出参数  :
*
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 udi_ioctl(UDI_HANDLE handle, BSP_U32 u32Cmd, BSP_VOID* pParam);

/*************************UDI END************************/

/*************************MEMORY BEGIN************************/
/**************************************************************************
  宏定义
**************************************************************************/
/* 内存池类型, Flags标记用 */
typedef enum tagMEM_POOL_TYPE
{
    MEM_NORM_DDR_POOL = 0,
    MEM_ICC_DDR_POOL,
    MEM_ICC_AXI_POOL,
    MEM_POOL_MAX
}MEM_POOL_TYPE;

/**************************************************************************
  接口声明
**************************************************************************/
BSP_VOID* BSP_Malloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags);
BSP_VOID* BSP_MallocDbg(BSP_U32 u32Size, MEM_POOL_TYPE enFlags, BSP_U8* pFileName, BSP_U32 u32Line);
BSP_VOID  BSP_Free(BSP_VOID* pMem);
BSP_VOID  BSP_FreeDbg(BSP_VOID* pMem, BSP_U8* pFileName, BSP_U32 u32Line);

/*****************************************************************************
* 函 数 名  : BSP_MALLOC
*
* 功能描述  : BSP 动态内存分配
*
* 输入参数  : sz: 分配的大小(byte)
*             flags: 内存属性(暂不使用,预留)
* 输出参数  : 无
* 返 回 值  : 分配出来的内存指针
*****************************************************************************/
#ifdef __BSP_DEBUG__
#define BSP_MALLOC(sz, flags) BSP_MallocDbg(sz, flags, __FILE__, __LINE__)
#else
#define BSP_MALLOC(sz, flags) BSP_Malloc(sz, flags)
#endif

/*****************************************************************************
* 函 数 名  : BSP_FREE
*
* 功能描述  : BSP 动态内存释放
*
* 输入参数  : ptr: 动态内存指针
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
#ifdef __BSP_DEBUG__
#define BSP_FREE(ptr) BSP_FreeDbg(ptr, __FILE__, __LINE__)
#else
#define BSP_FREE(ptr) BSP_Free(ptr)
#endif


/*************************MEMORY END************************/


/*****************************************************************************
  1 GU侧驱动对外接口，GUPorting项目添加.
*****************************************************************************/

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define DRV_OK                          (0)
#define DRV_ERROR                       (-1)
#define DRV_INTERFACE_RSLT_OK           (0)

/***************************************************AUDIO模块*********************************************************/
/*AUDIO IOCTL接口命令字*/
#define AUDIO_IOCTL_BASE                   0x30000000
#define IOCTL_AUDIO_SAMPLE_RATE_SET                (AUDIO_IOCTL_BASE+1)
#define IOCTL_AUDIO_PCM_MODE_SET                   (AUDIO_IOCTL_BASE+7)
#define IOCTL_AUDIO_IN_DEV_SELECT                  (AUDIO_IOCTL_BASE+11)
#define IOCTL_AUDIO_OUT_DEV_SELECT                 (AUDIO_IOCTL_BASE+12)
#define IOCTL_AUDIO_OUT_DEV_UNSELECT               (AUDIO_IOCTL_BASE+13)
#define IOCTL_AUDIO_IN_DEV_UNSELECT               (AUDIO_IOCTL_BASE+14)
#define IOCTL_AUDIO_VOICE_OPEN                     (AUDIO_IOCTL_BASE+23)
#define IOCTL_AUDIO_VOICE_CLOSE                    (AUDIO_IOCTL_BASE+24)

/***************************************************MNTN模块*********************************************************/
/*VERSIONINFO_I数据结构中版本字符串最大有效字符长度*/
#define VER_MAX_LENGTH                  30

/*memVersionCtrl接口操作类型*/
#define VERIONREADMODE                  0
#define VERIONWRITEMODE                 1

#define OM_SAVE_EXCHFILE_ING            0
#define OM_SAVE_EXCHFILE_END            1

/***************************************************USB模块*********************************************************/
/* 设备枚举最大端口个数 */
#define DYNAMIC_PID_MAX_PORT_NUM        17

/* 和ACM 定义重复*/
#if 0
/* UDI IOCTL 命令ID */
#define UDI_ACM_IOCTL_SET_READ_CB           0x7F001001
#define UDI_ACM_IOCTL_GET_READ_BUFFER_CB    0x7F001011
#define UDI_ACM_IOCTL_RETUR_BUFFER_CB       0x7F001012
#endif

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/***************************************************AUDIO模块*********************************************************/
typedef enum
{
    NODEV=0x00,
    HEADSET=0x01,
    HANDSET=0x02,
    HANDFREE=0x04
}AUDIO_DEV_TYPE_I;

typedef enum
{
    SAMPLE_RATE_8K=0,
    SAMPLE_RATE_11K025=1,
    SAMPLE_RATE_12K=2,
    SAMPLE_RATE_16K=3,
    SAMPLE_RATE_22K05=4,
    SAMPLE_RATE_24K=5,
    SAMPLE_RATE_32K=6,
    SAMPLE_RATE_44K1=7,
    SAMPLE_RATE_48K=8,
    SAMPLE_RATE_INVALID=0xFFFF
}AUDIO_SAMPLE_RATE_I;

/***************************************************DLOAD模块*********************************************************/
enum UPDATE_STATUS_I
{
    DL_UPDATE = 0,
    DL_NOT_UPDATE = 1
};

/***************************************************GPIO模块*********************************************************/
enum GPIO_OPRT_ENUM
{
    GPIO_OPRT_SET = 0,
    GPIO_OPRT_GET,
    GPIO_OPRT_BUTT
};
typedef unsigned int GPIO_OPRT_ENUM_UINT32;

/***************************************************MNTN模块*********************************************************/
/*组件类型*/
typedef enum
{
    VER_BOOTLOAD = 0,
    VER_BOOTROM =1,
    VER_NV =2 ,
    VER_VXWORKS =3,
    VER_DSP =4 ,
    VER_PRODUCT_ID =5 ,
    VER_WBBP =6 ,
    VER_PS =7,
    VER_OAM =8,
    VER_GBBP =9 ,
    VER_SOC =10,
    VER_HARDWARE =11,
    VER_SOFTWARE =12,
    VER_MEDIA =13,
    VER_APP =14,
    VER_ASIC =15,
    VER_RF =16,
    VER_PMU =17,
    VER_PDM = 18,
    VER_PRODUCT_INNER_ID =19,
    VER_INFO_NUM =20,
}COMP_TYPE_I;

/* 三色灯状态编号*/
enum
{
    LED_LIGHT_SYSTEM_STARTUP,
    LED_LIGHT_POWER_ON,
    LED_LIGHT_G_REGISTERED,
    LED_LIGHT_W_REGISTERED,
    LED_LIGHT_G_CONNECTED,
    LED_LIGHT_W_CONNNECTED,
    LED_LIGHT_H_CONNNECTED,
    LED_LIGHT_OFFLINE,
    LED_LIGHT_FORCE_UPDATING,
    LED_LIGHT_NORMAL_UPDATING,
    LED_LIGHT_UPDATE_FAIL,
    LED_LIGHT_UPDATE_SUCCESS,
    LED_LIGHT_UPDATE_FILEFAIL,
    LED_LIGHT_UPDATE_NVFAIL,
    LED_LIGHT_SIM_ABSENT,
    LED_LIGHT_SHUTDOWN,
    LED_LIGHT_G_PLMN_SEARCH,
    LED_LIGHT_W_PLMN_SEARCH,
	LED_LIGHT_L_REGISTERED,
    LED_LIGHT_STATE_MAX = 32,
};/*the state of LED */

enum SECURE_SUPPORT_STATUS_I
{
    SECURE_NOT_SUPPORT = 0,
    SECURE_SUPPORT = 1
};

enum SECURE_ENABLE_STATUS_I
{
    SECURE_DISABLE = 0,
    SECURE_ENABLE = 1
};

typedef enum                /* CACHE_TYPE */
{
    PS_OSAL_INSTRUCTION_CACHE ,
    PS_OSAL_DATA_CACHE
} PS_OSAL_CACHE_TYPE_I;

/***************************************************USB模块*********************************************************/
typedef  enum
{
    FLOW_CON,     /* 流控        */
    PDP_STATUS,   /* PDP激活结果 */
    SIGNAL_LEVEL, /* 信号强度    */
    PKT_STATICS,  /* 流量统计    */
    AT_CMD,       /* AT命令      */
    PS_NOTIFY,    /* Notify命令  */
    RESERVE       /* 保留        */
}NDIS_NOTICE;


typedef enum
{
    WWAN_WCDMA    = 1,/*WCDMA模式*/
    WWAN_CDMA	 = 2  /*CDMA模式*/
}NDIS_WWAN_MODE;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 消息头定义
*****************************************************************************/

/*****************************************************************************
  6 消息定义
*****************************************************************************/

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
/* 基本数据类型定义 */
typedef void (*PVOIDFUNC)();

typedef int  (*pFUNCPTR)();

typedef unsigned long (*pFUNCPTR2)( unsigned long ulPara1, unsigned long ulPara2);

typedef unsigned int tagUDI_DEVICE_ID_UINT32;

/***************************************************DPM模块*********************************************************/
typedef unsigned long (*pULULFUNCPTR)( unsigned long ulPara );

/***************************************************MNTN模块*********************************************************/
typedef int  (*OM_SAVE_FUNC)(int funcType, unsigned char *data, unsigned int* ulLength);

typedef struct
{
    unsigned char CompId;              /* 组件号：参见COMP_TYPE */
    unsigned char CompVer[VER_MAX_LENGTH+1];         /* 最大版本长度 30 字符+ \0 */
}VERSIONINFO_I;

typedef struct
{
    pULULFUNCPTR pRestoreFunction;
    pULULFUNCPTR pEnableFunction;
    pULULFUNCPTR pDisableFunction;
    pFUNCPTR2    pDfsSetFunc;
    pULULFUNCPTR pDfsReleaseFunc;
}DPM_SLEEP_CALLBACK_STRU;

typedef struct
{
    unsigned char enHSDSCHSupport;          /*硬件是否支持HSDPA*/
    unsigned char ucHSDSCHPhyCategory;      /*硬件支持的最大HSDPA能力等级*/
    unsigned char enEDCHSupport;            /*硬件是否支持HSUPA*/
    unsigned char ucEDCHPhyCategory;        /*硬件支持的最大HSUPA能力等级*/
    unsigned int  reserved;                 /*预留*/
}SUPPORT_MAX_HSPA_CAPA_STRU;

typedef unsigned long (*pPCSCFUNC)(unsigned long CmdType, unsigned char *pApdu, unsigned long ApduLen);

/* NV项50091结构，代表设备将要枚举的端口形态 */
typedef struct
{
    unsigned long ulStatus;
    unsigned char aucFirstPortStyle[DYNAMIC_PID_MAX_PORT_NUM];  /* 设备切换前端口形态 */
    unsigned char aucRewindPortStyle[DYNAMIC_PID_MAX_PORT_NUM]; /* 设备切换后端口形态 */
    unsigned char reserved[22];
}DRV_DYNAMIC_PID_TYPE_STRU;

/***************************************************SCI模块*********************************************************/
typedef void (*USIMMSCIInfo)(void);


/***************************************************USB模块*********************************************************/
/*管脚信号的定义*/
#define IO_CTRL_FC                      (0x02)
#define IO_CTRL_DSR                     (0x20)
#define IO_CTRL_DTR                     (0x01)
#define IO_CTRL_RFR                     (0x04)
#define IO_CTRL_CTS                     (0x10)
#define IO_CTRL_RI                      (0x40)
#define IO_CTRL_DCD                     (0x80)

typedef int  (*pComRecv)(unsigned char  uPortNo, unsigned char* pData, unsigned short uslength);

typedef int  (*pUartRecv)(unsigned char uPortNo, unsigned char* pData, unsigned short uslength);

typedef long (*pUsbStatus)(unsigned char ucStatus);

typedef long  (*pUsbState)(unsigned char ucStatus);

typedef int  (*pComStatusNotify)(unsigned char uStatus);

/* 和ACM 定义重复*/
#if 0
/* MODEM 管脚信号结构体定义*/
typedef struct _AT_DCE_MSC_STRU
{
    unsigned long    OP_Dtr     : 1;    /*DTR CHANGE FLAG*/
    unsigned long    OP_Dsr     :  1;  /*DSR CHANGE FLAG*/
    unsigned long    OP_Cts     :  1;  /*CTSCHANGE FLAG*/
    unsigned long    OP_Rts     : 1;   /*RTS CHANGE FLAG*/
    unsigned long    OP_Ri      :  1;   /*RI CHANGE FLAG*/
    unsigned long    OP_Dcd     :  1;  /*DCD CHANGE FLAG*/
    unsigned long    OP_Fc      :  1;  /*FC CHANGE FLAG*/
    unsigned long    OP_Brk     :  1;  /*BRK CHANGE FLAG*/
    unsigned long    OP_Spare   :  24; /*reserve*/
    unsigned char     ucDtr;                 /*DTR  VALUE*/
    unsigned char     ucDsr;                 /*DSR  VALUE*/
    unsigned char     ucCts;                 /*DTS VALUE*/
    unsigned char     ucRts;                  /*RTS  VALUE*/
    unsigned char     ucRi;                   /*RI VALUE*/
    unsigned char     ucDcd;                 /*DCD  VALUE*/
    unsigned char     ucFc;                   /*FC  VALUE*/
    unsigned char     ucBrk;                 /*BRK  VALUE*/
    unsigned char     ucBrkLen;         /*BRKLEN VALUE*/
}MODEM_MSC_STRU,*pMODEM_MSC_STRU,AT_DCE_MSC_STRU;
#endif

/*协议栈BULK数据接收回调函数类型定义*/
typedef int (*USB_MODEM_RECV_CALL)
(
    unsigned char ucPortType,          /*端口号*/
    unsigned char ucDlci,                 /*链路号*/
    unsigned char *pData,               /*数据指针*/
    unsigned short usLen                /*数据长度*/
);


/*协议栈消息通道状态变化数据接收回调函数定义*/
typedef int (*USB_MODEM_AT_Sig)
(
    unsigned char ucPortType,          /*端口号*/
    unsigned char ucDlci,                   /*链路号*/
    MODEM_MSC_STRU* data  /*指向MODEM 管角信号结构体的指针*/
);


/*协议栈断链、建链回调函数*/
typedef int (*USB_MODEM_IN_OUT)
(
    unsigned char ucPortType,           /*端口号*/
    unsigned char ucDlci                   /*链路号*/
);

/*协议栈高性能复制函数定义*/
typedef void (*USB_MODEM_COPY)
(
    unsigned char   *pDest,
    unsigned char   *pSrc,
    unsigned long   ulLen
);


typedef struct
{
    unsigned int    GateWayIpAddr;  /* PDP激活获取到的远程网关IP地址      */
    unsigned int    LocalIpAddr;    /* PDP激活获取到的分配到MODEM的IP地址 */
    unsigned int    MaskAddr;       /* 子网掩码 */
    unsigned int    speed;          /* 连接速度，单位bps */
    unsigned char   ActiveSatus;    /* 激活结果，0为成功，其他为失败 */
}NDIS_PDP_STRU,*pNDIS_PDP_STRU;


typedef struct
{
    unsigned int    currentTx;    /* CURRENT_TX */
    unsigned int    currentRx;    /* CURRENT_RX */
    unsigned int    maxTx;        /* MAX_TX */
    unsigned int    maxRx;        /* MAX_RX */
}NDIS_RATE_STRU,*pNDIS_RATE_STRU;


typedef struct
{
    unsigned int     TxOKCount;         /*发送包数*/
    unsigned int     RxOKCount;         /*接收包数*/
    unsigned int     TxErrCount;        /*发送错误*/
    unsigned int     RxErrCount;        /*接收错误*/
    unsigned int     TxOverFlowCount;   /*发送溢出丢包*/
    unsigned int     RxOverFlowCount;   /*接收溢出丢包*/
    unsigned int     MaxTx;         /*发送速率*/
    unsigned int     MaxRx;         /*接收速率*/
} NDIS_PKT_STATISTICS, *pNDIS_PKT_STATISTICS;

typedef struct
{
    unsigned char   *pucAtCmd;          /*指向AT命令的指针*/
    unsigned short   usLen;             /*AT命令的长度*/
}NDIS_AT_CMD_STRU, *pNDIS_AT_CMD_STRU;

/* 信号上报函数使用的结构体*/
typedef struct
{
    NDIS_NOTICE                 type;
    union
    {
        NDIS_PDP_STRU           pdp;
        NDIS_PKT_STATISTICS     pktStatics;
        unsigned char           flowFlag;   /*  1:流控，0：解流控 */
        int                     dbm;
        NDIS_AT_CMD_STRU        atCmd;
    }NDIS_STATUS_DATA;

} NDIS_PRO_STRU, *pNDIS_PRO_STRU;

typedef int (*USB_NDIS_RECV_CALL)
(
    unsigned char *pData, /* 接收缓冲区指针,内容为IP报文 */
    unsigned short usLen  /* 数据长度 */
);

typedef int (*USB_NDIS_BRK)
(
    void
);

typedef int (*USB_NDIS_EST)
(
    unsigned char *pData /* 接收缓冲区指针，内容为连接配置管理参数 */
);

typedef int (*USB_NDIS_PKT_STATISTICS_PERIOD)
(
    unsigned int period /*单位S */
);

typedef int (*USB_NDIS_GET_RATE)
(
    NDIS_RATE_STRU  *pRate
);

typedef int (*USB_NDIS_AT_CMD_RECV)
(
    unsigned char *pBuff,   /*指向AT命令指针*/
    unsigned short usLen    /*AT命令长度*/
);

typedef unsigned int (*USB_NDIS_GET_WWAN_MODE)	/*获取当前网络模式cdma/wcdma*/
(
    void
);

typedef unsigned long (*USB_NET_DEV_SET_MAX_TX_PACKET_NUMBER)
(
    unsigned long ulNumber
);

typedef void (*IPC_RECV_HANDLER)(unsigned char ucCidNo, unsigned char *pstData, unsigned int ulLength );

typedef int (*IPC_COM_RECV_HANDLER)(unsigned char uPortNo, unsigned char* pData, unsigned short uslength);



/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明(按模块分类)
*****************************************************************************/

/**************************************************ABB模块*********************************************************************/
/*****************************************************************************
 函 数 名  : BSP_ABB_RegGet
 功能描述  : 获取ABB寄存器值
 输入参数  : usInstruction: 待读寄存器地址
             uspData: 存放寄存器值
 输出参数  :
 返 回 值  :
*****************************************************************************/
extern int BSP_ABB_RegGet(unsigned char usInstruction, unsigned char *uspData);
#define DRV_ABB_REG_VALUE_GET(usInstruction,uspData)    BSP_ABB_RegGet (usInstruction,uspData)


/***************************************************AUDIO模块*********************************************************/
/********************************************************************************************************
 函 数 名  : BSP_AUDIO_CodecOpen
 功能描述  : 打开一个audio设备，并对codec进行初始化。
 输入参数  : devname：该参数固定为"/dev/codec0"。
                           flags：该参数表示读写方式，固定为0。
                           mode：该参数表示新建文件方式，固定为0。。
  输出参数  : 无。
 返 回 值  : 非负值:  设备句柄；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_AUDIO_CodecOpen(const char *devname, int flags, int mode);
#define DRV_CODEC_OPEN(devname, flags, mode)    BSP_AUDIO_CodecOpen(devname, flags, mode)

/********************************************************************************************************
 函 数 名  : BSP_AUDIO_Codec_ioctl
 功能描述  : codec设备命令字控制。
 输入参数  : devid：设备句柄。
                           cmd：命令字。
                           arg：命令参数。
  输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_AUDIO_Codec_ioctl(int devid, int cmd, int arg);
#define DRV_CODEC_IOCTL(devid, cmd, arg)   BSP_AUDIO_Codec_ioctl(devid, cmd, arg)


/***************************************************DLOAD模块*********************************************************/
/*****************************************************************************
 函 数 名  : BSP_DLOAD_SetSoftLoad
 功能描述  : 设置升级加载标志。
 输入参数  : flag：升级加载标志，
                   0：启动后进入bootrom，进行升级加载。
                   1：启动后不进入bootrom，正常启动。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
/*extern int BSP_DLOAD_SetSoftLoad(int flag);*/
#define DRV_SET_UPDATA_FLAG(flag)   BSP_DLOAD_SetSoftLoad(flag)

/*****************************************************************************
 函 数 名  : BSP_DLOAD_SetCdromMarker
 功能描述  : 设置ISO文件加载标志。
 输入参数  : flag：ISO文件加载标志，
                   0：启动后不加载ISO文件。
                   1：启动后加载ISO文件，正常启动。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
/*extern int BSP_DLOAD_SetCdromMarker(int flag);*/
#define DRV_SET_CDROM_FLAG(flag)    BSP_DLOAD_SetCdromMarker(flag)

/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetCdromMarker
 功能描述  : 获得ISO文件加载标志。
 输入参数  : 无
 输出参数  : 无。
 返 回 值  :
             0：启动后不加载ISO文件。
             1：启动后加载ISO文件，正常启动。
*****************************************************************************/
/*extern int BSP_DLOAD_GetCdromMarker(void);*/
#define DRV_GET_CDROM_FLAG()    BSP_DLOAD_GetCdromMarker()

/*****************************************************************************
 函 数 名  : getCdromVersion
 功能描述  : 获得ISO文件版本信息
 输入参数  : pVersionInfo: 存放返回的版本信息的内存地址
                           ulLength: 存放返回的版本信息的内存长度，目前固定
                           128
 输出参数  : pVersionInfo: 返回的版本信息的内存地址
 返 回 值  :  0:  操作成功；
             -1：操作失败。
*****************************************************************************/
/*extern int BSP_DLOAD_GetISOVer(char *pVersionInfo, unsigned long ulLength);*/
#define DRV_GET_CDROM_VERSION(pVersionInfo,ulLength)    BSP_DLOAD_GetISOVer(pVersionInfo, ulLength)

/********************************************************************************************************
 函 数 名  : BSP_DLOAD_NVBackupRead
 功能描述  : 从Flash中的NV项备份区读取数据，实现NV项的恢复功能。
 输入参数  : len：从NV项备份区起始处开始，需要读取的NV项长度（字节数），不超过1Block。
 输出参数  : pRamAddr：目的RAM地址，用于存放读出的NV项数据。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_DLOAD_NVBackupRead(unsigned char *pRamAddr, unsigned int len);
#define DRV_NVBACKUP_READ(pRamAddr,len)    BSP_DLOAD_NVBackupRead(pRamAddr, len)

/********************************************************************************************************
 函 数 名  : BSP_DLOAD_NVBackupWrite
 功能描述  : 将特定数据写入Flash中的NV项备份区，实现NV项的备份功能。
 输入参数  : pRamAddr：源RAM地址，用于存放需要写入的NV项数据。
             len：从NV项备份区起始处开始，需要写入的NV项长度（字节数），不超过1Block。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_DLOAD_NVBackupWrite(unsigned char *pRamAddr, unsigned int len);
#define DRV_NVBACKUP_WRITE(pRamAddr, len)   BSP_DLOAD_NVBackupWrite(pRamAddr, len)

/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetTFUpdateFlag
 功能描述  : 判断是否是TF卡升级
 输入参数  : None
 输出参数  : None
 返 回 值  : 返回1是TF升级，NV恢复成功，需要点蓝灯
             返回0不是TF升级，NV恢复成功，不需要点蓝灯

*****************************************************************************/
extern int BSP_DLOAD_GetTFUpdateFlag(void);
#define DRV_GET_TFUPDATE_FLAG()    BSP_DLOAD_GetTFUpdateFlag()


/****************************************** FS模块*******************************************/
/********************************************************************************************************
 函 数 名  : BSP_FS_GetDskspc
 功能描述  : 得到文件系统空间。
 输入参数  : path：文件系统路径。
 输出参数  : DskSpc：总空间。
                           UsdSpc：用过空间。
                           VldSpc：空闲空间。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_FS_GetDskspc(const char *path,unsigned int *DskSpc,unsigned int  *UsdSpc,  unsigned int *VldSpc);
#define DRV_FILE_GET_DISKSPACE(path,DskSpc,UsdSpc,VldSpc)    BSP_FS_GetDskspc(path,DskSpc,UsdSpc,VldSpc)


/****************************************** GPIO模块*******************************************/
/*****************************************************************************
 函 数 名  : BSP_GPIO_Oprt
 功能描述  : at^GPIOPL,设置和查询GPIO的电平
 输入参数  : 操作类型 ulOp:
             0  设置各GPIO的PL值
             1  查询当前各GPIO的PL值

             pucPL 对应20Byte的数组,每个BYTE代表一个8个管脚的值

             设置操作时,pucPL为设置的20的Byte
             查询操作时,pucPL为当前获取到的PL的实际值组成的16进制数据
             例如用户设置at^GPIOPL = ABCDEF,则对应pucPL的数组值为{A,B,C,D,E,F,0,0,...}

 输出参数  : 无
 返 回 值  :  0 ：OK  非 0 ：Error

*****************************************************************************/
extern unsigned long BSP_GPIO_Oprt(unsigned long ulOp, unsigned char *pucPL);
#define DRV_GPIO_OPRT(ulOp,pucPL)    BSP_GPIO_Oprt(ulOp,pucPL)


/******************************************MNTN模块*******************************************/
/*****************************************************************************
 函 数 名  : BSP_MNTN_GetFlashSpec
 功能描述  : Get flash infomation
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern int BSP_MNTN_GetFlashSpec(unsigned char * pFlashInfo, unsigned int ulLength);
#define DRV_GET_FLASH_INFO(pFlashInfo, usLength)    BSP_MNTN_GetFlashSpec (pFlashInfo, usLength )

/*****************************************************************************
 函 数 名  : BSP_GU_GetVerTime
 功能描述  : 获取版本编译时间
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern char *BSP_GU_GetVerTime(void);
#define DRV_GET_VERSION_TIME()    BSP_GU_GetVerTime()

/*****************************************************************************
 函 数 名  : BSP_MNTN_MemVersionCtrl
 功能描述  : 组件版本读写接口。
 输入参数  : pcData：当Mode为读的时候，为调用者待保存返回的组件版本信息的内存地址；
                     当Mode为写的时候，为调用者准备写入的组件版本信息的地址。
             ucLength：当Mode为读的时候，为调用者待保存返回的组件版本信息的内存大小；
                       当Mode为写的时候，为调用者准备写入的组件版本信息的字符数（不包括'\0'）。
             ucType：版本信息ID，
                     0： BOOTLOAD；
                     1： BOOTROM；
                     2： NV；
                     3： VXWORKS；
                     4： DSP；
                     5； CDROMISO；
                     6： PHY；
                     7： PS；
                     8： COM；
                     9： RF；
                     10：SOC；
                     11：HARDWARE；
                     12：SOFTWARE；
                     13：MEDIA；
                     14：APP；
                     15：INFO_NUM。
             ucMode：0：读取指定ID的组件版本信息；1：写入指定ID的组件版本信息。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_MemVersionCtrl(signed char *pcData, unsigned char ucLength, unsigned char ucType, unsigned char ucMode);
#define DRV_MEM_VERCTRL(pcData,ucLength,ucType,ucMode)  \
                          BSP_MNTN_MemVersionCtrl(pcData,ucLength,ucType,ucMode)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetFullHwVersion
 功能描述  : 硬件完整版本读接口。
 输入参数  : pFullHwVersion：为调用者待保存返回的硬件完整版本信息的内存首地址；
             ulLength      ：为调用者待保存返回的硬件完整版本信息的内存大小；
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetFullHwVersion(char * pFullHwVersion,unsigned int ulLength);
#define  DRV_GET_FULL_HW_VER(pFullHwVersion,ulLength)  \
               BSP_MNTN_GetFullHwVersion(pFullHwVersion,ulLength)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetProductIdInter
 功能描述  : 产品名称完整版本读接口。
 输入参数  : pProductIdInter：为调用者待保存返回的产品名称完整版本的内存首地址；
             ulLength       ：为调用者待保存返回的产品名称完整版本的内存大小；
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetProductIdInter(char * pProductIdInter, unsigned int ulLength);
#define  DRV_GET_PRODUCTID_INTER_VER(pProductIdInter,ulLength)  \
              BSP_MNTN_GetProductIdInter(pProductIdInter,ulLength)

/*****************************************************************************
 函 数 名  : BSP_MNTN_VersionQueryApi
 功能描述  : 查询所有组件的版本号。
 输入参数  : ppVersionInfo：待保存的版本信息地址。
 输出参数  : ucLength：待返回的数据的字节数。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_VersionQueryApi(void ** ppVersionInfo, unsigned int * ulLength);
#define  DRV_VER_QUERY(ppVersionInfo,ulLength)  \
              BSP_MNTN_VersionQueryApi (ppVersionInfo,ulLength)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetGsmPATemperature
 功能描述  : 获取GSM PA温度
 输入参数  : Temprature
             hkAdcTalble
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetGsmPATemperature(int *temperature, unsigned short *hkAdcTable);
#define DRV_GET_PA_GTEMP(temperature, hkAdcTable)   \
                BSP_MNTN_GetGsmPATemperature(temperature, hkAdcTable)

/*****************************************************************************
 函 数 名  : getWcdmaPATemperature
 功能描述  : 获取WCDMA PA温度
 输入参数  : Temprature
             hkAdcTalble
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetWcdmaPATemperature (int *temperature, unsigned short *hkAdcTable);
#define  DRV_GET_PA_WTEMP(temperature, hkAdcTable)  \
              BSP_MNTN_GetWcdmaPATemperature(temperature, hkAdcTable)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetSIMTemperature
 功能描述  : 获取SIM卡温度
 输入参数  : pusHkAdcTable：电磁电压表
 输出参数  : pTemp：        指向SIM卡温度的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetSIMTemperature(int *plTemp, unsigned short *pusHkAdcTable);
#define DRV_GET_SIM_TEMP(plTemp, pusHkAdcTable)   \
               BSP_MNTN_GetSIMTemperature(plTemp, pusHkAdcTable)

/*****************************************************************************
 函 数 名  : BSP_MNTN_ABBSelfCheck
 功能描述  : 返回abb自检结果
 输入参数  : None
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_ABBSelfCheck(void);
#define DRV_ABB_SELFCHECK()    BSP_MNTN_ABBSelfCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_PmuSelfCheck
 功能描述  : PMU自检。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_PmuSelfCheck(void);
#define DRV_PMU_SELFCHECK()    BSP_MNTN_PmuSelfCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_I2cSelfCheck
 功能描述  : 返回I2c自检结果
 输入参数  : None
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
extern int BSP_MNTN_I2cSelfCheck(void);
#define DRV_I2C_SELFCHECK()    BSP_MNTN_I2cSelfCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_SpiSelfCheck
 功能描述  : 返回spi自检结果
 输入参数  : None
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
extern int BSP_MNTN_SpiSelfCheck(void);
#define DRV_SPI_SELFCHECK()    BSP_MNTN_SpiSelfCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetCodecFuncStatus
 功能描述  : 获取CODEC功能状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 1:  CODEC存在
             0：CODEC 不存在
*****************************************************************************/
extern  int BSP_MNTN_GetCodecFuncStatus(void);
#define DRV_GET_CODEC_FUNC_STATUS()    BSP_MNTN_GetCodecFuncStatus()

/*****************************************************************************
 函 数 名  : MNTN_RFGLockStateGet
 功能描述  : 读取GSM RF锁定状态。
 输入参数  : 无。
 输出参数  : Status：存放锁定状态的输出值，
                        0：TX或者RX被锁定。
                        1：TX、RX都没有被锁定；

 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_RFGLockStateGet(unsigned int *Status);
#define DRV_GET_RF_GLOCKSTATE(Status)    BSP_MNTN_RFGLockStateGet(Status)

/****************************************************************************
 函 数 名  : BSP_MNTN_LedStateFuncReg
 功能描述  : 本接口为提供给协议栈的钩子函数，用于获取协议栈的点灯状态。
 输入参数  : 协议栈获取点灯状态函数的指针。
 输出参数  : 无。
 返 回 值  : 无。
 注意事项  : 无。

******************************************************************************/
extern void BSP_MNTN_LedStateFuncReg (pFUNCPTR p);
#define DRV_LED_STATE_FUNREG(p)    BSP_MNTN_LedStateFuncReg(p)

/*****************************************************************************
 函 数 名  : BSP_MNTN_LedFlush
 功能描述  : 三色灯设置。
 输入参数  : status：三色灯的状态，见LED_STATUS_I
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_LedFlush(unsigned long state);
#define DRV_LED_FLUSH(state)    BSP_MNTN_LedFlush(state)

/*****************************************************************************
 函 数 名  : BSP_MNTN_LedControlAddressGet
 功能描述  : 得到定制的点灯数据
 输入参数  : 无
 输出参数  : 无。
 返 回 值  : 点灯控制数据的全局变量地址
*****************************************************************************/
extern unsigned int BSP_MNTN_LedControlAddressGet(void);
#define DRV_LED_GET_ADDRESS()   BSP_MNTN_LedControlAddressGet()

/*****************************************************************************
 函 数 名  : BSP_MNTN_BootForceloadModeCheck
 功能描述  : 强制加载模式查询。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：强制加载；
             1：非强制加载。
*****************************************************************************/
extern unsigned int BSP_MNTN_BootForceloadModeCheck(void);
#define DRV_BOOT_FORCELOAD_MODE_CHECK()    BSP_MNTN_BootForceloadModeCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_BootFlagConfig
 功能描述  : 配置Bootline数据中的启动标志为快速自启动、倒计时7秒自启动或手动启动。
 输入参数  : ulBootFlag：暂未定义。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern  int BSP_MNTN_BootFlagConfig(unsigned int ulBootFlag);
#define DRV_BOOTFLAG_CFG(ulBootFlag)    BSP_MNTN_BootFlagConfig(ulBootFlag)

/*****************************************************************************
 函 数 名  : BSP_MNTN_CheckArmTCM
 功能描述  : TCM检测
 输入参数  : 无。
 输出参数  : 无。
 返回值：   0xffff0000：检测成功，TCM正常
            地址：检测出错的地址（0－0x5fff）
            0xffffffff:检测失败

*****************************************************************************/
extern unsigned int BSP_MNTN_CheckArmTCM(void);
#define DRV_CHECK_ARM_TCM()    BSP_MNTN_CheckArmTCM()

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExchOMSaveRegister
 功能描述  : 注册给底软的trace保存函数，用来在单板复位时，
             保存当前的信令和日志信息。
 输入参数  : funcType: 功能ID
             pFunc:注册的回调函数
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：相同的funcType只能注册一个回调函数，重复注册会覆盖原来函数
*****************************************************************************/
extern int BSP_MNTN_ExchOMSaveRegister(int funcType, OM_SAVE_FUNC *pFunc);
#define DRV_SAVE_REGISTER(funcType, pFunc)    BSP_MNTN_ExchOMSaveRegister(funcType, pFunc)

#if defined(VERSION_V7R1_C010)
/*****************************************************************************
 函 数 名  : BSP_MNTN_SystemError
 功能描述  : 系统热启动。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_SystemError(int modId, int arg1, int arg2, char * arg3, int arg3Length);
#define DRV_SYSTEM_ERROR(modId, arg1, arg2, arg3, arg3Length)\
                   BSP_MNTN_SystemError(modId, arg1, arg2, arg3, arg3Length)
#else
/*****************************************************************************
 函 数 名  : systemError
 功能描述  : 系统热启动。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void systemError(int modId, int arg1, int arg2, char * arg3, int arg3Length);
#define DRV_SYSTEM_ERROR(modId, arg1, arg2, arg3, arg3Length)\
                    systemError(modId, arg1, arg2, arg3, arg3Length)
#endif

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExchMemMalloc
 功能描述  : 与systemOsaError接口一起用来记录大块的复位信息。
             目前最多支持128k.
 输入参数  : ulSize - 内存大小
 输出参数  : 无。
 返 回 值  : 内存地址
*****************************************************************************/
extern unsigned int BSP_MNTN_ExchMemMalloc(unsigned int ulSize);
#define DRV_EXCH_MEM_MALLOC(ulSize)    BSP_MNTN_ExchMemMalloc(ulSize)

/*****************************************************************************
 函 数 名  : BSP_MNTN_OmExchFileSave
 功能描述  : 用来记录ARM和DSP交互的异常文件
 输入参数  :    address：buffer地址
                length：存储长度
                IsFileEnd：1表示文件的最后一块，0表示非最后一块
 输出参数  : 无。
 返 回 值  : 0 为OK。
*****************************************************************************/
extern int BSP_MNTN_OmExchFileSave(void * address, unsigned long length, unsigned char IsFileEnd,unsigned char FileType);
#define DRV_EXCH_FILE_SAVE(address, length, IsFileEnd, FileType) BSP_MNTN_OmExchFileSave(address, length, IsFileEnd, FileType)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetHwGpioInfo
 功能描述  : Get flash infomation
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern int BSP_MNTN_GetHwGpioInfo(unsigned char *pGpioInfo, unsigned long usLength );
#define DRV_GET_GPIO_INFO(pGpioInfo, usLength)    BSP_MNTN_GetHwGpioInfo(pGpioInfo, usLength )

/*****************************************************************************
 函 数 名  : BSP_MNTN_Int4ToString
 功能描述  : 将寄存器的值转换为ASCII字符
 输入参数  : 寄存器的值，和转换字符的存储空间,最大空间不超过30byte
 输出参数  : 转换字符
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_Int4ToString(unsigned int value, char * string);
#define DRV_MEM_VERCONVERT(value,string)    BSP_MNTN_Int4ToString(value, string)

/*****************************************************************************
 函 数 名  : BSP_MNTN_HeapInfoGet
 功能描述  : 获取单板侧的内存信息。
 输入参数  : allocSize: 已经分配的堆内存大小，单位byte。
             totalSize: 堆内存总尺寸，单位byte。
 输出参数  : None
 返 回 值  : 0: 操作成功；
             -1：  操作失败。
*****************************************************************************/
extern int BSP_MNTN_HeapInfoGet(unsigned int *allocSize, unsigned int *totalSize);
#define DRV_GET_HEAPINFO(allocSize, totalSize)    BSP_MNTN_HeapInfoGet(allocSize, totalSize)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetSupportBands
 功能描述  : 从底软获得当前支持的Bands.
 输入参数  : pusWBands - WCDMA的频段值
             pusGBands - GSM的频段值

 输出参数  : pusWBands - WCDMA的频段值
             pusGBands - GSM的频段值

 返 回 值  : 0：正确，非0: 失败
             通过Bit位来表示哪些频段支持。
*****************************************************************************/
extern int BSP_MNTN_GetSupportBands(unsigned short *pusWBands, unsigned short *pusGBands);
#define DRV_GET_SUPPORT_BANDS(pusWBands, pusGBands)    BSP_MNTN_GetSupportBands(pusWBands, pusGBands)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetSupportDivBands
 功能描述  : 从底软获得当前支持的分级频段
 输入参数  : N/A

 输出参数  : pDivBands - 返回当前单板所支持的分级频段，频段按bit
 			      流方式排列(同W 和G的频段查询)

 返 回 值  : 0：正确，非0: 失败
*****************************************************************************/
extern int BSP_MNTN_GetSupportDivBands(unsigned short* pDivBands);

#define DRV_GET_SUPPORT_DIVBANDS(pDivBands)    BSP_MNTN_GetSupportDivBands(pDivBands)

/********************************************************************************************************
 函 数 名  : BSP_MNTN_WriteSysBackupFile
 功能描述  : 向FLASH备份ZSP、NV数据
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_MNTN_WriteSysBackupFile(void);
#define DRV_BACKUP_SYS_FILE()    BSP_MNTN_WriteSysBackupFile()

/*****************************************************************************
 函 数 名  : DRV_MNTN_GetExtAntenLockState
 功能描述  : 获取有线与无线的连接状态
 输入参数  : None
 输出参数  : 0 - 无线连接
             1 - 有线连接
 返 回 值  : 0 - 成功
             其它为失败

*****************************************************************************/
extern int BSP_MNTN_GetExtAntenLockState(unsigned int *pulStatus);
#define DRV_GET_ANTEN_LOCKSTATE(enModemId, pulStatus)    BSP_MNTN_GetExtAntenLockState(pulStatus)

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExtAntenIntInstall
 功能描述  : 中断注册函数，用来获得当前的天线状态
 输入参数  : routine   - 中断处理函数
             para      - 保留字段
 输出参数  : None
 返 回 值  : void

*****************************************************************************/
extern void BSP_MNTN_ExtAntenIntInstall(void* routine, int para);
#define DRV_ANTEN_INT_INSTALL(routine, para)    BSP_MNTN_ExtAntenIntInstall(routine, para)

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExchHookFuncAdd
 功能描述  : 任务切换及中断钩子注册
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void BSP_MNTN_ExchHookFuncAdd(void);
#define DRV_EXCH_HOOK_FUNC_ADD()    BSP_MNTN_ExchHookFuncAdd()

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExchHookFuncDelete
 功能描述  : 任务切换及中断钩子注销
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void BSP_MNTN_ExchHookFuncDelete(void);
#define DRV_EXCH_HOOK_FUNC_DELETE()    BSP_MNTN_ExchHookFuncDelete()

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExcStackPeakRecord
 功能描述  : 记录任务栈超标的任务信息.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_ExcStackPeakRecord(void);
#define DRV_STACK_HIGH_RECORD()    BSP_MNTN_ExcStackPeakRecord()

/*****************************************************************************
 函 数 名  : BSP_MNTN_SubIntHook
 功能描述  : 二级中断服务钩子函数注册。可维可测相关
 输入参数  : p_Func：钩子函数指针。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_SubIntHook(pFUNCPTR p_Func);
#define DRV_VICINT_HOOK_ADD(p_Func)    BSP_MNTN_SubIntHook(p_Func)

/*****************************************************************************
 函 数 名  : BSP_MNTN_SubIntHookDel
 功能描述  : 二级中断服务钩子函数注销。可维可测相关
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_SubIntHookDel(void);
#define DRV_VICINT_HOOK_DEL()    BSP_MNTN_SubIntHookDel()

/*****************************************************************************
 函 数 名  : BSP_MNTN_IntBreakInHook
 功能描述  : 一级中断入口钩子函数注册。可维可测相关
 输入参数  : p_Func :钩子函数。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntBreakInHook(pFUNCPTR p_Func);
#define DRV_VICINT_IN_HOOK_ADD(p_Func)   BSP_MNTN_IntBreakInHook(p_Func)

/*****************************************************************************
 函 数 名  : BSP_MNTN_IntBreakInHookDel
 功能描述  : 一级中断入口钩子函数注销。可维可测相关
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntBreakInHookDel(void);
#define DRV_VICINT_IN_HOOK_DEL()    BSP_MNTN_IntBreakInHookDel()

/*****************************************************************************
 函 数 名  : BSP_MNTN_IntBreakOutHook
 功能描述  : 一级中断出口钩子函数注册。可维可测相关，V7先打桩
 输入参数  : p_Func: 钩子函数。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntBreakOutHook(pFUNCPTR p_Func);
#define DRV_VICINT_OUT_HOOK_ADD(p_Func)    BSP_MNTN_IntBreakOutHook(p_Func)

/*****************************************************************************
 函 数 名  : vicIntBreakOutHookDel
 功能描述  : 一级中断出口钩子函数注销。可维可测相关
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntBreakOutHookDel(void);
#define DRV_VICINT_OUT_HOOK_DEL()    BSP_MNTN_IntBreakOutHookDel()

/*****************************************************************************
 函 数 名  : BSP_MNTN_IntLvlChgHook
 功能描述  : 一级中断level change钩子函数注册。可维可测相关
 输入参数  : p_Func: 钩子函数。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntLvlChgHook(pFUNCPTR p_Func);
#define DRV_VICINT_LVLCHG_HOOK_ADD(p_Func)    BSP_MNTN_IntLvlChgHook(p_Func)

/*****************************************************************************
 函 数 名  : BSP_MNTN_IntLvlChgHookDel
 功能描述  : 一级中断level change钩子函数注销。可维可测相关
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntLvlChgHookDel(void);
#define DRV_VICINT_LVLCHG_HOOK_DEL()    BSP_MNTN_IntLvlChgHookDel()

/*****************************************************************************
 函 数 名  : BSP_MNTN_TFUPIfNeedNvBackup
 功能描述  : 使用TF卡升级前通过该接口判断是否需要备份NV项.
 输入参数  : None
 输出参数  : None
 返 回 值  : 1:需要备份
             0:无需备份
*****************************************************************************/
extern int BSP_MNTN_TFUPIfNeedNvBackup(void);
#define DRV_TF_NVBACKUP_FLAG()    BSP_MNTN_TFUPIfNeedNvBackup()

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetSDDevName
 功能描述  : 获取设备列表中的SD设备名
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : SD_DEV_TYPE_STR结构指针。
*****************************************************************************/
extern SD_DEV_TYPE_STR * BSP_MNTN_GetSDDevName(void);
#define DRV_GET_SD_PATH()     BSP_MNTN_GetSDDevName()

/****************************************** PMU模块 *******************************************/
/*****************************************************************************
 函 数 名  : BSP_PMU_GetPMUState
 功能描述  : 获取PMU模块开机方式、充电状态、电池电量、电池在位状态。
 输入参数  : 无。
 输出参数  : Pmu_State :开机方式、充电状态、电池电量、电池在位状态。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
extern unsigned int BSP_PMU_GetPMUState(void*  Pmu_State);
#define DRV_GET_PMU_STATE(Pmu_State)     BSP_PMU_GetPMUState(Pmu_State)

/*****************************************************************************
 函 数 名  : BSP_PMU_UsbEndRegActionFunc
 功能描述  : 本接口是USB插入/拔出回调注册函数。
 输入参数  : srcFunc：注册的USB插入或拔出动作发生时的回调函数指针。
             actionIndex：动作指示。
                          0：参数srcFunc是注册的USB插入动作的回调函数；
                          1：参数srcFunc是注册的USB拔出动作的回调函数。

 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：调用此接口，将所要执行的函数指针传入，即在USB插入或拔出时调用所要执行的函数。
*****************************************************************************/
extern int BSP_PMU_UsbEndRegActionFunc(pFUNCPTR srcFunc, unsigned char actionIndex);
#define DRV_USB_ENDREGACTION_FUNC(srcFunc,actionIndex) \
                BSP_PMU_UsbEndRegActionFunc(srcFunc,actionIndex)


/******************************************PRODUCT_INFO模块***************************/
/*****************************************************************************
 函 数 名  : BSP_PROD_GetRecoverNvInfo
 功能描述  : 从底软获得当前需要覆盖的NV信息
 输入参数  : N/A

 输出参数  : ppNvInfo  - 指向NV信息结构体的数组
             pulNvNum  - 数组个数

 返 回 值  : 0：正确，非0: 失败
*****************************************************************************/
extern  int BSP_PROD_GetRecoverNvInfo(void **ppNvInfo, unsigned long *pulNvNum);
#define  DRV_GET_RECOVER_NV_INFO(ppNvInfo, pulNvNum)  \
                BSP_PROD_GetRecoverNvInfo(ppNvInfo, pulNvNum)


/******************************************SD模块*******************************************/
/*****************************************************************************
 函 数 名  : BSP_MNTN_GETSDSTATUS
 功能描述  : 返回SD卡在位状态
 输入参数  : 无。
 输出参数  : 无。
 返回值：   1在位
            0不在位

*****************************************************************************/
extern int BSP_MNTN_GETSDSTATUS(void);
#define DRV_SDMMC_GET_STATUS()    BSP_MNTN_GETSDSTATUS()

/*****************************************************************************
 函 数 名  : SDIO_read_write_blkdata
 功能描述  : SD卡读写接口
 输入参数  :    nCardNo     SD卡号
                dwBlkNo     块数
                nBlkCount   读写大小
                pbDataBuff  读写缓冲区
                nFlags      读/写
 输出参数  : 无。
 返回值：   0:  读写成功
            非0:错误码
*****************************************************************************/
extern int BSP_MNTN_SDRWBlkData(int nCardNo,unsigned int dwBlkNo,
                   unsigned int nBlkCount, unsigned char *pbDataBuff,int nFlags);
#define DRV_SDMMC_BLK_DATA_RW(nCardNo,dwBlkNo,nBlkCount,pbDataBuff,nFlags)   \
             BSP_MNTN_SDRWBlkData(nCardNo,dwBlkNo,nBlkCount,pbDataBuff, nFlags)

/*****************************************************************************
 函 数 名  : BSP_SDMMC_ATProcess
 功能描述  : at^sd,SD卡操作，写，擦除，格式化操作
 输入参数  : 操作类型 ulOp:
            0  格式化SD卡
            1  擦除整个SD卡内容；
            2  用于指定地址内容的擦除操作，指定擦除的内容长度为512字节。擦除后的地址中写全1
            3  写数据到SD卡的指定地址中，需要带第二个和第三个参数
            4  读取ulAddr指定的地址(ulAddr*512)的512个字节的内容到pucBuffer中

            ulAddr < address >  地址，以512BYTE为一个单位，用数字n表示

            ulData
             < data >            数据内容，表示512BYTE的内容，每个字节的内容均相同。
             0       字节内容为0x00
             1       字节内容为0x55
             2       字节内容为0xAA
             3       字节内容为0xFF

 输出参数  : pulErr
 返 回 值  : 0 ：OK  非 0 ：Error

            具体的错误值填充在*pulErr中
            0 表示SD卡不在位
            1 表示SD卡初始化失败
            2 表示<opr>参数非法，对应操作不支持(该错误由AT使用,不需要底软使用)
            3 表示<address>地址非法，超过SD卡本身容量
            4 其他未知错误
*****************************************************************************/
extern  unsigned long  BSP_SDMMC_ATProcess(SD_MMC_OPRT_ENUM_UINT32 ulOp,
                unsigned long ulAddr,  unsigned long ulData,unsigned char *pucBuffer,unsigned long *pulErr);
#define DRV_SDMMC_AT_PROCESS(ulOp,ulAddr,ulData,pucBuffer,pulErr)   \
                     BSP_SDMMC_ATProcess (ulOp,ulAddr,ulData,pucBuffer,pulErr)

/*****************************************************************************
 函 数 名  : BSP_SDMMC_GetOprtStatus
 功能描述  : at^sd,SD卡当前操作状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
            0: 未操作或操作已经完成;
            1: 操作中
            2: 上次操作失败
*****************************************************************************/
extern unsigned long BSP_SDMMC_GetOprtStatus(void);
#define DRV_SDMMC_GET_OPRT_STATUS()    BSP_SDMMC_GetOprtStatus()


typedef enum USB_CHARGE_TYPE_ENUM_tag
{
    USB_CHARGE_TYPE_NON_CHECK = -1,          /* 还未判断出来连接线类型 */
    USB_CHARGE_TYPE_HW = 0,                      /* 华为充电器             */
    USB_CHARGE_TYPE_NON_HW =1,                  /* 非华为充电器           */

}USB_CHARGE_TYPE_ENUM;

/*Battery query API*/
typedef enum CHARGING_STATE_ENUM_tag
{
    CHARGING_INIT = -1,
    NO_CHARGING_UP= 0,         /* 开机未充电*/
    CHARGING_UP,               /*开机正在充电 */
    NO_CHARGING_DOWN,          /* 关机未充电 */
    CHARGING_DOWN,             /* 关机充电  */
}CHARGING_STATE_ENUM;

/* 电池电量及低电量查询**/
typedef enum BATT_LEVEL_ENUM_tag
{
    BATT_INIT = -2,
    BATT_LOW_POWER = -1 ,  /*电池低电，*/
    BATT_LEVEL_0,                 /*0格电池电量**/
    BATT_LEVEL_1 ,                /*1格电池电量*/
    BATT_LEVEL_2 ,                 /*2格电池电量*/
    BATT_LEVEL_3 ,               /*3格电池电量*/
    BATT_LEVEL_4  ,              /*4格电池电量*/
    BATT_LEVEL_MAX
}BATT_LEVEL_ENUM;



typedef struct BATT_STATE_tag
{
   CHARGING_STATE_ENUM  charging_state;
   BATT_LEVEL_ENUM           battery_level;
}BATT_STATE_T;

/*****************************************************************************
 函 数 名  : BSP_CHG_StateSet
 功能描述  :使能或者禁止充电
 输入参数  :ulState      0:禁止充电
 						1:使能充电
 输出参数  :无
 返回值：    无
*****************************************************************************/
extern void BSP_CHG_StateSet(unsigned long ulState);
#define DRV_CHG_STATE_SET(ulState)    BSP_CHG_StateSet(ulState)

/*****************************************************************************
 函 数 名  : BSP_CHG_GetBatteryState
 功能描述  :获取底层电池状态信息
 输入参数  :battery_state 电量信息
 输出参数  :battery_state 电量信息
 返回值：   0 操作成功
                         -1操作失败

*****************************************************************************/
extern int BSP_CHG_GetBatteryState(BATT_STATE_T *battery_state);
#define DRV_CHG_GET_BATTERY_STATE(battery_state)    BSP_CHG_GetBatteryState(battery_state)

/*****************************************************************************
 函 数 名  : BSP_CHG_ChargingStatus
 功能描述  :查询目前是否正在充电中
 输入参数  :无
 输出参数  :无
 返回值：   0 未充电
                          1 充电中
*****************************************************************************/
extern int BSP_CHG_ChargingStatus(void);
#define DRV_CHG_GET_CHARGING_STATUS()    BSP_CHG_ChargingStatus()

/*****************************************************************************
 函 数 名  : BSP_CHG_GetCbcState
 功能描述  : 返回电池状态和电量
 输入参数  :pusBcs 0:电池在供电 1:与电池连接电池未供电 2:没有与电池连接
                          pucBcl  0:电量不足或没有与电池连接
 输出参数  : pusBcs 0:电池在供电 1:与电池连接电池未供电 2:没有与电池连接
                          pucBcl  0:电量不足或没有与电池连接
 返回值：   0 操作成功
                         -1操作失败

*****************************************************************************/
extern int BSP_CHG_GetCbcState(unsigned char *pusBcs,unsigned char *pucBcl);
#define DRV_CHG_GET_CBC_STATE(pusBcs,pucBcl)    BSP_CHG_GetCbcState(pusBcs,pucBcl)

/*************************INT BEGIN*****************************/
typedef void(* USB_NET_DEV_SWITCH_GATEWAY)
(
    void
);
extern void DRV_AT_SETAPPDAILMODE(unsigned int ulStatus);


/*****************************************************************************
 函 数 名  : BSP_USB_NASSwitchGatewayRegExtFunc
 功能描述  : 本接口用于NAS注册切换网关通知回调函数
 输入参数  :回调接口。
 输出参数  : 无。
 返 回 值  : 0：成功
                       非零:失败
*****************************************************************************/
extern int BSP_USB_NASSwitchGatewayRegFunc(USB_NET_DEV_SWITCH_GATEWAY switchGwMode) ;
#define DRV_USB_NAS_SWITCH_GATEWAY_REGFUNC(switchGwMode)    BSP_USB_NASSwitchGatewayRegFunc(switchGwMode)

/*************************CLK BEGIN*****************************/
/*****************************************************************************
 函 数 名  : BSP_ONOFF_DrvPowerOff
 功能描述  : 单板直接下电
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void BSP_ONOFF_DrvPowerOff(void);
#define DRV_POWER_OFF() BSP_ONOFF_DrvPowerOff()

/************************************************************************
 * FUNCTION
 *       max_freeblock_size_get
 * DESCRIPTION
 *       get memory max free block size
 * INPUTS
 *       无
 * OUTPUTS
 *       max free block size
 *************************************************************************/
extern int DRV_GET_FREE_BLOCK_SIZE(void);

/*****************************************************************************
 函 数 名  : BSP_SDMMC_UsbGetStatus
 功能描述  : 返回SD卡状态
 输入参数  : 无。
 输出参数  : 无。
 返回值：   0  为可用
           非0 不可用

*****************************************************************************/
extern unsigned int BSP_SDMMC_UsbGetStatus(void);
#define DRV_SDMMC_USB_STATUS()    BSP_SDMMC_UsbGetStatus();


/****************************************** SYSCTRL模块*******************************************/
/*****************************************************************************
 函 数 名  : BSP_GUSYS_DspReset
 功能描述  : 本接口实现ZSP模块的复位动作，包括zsp core、总线和外设。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_DspReset(void);
#define DRV_DSP_ZONE_RESET()    BSP_GUSYS_DspReset()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_DspResetCancel
 功能描述  : 本接口实现ZSP模块的解复位动作，包括zsp core、总线和外设。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_DspResetCancel(void);
#define DRV_DSP_ZONE_RESET_CANCEL()    BSP_GUSYS_DspResetCancel()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_ABBGSMRxCtrl
 功能描述  : 本接口实现ABB GSM接收控制。
 输入参数  : ucStatus：0：关闭；1：打开。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern unsigned int BSP_GUSYS_ABBGSMRxCtrl(unsigned char ucStatus);
#define DRV_ABB_GRX_CTRL(ucStatus)    BSP_GUSYS_ABBGSMRxCtrl(ucStatus)

/*****************************************************************************
 函 数 名  : ABBWCDMARxCtrl
 功能描述  : 本接口实现ABB WCDMA接收控制。
 输入参数  : ucStatus：0：关闭；1：打开。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern unsigned int BSP_GUSYS_ABBWCDMARxCtrl(unsigned char ucStatus);
#define DRV_ABB_WRX_CTRL(ucStatus)    BSP_GUSYS_ABBWCDMARxCtrl(ucStatus)

/*****************************************************************************
 函 数 名  : ABBTxCtrl
 功能描述  : 本接口实现ABB 发送控制。
 输入参数  : ucStatus：0：关闭；1：打开。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：ABB发送控制不区分W/G模式。
*****************************************************************************/
extern int BSP_GUSYS_ABBTxCtrl(unsigned char ucStatus);
#define DRV_ABB_TX_CTRL(ucStatus)    BSP_GUSYS_ABBTxCtrl(ucStatus)

/*****************************************************************************
 函 数 名  : BSP_GUSYS_WcdmaBbpPllEnable
 功能描述  : 本接口实现使能WCDMA BBP PLL。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_WcdmaBbpPllEnable(void);
#define DRV_WBBP_PLL_ENABLE()    BSP_GUSYS_WcdmaBbpPllEnable()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_WcdmaBbpPllDisable
 功能描述  : 本接口实现关闭WCDMA BBP PLL。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_WcdmaBbpPllDisable(void);
#define DRV_WBBP_PLL_DSABLE()    BSP_GUSYS_WcdmaBbpPllDisable()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_GsmBbpPllEnable
 功能描述  : 本接口实现使能GSM BBP PLL。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_GsmBbpPllEnable(void);
#define DRV_GBBP_PLL_ENABLE()    BSP_GUSYS_GsmBbpPllEnable()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_GsmBbpPllDisable
 功能描述  : 本接口实现关闭GSM BBP PLL。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_GsmBbpPllDisable(void);
#define DRV_GBBP_PLL_DISABLE()    BSP_GUSYS_GsmBbpPllDisable()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_RFLdoOn
 功能描述  : 本接口实现WCDMA和GSM RF LDO上电。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_RFLdoOn(void);
#define DRV_RF_LDOUP()    BSP_GUSYS_RFLdoOn()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_RFLdoDown
 功能描述  : 本接口实现WCDMA和GSM RF LDO下电。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_RFLdoDown(void);
#define DRV_RF_LDODOWN()    BSP_GUSYS_RFLdoDown()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_WcdmaPllStatusGet
 功能描述  : 读取WCDMA BBP PLL稳定状态。
 输入参数  : 无。
 输出参数  :无。
 返 回 值  :
                     0：稳定
                     1：未稳定
*****************************************************************************/
extern unsigned long BSP_GUSYS_WcdmaPllStatusGet(void);
#define DRV_GET_WCDMA_PLL_STATUS()    BSP_GUSYS_WcdmaPllStatusGet()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_GsmPllStatusGet
 功能描述  : 读取GSM BBP PLL稳定状态。
 输入参数  : 无。
 输出参数  :无。
 返 回 值  :
                     0：稳定
                     1：未稳定
*****************************************************************************/
extern unsigned long BSP_GUSYS_GsmPllStatusGet(void);
#define DRV_GET_GSM_PLL_STATUS()    BSP_GUSYS_GsmPllStatusGet()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_DspPllStatusGet
 功能描述  : 读取DSP PLL稳定状态。
 输入参数  : 无。
 输出参数  :无。
 返 回 值  :
                     0：稳定
                     1：未稳定
*****************************************************************************/
extern unsigned long BSP_GUSYS_DspPllStatusGet(void);
#define DRV_GET_DSP_PLL_STATUS()    BSP_GUSYS_DspPllStatusGet()




/*****************************************************************************
 函 数 名  : BSP_GUSYS_ArmPllStatusGet
 功能描述  : 读取ARM PLL稳定状态。
 输入参数  : 无。
 输出参数  :无。
 返 回 值  :
                     0：稳定
                     1：未稳定
*****************************************************************************/
extern unsigned int BSP_GUSYS_ArmPllStatusGet(void);
#define DRV_GET_ARM_PLL_STATUS()    BSP_GUSYS_ArmPllStatusGet()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_BBPAutoAdjust
 功能描述  : 初始化ABB的自校准使能
 输入参数  : uiSysMode：0：WCDMA模式；1：GSM模式。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_GUSYS_BBPAutoAdjust(unsigned char ucAdjustMode);
#define DRV_BBP_AUTO_ADJUST(ucAdjustMode)    BSP_GUSYS_BBPAutoAdjust(ucAdjustMode)

/*****************************************************************************
 函 数 名  : BSP_GUSYS_DspPllEnable
 功能描述  : 打开DSP PLL
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_DspPllEnable(void);
#define DRV_DSP_PLL_ENABLE()    BSP_GUSYS_DspPllEnable()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_DspPllDisable
 功能描述  : 关闭DSP PLL
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_DspPllDisable(void);
#define DRV_DSP_PLL_DISABLE()    BSP_GUSYS_DspPllDisable()


/*****************************************************************************
 函 数 名  : BSP_GUSYS_ModeSwitchs
 功能描述  : 本接口实现WCDMA/GSM系统模式切换。
 输入参数  : uiSysMode：0：切换到WCDMA模式；1：切换到GSM模式。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_GUSYS_ModeSwitch(unsigned int uiSysMode);
#define DRV_SYSMODE_SWITCH(uiSysMode)    BSP_GUSYS_ModeSwitch(uiSysMode)


/******************************************UDI接口模块*******************************************/
/*****************************************************************************
 函 数 名  : udi_open
 功能描述  : 本接口可打开指定的UDI设备。
 输入参数  : pParam：需要打开的设备结构。
 输出参数  : 无。
 返 回 值  : -1：操作失败。
             其它值:  操作成功。
 注意事项  ：无
*****************************************************************************/
/*extern UDI_HANDLE udi_open(UDI_OPEN_PARAM *pParam);*/
#define DRV_UDI_OPEN(pParam)    udi_open(pParam)

/*****************************************************************************
 函 数 名  : udi_write
 功能描述  : 本接口可往打开的UDI设备发送数据。
 输入参数  : handle:  设备的handle
*            pMemObj: buffer内存 或 内存链表对象
*            u32Size: 数据写尺寸 或 内存链表对象可不设置
 输出参数  : 无。
 返 回 值  : -1：操作失败。
             其它值:  操作成功。
 注意事项  ：完成字节数 或 成功/失败
*****************************************************************************/
/*extern BSP_S32 udi_write(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size);*/
#define  DRV_UDI_WRITE(handle,pMemObj,u32Size)     udi_write (handle,pMemObj,u32Size)

/*****************************************************************************
* 函 数 名  : udi_read
*
* 功能描述  : 数据读
*
* 输入参数  : handle:  设备的handle
*             pMemObj: buffer内存 或 内存链表对象
*             u32Size: 数据读尺寸 或 内存链表对象可不设置
* 输出参数  :
*
* 返 回 值  : 完成字节数 或 成功/失败
*****************************************************************************/
/*extern BSP_S32 udi_read(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size);*/
#define DRV_UDI_READ(handle,pMemObj,u32Size)  udi_read(handle,pMemObj,u32Size)


/*****************************************************************************
* 函 数 名  : udi_ioctl
*
* 功能描述  : 数据通道属性配置
*
* 输入参数  : handle: 设备的handle
*             u32Cmd: IOCTL命令码
*             pParam: 操作参数
* 输出参数  :
*
* 返 回 值  : 成功/失败
*****************************************************************************/
/*extern BSP_S32 udi_ioctl(UDI_HANDLE handle, BSP_U32 u32Cmd, VOID* pParam);*/
#define DRV_UDI_IOCTL(handle,u32Cmd,pParam)    udi_ioctl(handle,u32Cmd,pParam)

/*****************************************************************************
* 函 数 名  : udi_close
*
* 功能描述  : 关闭设备(数据通道)
*
* 输入参数  : handle: 设备的handle
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
/*BSP_S32 udi_close(UDI_HANDLE handle);*/
#define DRV_UDI_CLOSE(handle)    udi_close(handle)


/****************************************** USB模块(产品定制需求，暂时打桩) *******************************************/
/*****************************************************************************
 函 数 名  : BSP_USB_SetPid
 功能描述  :
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern int BSP_USB_SetPid(unsigned char u2diagValue);
#define DRV_SET_PID(u2diagValue)    BSP_USB_SetPid(u2diagValue)

#if defined(VERSION_V7R1_C010)
/*****************************************************************************
函数名：   BSP_USB_PortTypeQuery
功能描述:  查询当前的设备枚举的端口形态值
输入参数： stDynamicPidType  端口形态
输出参数： stDynamicPidType  端口形态
返回值：   0:    查询成功
           其他：查询失败
*****************************************************************************/
unsigned int BSP_USB_PortTypeQuery(DRV_DYNAMIC_PID_TYPE_STRU *pstDynamicPidType);
#define DRV_SET_PORT_QUIRY(a)   BSP_USB_PortTypeQuery(a)
#else
/*****************************************************************************
函数名：   set_port_quiry
功能描述:  查询当前的设备枚举的端口形态值
输入参数： stDynamicPidType  端口形态
输出参数： stDynamicPidType  端口形态
返回值：   0:    查询成功
           其他：查询失败
*****************************************************************************/
unsigned int set_port_quiry(DRV_DYNAMIC_PID_TYPE_STRU *pstDynamicPidType);
#define DRV_SET_PORT_QUIRY(a)   set_port_quiry(a)
#endif

/*****************************************************************************
 函 数 名  : BSP_USB_ATProcessRewind2Cmd
 功能描述  : rewind2 CMD 处理。
 输入参数  : pData：数据。
 输出参数  : 无
 返 回 值  : false(0):处理失败
             tool(1):处理成功
*****************************************************************************/
extern int BSP_USB_ATProcessRewind2Cmd(unsigned char *pData);
#define DRV_PROCESS_REWIND2_CMD(pData)    BSP_USB_ATProcessRewind2Cmd(pData)


/*****************************************************************************
 函 数 名  : BSP_USB_GetDiagModeValue
 功能描述  : 获得设备类型。
 输入参数  : 无。
 输出参数  : ucDialmode:  0 - 使用Modem拨号; 1 - 使用NDIS拨号; 2 - Modem和NDIS共存
              ucCdcSpec:   0 - Modem/NDIS都符合CDC规范; 1 - Modem符合CDC规范;
                           2 - NDIS符合CDC规范;         3 - Modem/NDIS都符合CDC规范
 返 回 值  : VOS_OK/VOS_ERR
*****************************************************************************/
extern BSP_S32 BSP_USB_GetDiagModeValue(unsigned char *pucDialmode,
                             unsigned char *pucCdcSpec);
#define DRV_GET_DIAG_MODE_VALUE(pucDialmode, pucCdcSpec)    \
                             BSP_USB_GetDiagModeValue(pucDialmode, pucCdcSpec)


/*****************************************************************************
 函 数 名  : BSP_USB_GetPortMode
 功能描述  : 获取端口形态模式，网关对接需求，打桩。
 输入参数  : 。
 输出参数  :
 返 回 值  :
*****************************************************************************/
extern unsigned char BSP_USB_GetPortMode(char*PsBuffer, unsigned long*Length );
#define DRV_GET_PORT_MODE(PsBuffer, Length)    BSP_USB_GetPortMode(PsBuffer,Length)

/*****************************************************************************
 函 数 名  : BSP_USB_GetU2diagDefaultValue
 功能描述  : 获得端口默认u2diag值
 输入参数  : 无。
 输出参数  : 无。
 返回值：   u2diag值

*****************************************************************************/
extern BSP_U32 BSP_USB_GetU2diagDefaultValue(void);
#define DRV_GET_U2DIAG_DEFVALUE()    BSP_USB_GetU2diagDefaultValue()

/*****************************************************************************
 函 数 名  : ErrlogRegFunc
 功能描述  : USB MNTN注册异常日志接口，Porting项目中打桩
 输入参数  : 无。
 输出参数  : 无。
 返回值：   无

*****************************************************************************/
/*extern void ErrlogRegFunc(MNTN_ERRLOGREGFUN pRegFunc);*/
#define MNTN_ERRLOG_REG_FUNC(pRegFunc)

/*****************************************************************************
 函 数 名  : BSP_USB_UdiagValueCheck
 功能描述  : 本接口用于检查NV项中USB形态值的合法性.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：OK；
            -1：ERROR
*****************************************************************************/
extern int BSP_USB_UdiagValueCheck(unsigned long DiagValue);
#define DRV_UDIAG_VALUE_CHECK(DiagValue)     BSP_USB_UdiagValueCheck(DiagValue)
#define DRV_U2DIAG_VALUE_CHECK(DiagValue)     BSP_USB_UdiagValueCheck(DiagValue)

/*****************************************************************************
 函 数 名  : BSP_USB_GetLinuxSysType
 功能描述  : 本接口用于检查PC侧是否为Linux，以规避Linux后台二次拨号失败的问题.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：Linux；
            -1：非Linux。
*****************************************************************************/
extern int BSP_USB_GetLinuxSysType(void) ;
#define DRV_GET_LINUXSYSTYPE()    BSP_USB_GetLinuxSysType()



/***************************************************************SCI模块*********************************************************************/
/*************************************************************************************************************************************
 函 数 名  : BSP_SCI_Reset
 功能描述  : 本接口用于复位SCI（Smart Card Interface）驱动和USIM（Universal Subscriber Identity Module）卡。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0:  操作成功。
 注意事项  ：在卡未激活时才能调用，在函数调用前需先确定USIM卡已插入，否则会提示出错，调用无效，需要插入卡后重新调用。
*************************************************************************************************************************************/
/*extern void BSP_SCI_Reset(void);*/
#define DRV_USIMMSCI_RST()    BSP_SCI_Reset()

/********************************************************************************************************************
 函 数 名  : BSP_SCI_GetCardStatus
 功能描述  : 本接口用于获得卡当前的状态，目前LTE提供接口参数类型不一致
 输入参数  : 无。
 输出参数  :
 返 回 值  : 0:  卡处于Ready；
             -1：卡未处于Ready。
 注意事项  ：有效调用BSP_SCI_Reset()函数后，再调用此函数。
********************************************************************************************************************/
extern int BSP_GUSCI_GetCardStatus();
#define DRV_USIMMSCI_GET_CARD_STAU()    BSP_GUSCI_GetCardStatus()


/********************************************************************************************************************
 函 数 名  : BSP_SCI_SendData
 功能描述  : 本接口用于发送一段数据到USIM卡。
 输入参数  : ulDataLength：发送数据的有效长度。取值范围1～511，单位为字节。
             pucDataBuffer：发送数据所在内存的首地址，如是动态分配，调用接口后不能立即释放，依赖于硬件发完数据。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：USIMMSCIReset()函数有效调用后，再调用此函数，并确保输入参数的有效性。
********************************************************************************************************************/
/*extern int BSP_SCI_SendData(unsigned long ulDataLength, unsigned char *pucDataBuffer);*/
#define DRV_USIMMSCI_SND_DATA(ulDataLength,pucDataBuffer)\
                BSP_SCI_SendData(ulDataLength,pucDataBuffer)

/********************************************************************************************************************
 函 数 名  : USIMMSCIReceived
 功能描述  : 本接口用于USIM Manager读取来自SCI Driver的USIMMSCIPreRead()函数指定的数据量。
 输入参数  : ulDataLength：USIM Manager欲从SCI Driver读取的数据字节数。
                           取值范围应与USIMMSCIPreRead()函数指定的数据长度相同。
 输出参数  : pucDataBuffer：USIM Manager指定的Buffer，SCI Driver将数据拷贝到本Buffer。
                            取值范围是操作系统函数动态分配或者静态分配的地址。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：收到底层发的数据长度满足消息后，再调用此函数，接收的长度应该和预读的长度一致。
********************************************************************************************************************/
/*extern int BSP_SCI_ReceiveSync(unsigned long ulDataLength,unsigned char *pucDataBuffer);*/
#define DRV_USIMMSCI_RCV(ulDataLength,pucDataBuffer)    BSP_SCI_ReceiveSync(ulDataLength,pucDataBuffer)

/********************************************************************************************************************
 函 数 名  : USIMMSCIGetATR
 功能描述  : 本接口用于将Driver层缓存的ATR数据和数据个数返回给USIM Manager层。
 输入参数  : 无。
 输出参数  : ulDataLength：Driver读取的ATR数据长度，返回给USIM Manager。取值范围0～32，单位是字节。
             pucATR：USIM Manager指定的Buffer，SCI Driver将ATR数据拷贝到本Buffer。
                     一般为操作系统函数动态分配或者静态分配的地址。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：有效调用USIMMSCIReset()函数后，再调用此函数。
********************************************************************************************************************/
/*extern int BSP_SCI_GetATR(unsigned long *pulDataLength,unsigned char  *pucATR);*/
#define DRV_USIMMSCI_GET_ATR(pulDataLength,pucATR)    BSP_SCI_GetATR(pulDataLength,pucATR)

/*****************************************************************************
 函 数 名  : USIMMSCIClassSwitch
 功能描述  : 本接口用于支持PS对卡的电压类型进行切换，从1.8V切换到3V。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0： 操作成功（当前电压不是最高的，进行电压切换操作）；
             -1：操作失败（当前电压已经是最高电压，没有进行电压切换）。
 注意事项  ：有效调用USIMMSCIReset()函数后，再调用此函数。
*****************************************************************************/
/*extern int BSP_SCI_ClassSwitch(void);*/
#define DRV_USIMMSCI_CLASS_SWITCH()    BSP_SCI_ClassSwitch()

/*****************************************************************************
 函 数 名  : USIMMSCITimeStop
 功能描述  : 本接口用于支持PS关闭SIM卡时钟。有效调用USIMMSCIReset()函数后，再调用此函数。
 输入参数  : ulCardType：卡类型识别参数，USIM or SIM。
             ulTimeStopCfg：如果卡支持时钟停止模式，本参数用于设置SIM卡进入始终停止模式时的时钟电压高低状态。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：有效调用USIMMSCIReset()函数后，再调用此函数。
*****************************************************************************/
/*extern int BSP_SCI_TimeStop(unsigned long ulStopType);*/
#define DRV_USIMMSCI_TM_STOP(ulStopType)    BSP_SCI_TimeStop(ulStopType)

/*****************************************************************************
 函 数 名  : USIMMSCIDeactive
 功能描述  : 本接口用于对SIM卡的去激活操作。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：有效调用USIMMSCIReset()函数后，再调用此函数。
*****************************************************************************/
/*extern int BSP_SCI_Deactive(void);*/
#define DRV_USIMMSCI_DEACT()    BSP_SCI_Deactive()

/*****************************************************************************
 函 数 名  : USIMMSCIGetClkStatus
 功能描述  : 本接口用于获取当前SIM卡时钟状态。
 输入参数  : 无。
 输出参数  : pulSciClkStatus：变量指针，用于返回SIM卡时钟状态，
                              0：时钟已打开；1：时钟停止。
 返 回 值  : 0:  操作成功；
             -1：操作失败，指针参数为空。
 注意事项  ：有效调用USIMMSCIReset()函数后，再调用此函数。
*****************************************************************************/
/*extern int BSP_SCI_GetClkStatus(unsigned long *pulSciClkStatus);*/
#define DRV_USIMMSCI_GET_CLK_STAU(pulSciClkStatus)    BSP_SCI_GetClkStatus(pulSciClkStatus)

/*****************************************************************************
 函 数 名  : sciRecordDataSave
 功能描述  : 本接口用于获取当前SIM卡交互数据。属于异常处理模块，V7先打桩
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 0:  操作成功；
             -1：操作失败，指针参数为空。
 注意事项  ：
*****************************************************************************/
/*extern int sciRecordDataSave(void);*/
#define DRV_USIMMSCI_RECORD_DATA_SAVE()        DRV_OK

/*****************************************************************************
 函 数 名  : USIMMSCIGetPCSCParameter
 功能描述  : PCSC命令获取参数函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
/*extern unsigned long BSP_SCI_GetPCSCParameter(unsigned char *pbuf);*/
#define DRV_PCSC_GET_SCI_PARA(Para)    BSP_SCI_GetPCSCParameter(Para)


/*****************************************************************************
 函 数 名  : USIMMGetCLKFREQ
 功能描述  : PCSC命令获取时钟频率参数函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
/*extern unsigned long BSP_SCI_GetClkFreq(unsigned long *pDatalen, unsigned char *pbuf);*/
#define DRV_PCSC_GET_CLK_FREQ(pDataLen, Buffer)    BSP_SCI_GetClkFreq(pDataLen, Buffer)


/*****************************************************************************
 函 数 名  : BSP_SCI_GetBaudRate
 功能描述  : PCSC命令获取时波特率参数函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
/*extern unsigned long BSP_SCI_GetBaudRate(unsigned long *pDatalen, unsigned char *pbuf);*/
#define DRV_PCSC_GET_BAUD_RATE(pDataLen, Buffer)    BSP_SCI_GetBaudRate(pDataLen, Buffer)


/***********************************Watch Dog模块*****************************************/
/*****************************************************************************
 函 数 名  : BSP_WDT_Init
 功能描述  : 初始化定制看门狗，设置超时时间，挂接中断服务函数。
 输入参数  : wdtId：看门狗ID。
                    取值范围：若硬件提供1个看门狗，则只能取值为0；
                    若硬件提供2个看门狗，则取值为0/1。
             wdtTimeOutms：看门狗超时时间，单位毫秒。时间最大不能超过1天的时间。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：该函数只能被一个任务调用，可以是软件看门狗任务，其它模块除非用于调试，否则不得调用该看门狗作为计时工具。
*****************************************************************************/
/*extern int BSP_WDT_Init(unsigned char wdtId, unsigned long wdtTimeOutms);*/
#define DRV_WDT_INIT(wdtId,wdtTimeOutms)      BSP_WDT_Init(wdtId,wdtTimeOutms)

/*****************************************************************************
 函 数 名  : wdtEnable
 功能描述  : 开启看门狗。
 输入参数  : wdtId：看门狗ID。
                    取值范围：若硬件提供1个看门狗，则只能取值为0；
                    若硬件提供2个看门狗，则取值为0/1。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：该函数只能被一个任务调用，可以是软件看门狗任务，其它模块除非用于调试，否则不得调用该看门狗作为计时工具。
*****************************************************************************/
/*extern int BSP_WDT_Enable(unsigned char wdtId);*/
#define DRV_WDT_ENABLE(wdtId)    BSP_WDT_Enable(wdtId)

/*****************************************************************************
 函 数 名  : BSP_WDT_HardwareFeed
 功能描述  : 清除看门狗中断（喂狗）。
 输入参数  : wdtId：看门狗ID。
                    取值范围：若硬件提供1个看门狗，则只能取值为0；
                    若硬件提供2个看门狗，则取值为0/1。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：该函数只能被一个任务调用，可以是软件看门狗任务，其它模块除非用于调试，否则不得调用该看门狗作为计时工具。
*****************************************************************************/
/*extern int BSP_WDT_HardwareFeed(unsigned char wdtId);*/
#define DRV_WDT_FEED(wdtId)    BSP_WDT_HardwareFeed(wdtId)

/*****************************************************************************
 函 数 名  : BSP_WDT_Disable
 功能描述  : 关闭看门狗。
 输入参数  : wdtId：看门狗ID。
                    取值范围：若硬件提供1个看门狗，则只能取值为0；
                    若硬件提供2个看门狗，则取值为0/1。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：该函数只能被一个任务调用，可以是软件看门狗任务，其它模块除非用于调试，否则不得调用该看门狗作为计时工具。
*****************************************************************************/
/*extern int BSP_WDT_Disable(unsigned char wdtId);*/
#define DRV_WDT_DISABLE(wdtId)    BSP_WDT_Disable(wdtId)


/***********************************内存拷贝优化*****************************************/
/*****************************************************************************
 函 数 名  : __rt_memcpy
 功能描述  : 汇编版本的memcpy函数
 输入参数  : Dest :目的地址
             Src :源地址
             Count:拷贝数据的大小
 输出参数  : 无。
 返 回 值  : 目的地址。
*****************************************************************************/
extern void * __rt_memcpy(void * Dest, const void * Src, unsigned long Count);
#define DRV_RT_MEMCPY(Dest,Src,Count)       __rt_memcpy(Dest,Src,Count)


/***********************************GU DSP加载*****************************************/
/*****************************************************************************
 函 数 名  : BSP_GUDSP_LayerInfoGet
 功能描述  : 获取物理层加载信息。
 输入参数  : 无。
 输出参数  : pulAddr：存放物理层加载信息的缓存。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUDSP_LayerInfoGet(unsigned long* pulAddr, unsigned long* pulLength);
#define DRV_PHY_SLOAD_INFO_GET(pulAddr,pulLength)    BSP_GUDSP_LayerInfoGet(pulAddr,pulLength)

/*****************************************************************************
 函 数 名  : BSP_GUDSP_CommonInfoGet
 功能描述  : 获取物理层BSS COMMON段信息。
 输入参数  : 无。
 输出参数  : pulAddr：存放物理层加载信息的缓存。
 返 回 值  : 0:成功，-1:失败。
*****************************************************************************/
extern int BSP_GUDSP_CommonInfoGet(unsigned long* pulAddr, unsigned long* pulLength);
#define DRV_DSP_COMMON_INFO_GET(pulAddr,pulLength)    BSP_GUDSP_CommonInfoGet(pulAddr,pulLength)

/*****************************************************************************
 函 数 名  : BSP_GUDSP_Load
 功能描述  : 完成GU ZSP加载功能：将ZSP静态段从DDR加载到ZSP TCM。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0:成功，-1:失败。
*****************************************************************************/
extern int BSP_GUDSP_Load(void);
#define DRV_GUDSP_LOAD()    BSP_GUDSP_Load()


/***********************************MD5相关*****************************************/
/*****************************************************************************
 函 数 名  : cardlock_md5_verify
 功能描述  : 判断当前解锁码是否正确 .
 输入参数  : imei       - 单板IMEI号
             unlockcode - 解锁码
 输出参数  : 无。
 返 回 值  : 1：   解锁成功
             0:    解锁失败
*****************************************************************************/
extern int cardlock_md5_verify(char *imei, char *unlockcode);
#define DRV_CARDLOCK_MD5_VERIFY(imei, unlockcode)    cardlock_md5_verify(imei, unlockcode)


/************************************其余宏定义为空的函数******************************************/
/*****************************************************************************
 函 数 名  : UsbStatusCallbackRegister
 功能描述  : 记录pCallBack至全局变量中。SD卡功耗相关接口，V7先打桩
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
 /*extern int  UsbStatusCallbackRegister(pUsbState pCallBack);*/
#define DRV_USB_STATUS_CALLBACK_REGI(pCallBack)              DRV_OK

/*****************************************************************************
 函 数 名  : CicomClkEnable
 功能描述  : This routine enable CICOM IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void CicomClkEnable(void);*/
#define DRV_CICOM_CLK_ENABLE()

/*****************************************************************************
 函 数 名  : CicomClkDisable
 功能描述  : This routine disable CICOM IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void CicomClkDisable(void);*/
#define DRV_CICOM_CLK_DISABLE()

/*****************************************************************************
 函 数 名  : HdlcClkEnable
 功能描述  : This routine enable HDLC IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void HdlcClkEnable(void);*/
#define DRV_HDLC_CLK_ENABLE()

/*****************************************************************************
 函 数 名  : HdlcClkDisable
 功能描述  : This routine disable HDLC IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void HdlcClkDisable(void);*/
#define DRV_HDLC_CLK_DISABLE()

/*****************************************************************************
 函 数 名  : memReadApi
 功能描述  : 按32位宽度查询内存，输出4字节数据。可维可测，V7先打桩
 输入参数  : ulAddress：查询地址空间地址，地址需在内存范围内，否则返回-1。
 输出参数  : pulData：指向查询内容的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
/*extern int  memReadApi (unsigned int ulAddress, unsigned int* pulData);*/
#define DRV_MEM_READ(ulAddress,pulData)     DRV_OK

/*****************************************************************************
 函 数 名  : DrvLogInstall
 功能描述  : 打印函数注册。可维可测，V7先打桩
 输入参数  : fptr 注册的函数指针
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
/*extern int DrvLogInstall(PVOIDFUNC fptr, PVOIDFUNC fptr1, PVOIDFUNC fptr2, PVOIDFUNC fptr3, PVOIDFUNC fptr4);*/
#define DRV_LOG_INSTALL(fptr, fptr1, fptr2, fptr3, fptr4)     DRV_OK

/*****************************************************************************
 函 数 名  : ErrlogRegFunc
 功能描述  : 向底软模块注册Errlog注册函数，V7定义为空
 输入参数  : 无

 输出参数  : 无。
 返 回 值  : 无
 注意事项  ：
*****************************************************************************/
/*extern void ErrlogRegFunc(MNTN_ERRLOGREGFUN pRegFunc);*/
#define MNTN_ERRLOG_REG_FUNC(pRegFunc)


/***************************************************DPM模块 V7先定义为空*********************************************************/

//wangwei dfs
typedef  enum
{
    PWRCTRL_DFS_USB = 0,
    PWRCTRL_DFS_SD  = 1,
    PWRCTRL_DFS_PS1 = 2,
    PWRCTRL_DFS_BUTT
} PWC_DFS_ID_E;

typedef enum
{
    DFS_PROFILE_0 = 0,            /*频率级别1*/
    DFS_PROFILE_1 = 1,            /*频率级别2*/
    DFS_PROFILE_2 = 2,            /*频率级别3*/
    DFS_PROFILE_3 = 3,            /*频率级别4*/
    DFS_PROFILE_4 = 4,            /*频率级别5*/
    DFS_PROFILE_BUTT
} PWC_DFS_PROFILE_E;//PWC_DFS_PROFILE_E;

/*
 *  DFS Device QoS
 */
typedef enum tagDFS_QOS_ID_E
{
    DFS_QOS_ID_ACPU_MINFREQ_E = 0,  /*(0)*/
    DFS_QOS_ID_ACPU_MAXFREQ_E,      /*(1)*/
    DFS_QOS_ID_CCPU_MINFREQ_E,      /*(2)*/
    DFS_QOS_ID_CCPU_MAXFREQ_E,      /*(3)*/
    DFS_QOS_ID_DDR_MINFREQ_E,       /*(4)*/
    DFS_QOS_ID_DDR_MAXFREQ_E,       /*(5)*/
    DFS_QOS_ID_GPU_MINFREQ_E,       /*(6)*/
    DFS_QOS_ID_GPU_MAXFREQ_E,       /*(7)*/
    DFS_QOS_ID_BUS_MINFREQ_E,       /*(8)*/
    DFS_QOS_ID_BUS_MAXFREQ_E        /*(9)*/
}DFS_QOS_ID_E;


typedef enum tagPWC_COMM_MODE_E
{
    PWC_COMM_MODE_WCDMA,
    PWC_COMM_MODE_GSM,
    PWC_COMM_MODE_LTE,
    PWC_COMM_MODE_TDS,
    PWC_COMM_MODE_BUTT
}PWC_COMM_MODE_E;

typedef enum tagPWC_COMM_MODULE_E
{
    PWC_COMM_MODULE_PA,
    PWC_COMM_MODULE_RF,
    PWC_COMM_MODULE_BBP_SOC,
    PWC_COMM_MODULE_BBP_PWR,
    PWC_COMM_MODULE_DSP,
    PWC_COMM_MODULE_ABB,
    PWC_COMM_MODULE_HIFI,
    PWC_COMM_MODULE_BUTT
}PWC_COMM_MODULE_E;

/* 睡眠投票ID */
typedef enum tagPWC_CLIENT_ID_E
{
    PWRCTRL_SLEEP_SCI       = 0x100,
    PWRCTRL_SLEEP_PS        = 0x101,
    PWRCTRL_SLEEP_BBPMASTER = 0x102,
    PWRCTRL_SLEEP_CICOM     = 0x103,
    PWRCTRL_SLEEP_NV        = 0x104,

    PWRCTRL_SLEEP_DMA       = 0x108,
    PWRCTRL_SLEEP_FLASH     = 0x109,
    PWRCTRL_SLEEP_SD        = 0x10A,
    PWRCTRL_SLEEP_FTM       = 0x10B,

    PWRCTRL_SLEEP_BOOT      = 0x110,
    PWRCTRL_SLEEP_ACPU      = 0x111,

    PWRCTRL_SLEEP_WIFI      = 0x114,
    PWRCTRL_SLEEP_USB       = 0x115,
    PWRCTRL_SLEEP_LCD       = 0x116,
    PWRCTRL_SLEEP_NCM     	= 0x117,
    PWRCTRL_SLEEP_ACM     	= 0x118,
    PWRCTRL_SLEEP_ICC     	= 0x119,
    PWRCTRL_SLEEP_NAS     	= 0x11a,
    PWRCTRL_SLEEP_LTEPS     = 0x11b,

    /* Test */
    PWRCTRL_TEST_LIGHTSLEEP = 0x11E,
    PWRCTRL_TEST_DEEPSLEEP  = 0x11F,
    PWRCTRL_SLEEP_RNIC      = 0x120,
    PWRCTRL_SLEEP_DSFLOW    = 0x121,
    PWRCTRL_CLIENT_BUTT     = 0x122
} PWC_CLIENT_ID_E;

typedef enum tagPWC_TEMP_PROTECT_E
{
	PWC_TEMP_OLED,
	PWC_TEMP_BATTERY,
	PWC_TEMP_L_PA,
	PWC_TEMP_G_PA,
	PWC_TEMP_W_PA,
	PWC_TEMP_SIM_CARD,
	PWC_TEMP_DCXO,
	PWC_TEMP_BUTT
}PWC_TEMP_PROTECT_E;

typedef struct
{
    PVOIDFUNC drx_slow;
} PWC_SLEEP_CALLBACK_STRU;

/*****************************************************************************
 函 数 名  : dpm_armsleep
 功能描述  : ARM进入 normal WFI
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/* A13 回片，不进行低功耗功能联调 */
#define DRV_ARM_SLEEP()

/*****************************************************************************
 函 数 名  : dpm_lightsleep
 功能描述  : ARM进入 normal WFI 总线时钟停止
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/* A13 回片，不进行低功耗功能联调 */
#define DRV_LIGHT_SLEEP()

/*****************************************************************************
 函 数 名  : dpm_deepsleep
 功能描述  : ARM进入 sleep
 输入参数  : 0 没有USB插入 1 有USB插入
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
/* A13 回片，不进行低功耗功能联调 */
#define DRV_DEEP_SLEEP(sleepmode)

/*****************************************************************************
 函 数 名  : dpm_sleep_callback_register
 功能描述  : 注册DPM函数给底软使用
 输入参数  : 待注册的函数
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
/* A13 回片，不进行低功耗功能联调 */
#define DRV_DPM_SLEEP_CALLBACK_REG(Callback)
/*****************************************************************************
 函 数 名  : dpm_sleep_ahb_bak_addr_alloc
 功能描述  : arm上下电功能，需要备份ahb数据，该接口申请数据保存地址和长度
 输入参数  : length     保存申请空间大小的指针
 输出参数  : 无
 返 回 值  : 申请空间的地址 。
*****************************************************************************/
#if defined (VERSION_V3R2)
extern void * dpm_sleep_ahb_bak_addr_alloc(unsigned long * length);
#define DRV_DPM_SLEEP_AHB_BAK_ADDR_ALLOC(length)  dpm_sleep_ahb_bak_addr_alloc(length)
#else
#define DRV_DPM_SLEEP_AHB_BAK_ADDR_ALLOC(length)
#endif
/*****************************************************************************
 函 数 名  : dpm_bbp_powerdown
 功能描述  : BBP下电
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/* A13 回片，不进行低功耗功能联调 */
#define DRV_BBP_POWERDOWN()

/*****************************************************************************
 函 数 名  : pwrctrl_bbp_powerdown_adjust
 功能描述  : BBP下电,控SOC
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
#define DRV_BBP_POWERDOWN_ADJUST()
/*****************************************************************************
 函 数 名  : dpm_bbp_powerup
 功能描述  : BBP上电
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/* A13 回片，不进行低功耗功能联调 */
#define DRV_BBP_POWERUP()
/*****************************************************************************
 函 数 名  : pwrctrl_bbp_powerup_adjust
 功能描述  : BBP上电,控SOC
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
#define DRV_BBP_POWERUP_ADJUST()

/*****************************************************************************
 函 数 名  : dpm_dfs_exec
 功能描述  : 调整ARM频率
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/* A13 回片，不进行低功耗功能联调 */
#define DRV_DFS_EXEC(en_curr_profile,en_new_profile)      0

/*****************************************************************************
 函 数 名  : pwrctrl_rf_pwr_down
 功能描述  : RF下电
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/* A13 回片，不进行低功耗功能联调 */
#define DRV_RF_POWERDOWN()

/*****************************************************************************
 函 数 名  : pwrctrl_rf_pwr_up
 功能描述  : RF上电
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/* A13 回片，不进行低功耗功能联调 */
#define DRV_RF_POWERUP()

/*****************************************************************************
 函 数 名  : pwrctrl_rf_pd_io_set
 功能描述  : RF下电，BB输入上下拉
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/* A13 回片，不进行低功耗功能联调 */
#define DRV_RF_DOWN_BB_SET()

/*****************************************************************************
 函 数 名  : pwrctrl_rf_pu_io_set
 功能描述  : RF上电，BB输入上下拉
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/* A13 回片，不进行低功耗功能联调 */
#define DRV_RF_UP_BB_SET()

/*****************************************************************************
 函 数 名  : pwrctrl_usb_lowpower_enter
 功能描述  : USB进入低功耗
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
#define USB_POWER_OFF()

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_SleepVoteLock
 功能描述  : 外设禁止睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
#define DRV_PWRCTRL_SLEEPVOTE_LOCK(enClientId) DRV_OK
/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_SleepVoteUnLock
 功能描述  : 外设允许睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
#define DRV_PWRCTRL_SLEEPVOTE_UNLOCK(enClientId) DRV_OK
/*************************************************
 函 数 名       : secureSupport
 功能描述   : 当前版本是否支持安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
#define DRV_SECURE_SUPPORT(pData)    DRV_OK


/*************************************************
 函 数 名       : secureAlreadyUse
 功能描述   : 查询当前版本是否已经启用安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
#define DRV_SECURE_ALREADY_USE(pData)    DRV_OK

/*************************************************
 函 数 名       : startSecure
 功能描述   : 启用安全启动
 输入参数   :
 输出参数   :
 返 回 值      : OK/ERROR
*************************************************/
#define DRV_START_SECURE()    DRV_OK


/********************************************************
函数说明： 返回当前设备列表中支持(sel=1)或者不支持(sel=0)PCSC的设备形态值
函数功能:
输入参数：sel
          0: 通过参数dev_type返回当前不带PCSC是设备形态值
          1：通过参数dev_type返回当前带PCSC是设备形态值
输出参数：dev_type 写入需要的设备形态值，如果没有则不写入值。
          NV中存储的设备形态值
输出参数：pulDevType 与ulCurDevType对应的设备形态值，如果没有返回值1。
返回值：
          0：查询到相应的设备形态值；
          1：没有查询到响应的设备形态值。
********************************************************/
int BSP_USB_PcscInfoSet(unsigned int  ulSel, unsigned int  ulCurDevType, unsigned int *pulDevType);
#define DRV_USB_PCSC_INFO_SET(ulSel, ulCurDevType, pulDevType)    BSP_USB_PcscInfoSet(ulSel, ulCurDevType, pulDevType)



/********************************************************
函数说明：协议栈查询HSIC枚举状态
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  1：枚举完成
          0：枚举未完成
********************************************************/
extern unsigned int BSP_GetHsicEnumStatus(void);
#define DRV_GET_HSIC_ENUM_STATUS() BSP_GetHsicEnumStatus()


/********************************************************
函数名：   BSP_UDI_FindVcom
功能描述： 查询当前设备形态下指定的UDI虚拟串口端口是否存在
输入参数： UDI_DEVICE_ID枚举值，即待查询的虚拟串口端口ID
输出参数： 无
返回值：
           0：当前设备形态不支持查询的虚拟串口端口；
           1：当前设备形态支持查询的虚拟串口端口。
注意事项： 无
********************************************************/
extern int BSP_UDI_FindVcom(UDI_DEVICE_ID enVCOM);

/********************************************************
函数说明： 返回当前NV项中的设备形态值，是否支持PCSC
函数功能:
输入参数： dev_type是设备形态值（OAM从NV中读出）

输出参数： 无。
返回值：
          0: dev_type不支持PCSC设备；
          1：dev_type支持PCSC设备
********************************************************/
/*unsigned int pcsc_info_quiry(unsigned int ulDevType);*/
#define DRV_USB_PCSC_INFO_QUIRY(a)            DRV_OK


/*****************************************************************************
 函 数 名  : pcsc_usim_int
 功能描述  : 注册PCSC命令接收函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
/*extern void pcsc_usim_int(pPCSCFUNC pFunc);*/
#define DRV_PCSC_REG_CALLBACK(pFunc)


/*****************************************************************************
 函 数 名  : pcsc_usim_ctrl_cmd
 功能描述  : PCSC命令回复函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
/*extern unsigned long pcsc_usim_ctrl_cmd(unsigned long CmdTpye, unsigned long Result,
                                        unsigned char *Buf, unsigned long Length);*/
#define DRV_PCSC_SEND_DATA(CmdType, Result, Buffer, Length)

#if 0
/* Porting联编，暂时定义为空 */
#define DRV_COM_RCV_CALLBACK_REGI(uPortNo,pCallback)    DRV_OK
#endif
extern signed int Sock_RecvCallbackRegister(unsigned char  uPortNo, pComRecv pCallback);
#define DRV_COM_RCV_CALLBACK_REGI(uPortNo,pCallback)\
                Sock_RecvCallbackRegister(uPortNo,pCallback)

/*****************************************************************************
 函 数 名  : pcsc_usim_ctrl_cmd
 功能描述  : PCSC命令回复函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/

extern BSP_VOID* cacheDmaMalloc(BSP_U32  bytes);
#define DRV_CACHEDMAM_ALLOC(bytes) cacheDmaMalloc(bytes)

/*****************************************************************************
 函 数 名  : pcsc_usim_ctrl_cmd
 功能描述  : PCSC命令回复函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_S32 cacheDmaFree(BSP_VOID*  pBuf);
#define DRV_CACHEDMAM_FREE(pBuf) cacheDmaFree(pBuf)

/*****************************************************************************
 函 数 名  : usbModemRegFastCpyFunc
 功能描述  : 协议栈注册高效复制函数到底软
 输入参数  : FUNCPTR srcFunc , 高效复制函数指针
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int usbModemRegFastCpyFunc
(
  USB_MODEM_COPY pCopyFunc
);
#define DRV_MODEM_REG_FAST_COPY_FUNC(pFunc) \
    usbModemRegFastCpyFunc(pFunc)

/*****************************************************************************
 函 数 名  : Sock_Send
 功能描述  : 实现USB串口通道的数据发送功能，将上层软件传递的待发送数据直接传递至USB端点驱动程序发往HOST侧。
 输入参数  : uPortNo：串口实例号。
             pData：缓冲区指针。
             uslength：发送字节数。
 输出参数  : 无。
 返 回 值  : 0：操作成功；
             其它值：操作失败
 注意事项  ：
 1、由于USB R接口模块串口的发送缓冲区的大小固定为USBCOMS_MAX_BUFSIZE（8192）中所定义的长度，
    所以在调用该函数进行数据发送时，待发送数据缓冲区的长度应不大于USBCOMS_MAX_BUFSIZE所指定的长度。
 2、数据缓冲区指针要求4字节对齐。
 3、串口号定义OM为1,AT为2,DLOAD为2（AT与DLOAD不同时工作）。
 4、该接口属于阻塞式接口,即数据发送成功后才返回。
*****************************************************************************/
extern VOS_INT32 Sock_Send(VOS_UINT8 ucPortNo, VOS_UINT8* pucData, VOS_UINT16 uslength);
#define DRV_COM_SEND(uPortNo,pData,uslength) Sock_Send(uPortNo,pData,uslength)

/*****************************************************************************
 函 数 名  : UartRecvCallbackRegister
 功能描述  : 提供上层应用程序注册数据接收回调函数指针的API接口函数。
 输入参数  : uPortNo：串口实例号。
             pCallback：回调函数指针。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int  UartRecvCallbackRegister(unsigned char  uPortNo, pUartRecv pCallback);
#define DRV_UART_RCV_CALLBACK_REGI(uPortNo,pCallback)\
                UartRecvCallbackRegister(uPortNo,pCallback)

/*****************************************************************************
 函 数 名  : pwrctrl_wpa_pwr_up
 功能描述  : RF下电
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void pwrctrl_wpa_pwr_up(void);
#define DRV_WPA_POWERUP()

/*****************************************************************************
 函 数 名  : pwrctrl_wpa_pwr_up
 功能描述  : RF下电，BB输入上下拉
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void pwrctrl_wpa_pwr_down(void);
#define DRV_WPA_POWERDOWN()

/************************************************************************
 * FUNCTION
 *       vmEnable
 * DESCRIPTION
 *       内存读写保护功能使能函数
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern int vmEnable(int enable);
#define DRV_VM_ENABLE(flag)   vmEnable(flag)


extern unsigned int drvSecCheck(void);
#define DRV_SEC_CHECK() drvSecCheck()

/*****************************************************************************
 函 数 名  : vmStateSet
 功能描述  : 设置地址空间状态
 输入参数  : 无

 输出参数  : 无。
 返 回 值  : 无
 注意事项  ：
*****************************************************************************/
#define DRV_VM_STATESET(context, virtAdrs, len, stateMask, state)

/*****************************************************************************
 函 数 名  : pwrctrl_gpa_pwr_down
 功能描述  : RF上电
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void pwrctrl_gpa_pwr_down(void);
#define DRV_GPA_POWERDOWN()


extern int GetAuthVer(void);
#define DRV_GET_AUTH_VER()  GetAuthVer()

/*****************************************************************************
 函 数 名  : hkadcBatADCRead
 功能描述  : 获取电池电压采样值
 输入参数  : 无
 输出参数  : pTemp：        指向电池温度的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int hkadcBatADCRead(int* pslData);
#define DRV_GET_BATTERY_ADC(pslData) hkadcBatADCRead(pslData)

/*************************************************
*  Function:  wifi_end_athr_tcmd_init
*  Description: 设置WIFI测试状态
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          x:     控制WIFI的PA状态
*  Output:
*         N/A
*  Return:
*         函数执行的状态结果值；
*************************************************/
int wifi_end_athr_tcmd_init(int x);
#define WIFI_TEST_CMD_INIT(x) wifi_end_athr_tcmd_init(x)

/*****************************************************************************
 函 数 名  : ComPnpCallbackRegister
 功能描述  : 提供上层应用程序注册USB插入/拔出回调函数指针的API接口函数。
 输入参数  : pInsert：USB插入回调函数指针。
             pPullout：USB拔出回调函数指针。
 输出参数  : 无。
 返 回 值  : 0：操作成功；
             其它值：操作失败。
 注意事项  ：该接口目前没有应用，暂保留。
*****************************************************************************/
extern int  ComPnpCallbackRegister(void* pInsert, void* pPullout);
#define DRV_COM_PNP_CALLBACK_REGI(pInsert, pPullout)\
                ComPnpCallbackRegister(pInsert, pPullout)

/*****************************************************************************
 函 数 名  : drvOnLineUpdateResult
 功能描述  : WebUI模块函数注册。
 输入参数  : 无
 输出参数  : 无。
 返 回 值  : 无
 注意事项  ：
*****************************************************************************/
extern void drvOnLineUpdateResult(void);
#define DRV_ONLINE_UPDATE_RESULT()    drvOnLineUpdateResult()

/*****************************************************************************
 函 数 名  : pwrctrl_gpa_pwr_up
 功能描述  : RF上电
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void pwrctrl_gpa_pwr_up(void);
#define DRV_GPA_POWERUP()

/*****************************************************************************
 函 数 名  : MNTN_RecordVersionInfo
 功能描述  : 输出版本信息到Errlog
 输入参数  : 无

 输出参数  : 无。
 返 回 值  : 无
 注意事项  ：传入指针不能为NULL
*****************************************************************************/
extern void  MNTN_RecordVersionInfo(void);

/*****************************************************************************
 函 数 名  : usbModemRegExtFunc
 功能描述  : AT接收数据或管脚信号函数注册
 输入参数  : FUNCPTR srcFunc , 协议栈BULK数据接收回调函数
FUNCPTR statusSet, 协议栈消息通道状态变化数据接收回调函数
FUNCPTR connectBrk, 协议栈断链回调函数
FUNCPTR connectEst ，协议栈建链回调函数
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int usbModemRegExtFunc
(
 USB_MODEM_RECV_CALL srcFunc ,
 USB_MODEM_AT_Sig statusSet,
 USB_MODEM_IN_OUT connectBrk,
 USB_MODEM_IN_OUT connectEst
 );
#define DRV_MODEM_REG_EXT_FUNC(srcFunc,statusSet,connectBrk,connectEst)\
    usbModemRegExtFunc(srcFunc,statusSet,connectBrk,connectEst)

/*****************************************************************************
 函 数 名  : USB_AT_SendForProtocal
 功能描述  : USB 提供给协议栈数据发送接口函数
 输入参数  : buffer-要传输的字符串
                           Lenth---字符串的长度
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int USB_AT_SendForProtocal(unsigned char* buffer,  unsigned long length);
#define DRV_AT_SEND_FOR_PROTOCAL(buffer,length)   USB_AT_SendForProtocal(buffer,length)

/*****************************************************************************
 函 数 名  : USB_AT_SetMode
 功能描述  : 提供给协议栈中断通道上报
                           UE单板相关状态给PC主机
 输入参数  : pMscStru -pointer to MODEM_MSC_STRU
 输出参数  : 无。
 返 回 值  : 无
*****************************************************************************/
extern void USB_AT_SetMode(MODEM_MSC_STRU *pMscStru);
#define DRV_AT_SET_MODE(pMscStru)   USB_AT_SetMode(pMscStru)

/*****************************************************************************
 函 数 名  : usb_modem_for_ps_flowctrl
 功能描述  : 提供给协议栈调用流控解流控接口
 输入参数  : MODEM_MSC_STRU *pMscStru
 输出参数  : 无。
 返 回 值  : 无
*****************************************************************************/
extern void usb_modem_for_ps_flowctrl(MODEM_MSC_STRU *pMscStru);
#define DRV_MODEM_FOR_PS_FLOWCTRL(pMscStru)     usb_modem_for_ps_flowctrl(pMscStru)

/*****************************************************************************
 函 数 名  : ComRecvCallbackRegister
 功能描述  : 提供上层应用程序注册串口状态通知回调函数指针的API接口函数。
 输入参数  : uPortNo：串口实例号。
             pCallback：回调函数指针。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int ComStatusCallbackRegister(unsigned char uPortNo, pComStatusNotify pCallback);
#define DRV_COM_STATUS_CALLBACK_REGI(uPortNo,pCallback)\
                ComStatusCallbackRegister(uPortNo,pCallback)

/*****************************************************************************
 函 数 名  : GetComStatus
 功能描述  : 底软提供的接口，用来判断USB设备是否已经准备好，可以接收数据。
 输入参数  : no：串口实例号。
 输出参数  : 无
 返 回 值  : OK(0):设备ready
             ERROR(-1):设备没有ready
*****************************************************************************/
extern int  GetComStatus(unsigned char no);
#define DRV_GetComStatus(no) GetComStatus(no)

/*****************************************************************************
 函 数 名  : UartSend
 功能描述  : 实现UART串口通道的数据发送功能，将上层软件传递的待发送数据直接传递至UART端点驱动程序发往HOST侧。
 输入参数  : uPortNo：  串口实例号。
             pData：    缓冲区指针。
             uslength： 发送字节数。
 输出参数  : 无。
 返 回 值  : 0：操作成功；
             其它值：操作失败。
 注意事项  ：该接口目前没有应用，暂保留。
*****************************************************************************/
extern int  UartSend (unsigned char  uPortNo, unsigned char  *pData, unsigned short  uslength);
#define DRV_UART_SEND(uPortNo,pData,uslength)    UartSend(uPortNo,pData,uslength)

/*****************************************************************************
 函 数 名  : usb_modem_for_ps_notify
 功能描述  : 释放信号量给notify任务
 输入参数  : 无
 输出参数  : 无。
 返 回 值  : 无
*****************************************************************************/
extern void usb_modem_for_ps_notify(void);
#define DRV_MODEM_FOR_PS_NOTIFY()       usb_modem_for_ps_notify()

/************************************************************************
 * FUNCTION
 *       max_freeblock_size_get
 * DESCRIPTION
 *       get memory max free block size
 * INPUTS
 *       无
 * OUTPUTS
 *       max free block size
 *************************************************************************/
extern int max_freeblock_size_get(void);
#define DRV_GET_FREE_LOCK_SIZE() max_freeblock_size_get()

/*****************************************************************************
 函 数 名  : getDloadVersion
 功能描述  : 查询当前下载协议版本
 输入参数  : unsigned char *buf
                          int len
 输出参数  : 无
 返 回 值  : 0:  操作成功；
                      -1：操作失败。
*****************************************************************************/
extern int getDloadVersion(unsigned char *buf, int len);
#define DRV_GET_DLOAD_VERSION(buf,len) getDloadVersion(buf,len)


/*****************************************************************************
 函 数 名  : getDloadNetMode
 功能描述  : 查询网络模式
 输入参数  : unsigned long*netMode
 输出参数  : 无
 返 回 值  : 0:  操作成功；
                      -1：操作失败。
*****************************************************************************/
extern int getDloadNetMode(unsigned long *netMode);
#define DRV_GET_DLOAD_NETMODE(netMode) getDloadNetMode (netMode)


extern int GetAuthVer(void);
#define DRV_GET_AUTH_VER()  GetAuthVer()

/*****************************************************************************
 函 数 名  : getDloadFlashInfo
 功能描述  : 获取下载Flash信息
 输入参数  : DLOAD_FLASH_STRU *flashInfo
 输出参数  : 无
 返 回 值  : 0:  操作成功；
                      -1：操作失败。
*****************************************************************************/
extern int getDloadFlashInfo(DLOAD_FLASH_STRU *flashInfo);
#define DRV_GET_DLOAD_FLASHINFO(flashinfo) getDloadFlashInfo (flashinfo)

/*****************************************************************************
 函 数 名  : pmLDOOFF
 功能描述  : 本接口用于关闭某路LDO的电压输出。
 输入参数  : ucLDO：指示某路LDO电压。取值范围为1～12。
 输出参数  : 无。
 返 回 值  : 无。
 注意事项　：LDO6由硬件控制，软件不支持对LDO6的操作。如果对LDO1关闭，则系统会关机。
*****************************************************************************/
extern void pmLDOOFF(unsigned char ucLDO);
#define DRV_PM_LDO_OFF(ucLDO)                 pmLDOOFF(ucLDO)

/*****************************************************************************
 函 数 名  : pmLDOON
 功能描述  : 本接口用于设置打开某路LDO的电压输出。
 输入参数  : ucLDO：指示某路LDO电压。取值范围为2～12。
 输出参数  : 无。
 返 回 值  : 无。
 注意事项　：不支持对LDO1，LDO6的操作。LDO1是在系统开机时自动打开，一旦关闭则整个系统会下电；LDO6是由硬件管脚控制。
*****************************************************************************/
extern void pmLDOON(unsigned char ucLDO);
#define DRV_PM_LDO_ON(ucLDO)                  pmLDOON(ucLDO)

/************************* DMA BEGIN ****************************************/

/**************************************************************************
  函数声明
**************************************************************************/
/*****************************************************************************
* 函 数 名  : BSP_EDMA_CheckChannelBusy
*
* 功能描述  : 判断通道是否空闲
*
* 输入参数  : u32Channel：待判断的通道号
* 输出参数  : 无
* 返 回 值  : DMA_SUCCESS       通道空闲
*             BSP_ERR_DMA_CHANNEL_BUSY  通道忙
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*
* 修改记录  :2009年06月20日   吴振  创建

*****************************************************************************/
extern BSP_S32 BSP_EDMA_CheckChannelBusy(BSP_U32 u32Channel);
#define DRV_EDMA_CHECK_CHANNEL_BUSY(u32Channel) BSP_EDMA_CheckChannelBusy(u32Channel)

/*****************************************************************************
* 函 数 名  : BSP_EDMA_SingleBlockStart
*
* 功能描述  : 单块数据传输启动
*
* 输入参数  : DMA_SINGLE_BLOCK_S stDMASingleBlock   单块传输使用的参数结构体
*
* 输出参数  : 无
* 返 回 值  : DMA_SUCCESS    传输启动完成
*          DMA_CHANNEL_INVALID   传入通道号错误
*          DMA_TRXFERSIZE_INVALID   传入传输长度非法
*          DMA_CHANNEL_BUSY         通道忙
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
extern BSP_S32 BSP_EDMA_SingleBlockStart(DMA_SINGLE_BLOCK_S * stDMASingleBlock);
#define DRV_EDMA_SINGLE_BLOCK_START(stDMASingleBlock) BSP_EDMA_SingleBlockStart(stDMASingleBlock)

/*****************************************************************************
* 函 数 名  : BSP_EDMA_LliBuild
*
* 功能描述  : 链表构建函数
*
* 输入参数  : BSP_U32 ulChan        通道号
*             DMA_LLI_S *pFirstLLI  资源链表的第一个节点指针
* 输出参数  : 无
* 返 回 值  : BSP_VOID *  指向底层建立好的传输链表的头节点的指针
*
* 修改记录  :2009年7月24日   吴振  创建

*****************************************************************************/
extern BSP_VOID * BSP_EDMA_LliBuild(BSP_U32 ulChan, DMA_LLI_S *pFirstLLI);
#define DRV_EDMA_LLI_BUILD(ulChan,pFirstLLI) BSP_EDMA_LliBuild(ulChan,pFirstLLI)

/*****************************************************************************
* 函 数 名  : BSP_EDMA_LliStart
*
* 功能描述  : 链表数据传输启动专用函数
*
* 输入参数  : BSP_U32 ulChan        通道号
*             BSP_VOID *pu32Head：   调用BSP_DMA_LliBuild获得的待传输链表头指针
* 输出参数  : 无
* 返 回 值  : DMA_SUCCESS    传输启动完成
*          DMA_LLIHEAD_ERROR   传入的链表头错误
*          DMA_CHANNEL_INVALID   传入通道号错误
*          DMA_ERROR    通道不空闲或者DMA未正确初始化
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
extern BSP_U32 BSP_EDMA_LliStart(DMA_CHN_PARA_S * stChnPara, BSP_VOID *pu32Head);
#define DRV_EDMA_LLI_START(stChnPara,pu32Head) BSP_EDMA_LliStart(stChnPara,pu32Head)

/*****************************************************************************
* 函 数 名  : BSP_EDMA_GetRegistValue
*
* 功能描述  : 获取异常时候的EDMAC模块寄存器的值
*
* 输入参数  : BSP_VOID
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
extern BSP_VOID BSP_EDMA_GetRegistValue(BSP_VOID);
#define DRV_EDMA_GET_REGIST_VALUE(BSP_VOID) BSP_EDMA_GetRegistValue(BSP_VOID)

/*************************DMA END************************/

/*****************************************************************************
 函 数 名  : pwrctrl_abb_powerdown
 功能描述  : ABB下电
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void pwrctrl_abb_powerdown(void);
#define DRV_ABB_POWERDOWN()                       pwrctrl_abb_powerdown()

/*****************************************************************************
 函 数 名  : pwrctrl_abb_powerup
 功能描述  : ABB上电
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void pwrctrl_abb_powerup(void);
#define DRV_ABB_POWERUP()                       pwrctrl_abb_powerup()

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_GetTemp
 功能描述  : 获取OLED、电池、GPA、WPA、SIM卡、DCXO的温度。
 输入参数  : enModule:PWC_TEMP_PROTECT_E
             hkAdcTable:电压温度换算表
 输出参数  : pslData:存储的电压转换为的温度值
 返 回 值  : HKADC_OK/HKADC_ERROR 。
*****************************************************************************/
#if defined (VERSION_V3R2)
extern BSP_S32 BSP_PWRCTRL_GetTemp(PWC_TEMP_PROTECT_E enModule, BSP_U16 *hkAdcTable,BSP_S32 *pslData);
#define DRV_PWRCTRL_GET_TEMP(enModule,hkAdcTable,pslData) BSP_PWRCTRL_GetTemp(enModule,hkAdcTable,pslData)
#else
#define DRV_PWRCTRL_GET_TEMP(enModule,hkAdcTable,pslData) DRV_OK
#endif

/********************************************************************************************************
 函 数 名  : getHwVersionIndex
 功能描述  : 获取由HKADC读取的硬件ID经过拼装后的硬件索引值
 输入参数  : 无
 输出参数  : 硬件ID的索引值

 返 回 值  : 非-1:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int getHwVersionIndex(void);
#define DRV_GET_HW_VERSION_INDEX()       getHwVersionIndex()

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PwrUp
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB的上电控制。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
#if defined (VERSION_V3R2)
extern BSP_U32 BSP_PWRCTRL_PwrUp (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);
#define DRV_PWRCTRL_PWRUP(enCommMode,enCommModule) BSP_PWRCTRL_PwrUp(enCommMode,enCommModule)
#else
#define DRV_PWRCTRL_PWRUP(enCommMode,enCommModule) DRV_OK
#endif
/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PwrDown
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB的下电控制。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
#if defined (VERSION_V3R2)
extern BSP_U32 BSP_PWRCTRL_PwrDown (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);
#define DRV_PWRCTRL_PWRDOWN(enCommMode,enCommModule) BSP_PWRCTRL_PwrDown(enCommMode,enCommModule)
#else
#define DRV_PWRCTRL_PWRDOWN(enCommMode,enCommModule) DRV_OK
#endif
/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PllEnable
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL使能。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
#if defined (VERSION_V3R2)
extern BSP_U32 BSP_PWRCTRL_PllEnable (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);
#define  DRV_PWRCTRL_PLL_ENABLE(enCommMode,enCommModule) BSP_PWRCTRL_PllEnable(enCommMode,enCommModule)
#else
#define  DRV_PWRCTRL_PLL_ENABLE(enCommMode,enCommModule) DRV_OK
#endif
/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PllDisable
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL去使能。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
#if defined (VERSION_V3R2)
extern BSP_U32 BSP_PWRCTRL_PllDisable (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);
#define  DRV_PWRCTRL_PLL_DISABLE(enCommMode,enCommModule) BSP_PWRCTRL_PllDisable(enCommMode,enCommModule)
#else
#define  DRV_PWRCTRL_PLL_DISABLE(enCommMode,enCommModule) DRV_OK
#endif

typedef enum tagMSP_PROC_ID_E
{
    OM_REQUEST_PROC = 0,
    OM_ADDSNTIME_PROC = 1,
    OM_PRINTF_WITH_MODULE = 2,
    OM_PRINTF = 3,

    OM_PRINTF_GET_MODULE_IDLEV = 4,
    OM_READ_NV_PROC = 5,
    OM_WRITE_NV_PROC = 6,
    OM_MNTN_ERRLOG = 7,

    MSP_PROC_REG_ID_MAX
}MSP_PROC_ID_E;

typedef void (*BSP_MspProc)(void);
/*****************************************************************************
* 函 数 名  : BSP_MspProcReg
*
* 功能描述  : DRV提供给OM的注册函数
*
* 输入参数  : MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
#if defined (VERSION_V3R2)
extern void BSP_MspProcReg(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc);
#define DRV_MSP_PROC_REG(eFuncID, pFunc)  BSP_MspProcReg(eFuncID, pFunc)
#else
extern void BSP_MspProcReg(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc);
#define DRV_MSP_PROC_REG(eFuncID, pFunc)
#endif

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_SleepCallbackRegister
 功能描述  : 注册PWC函数给底软使用
 输入参数  : 待注册的函数
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
#if defined (VERSION_V3R2)
extern int BSP_PWRCTRL_SleepCallbackRegister(PWC_SLEEP_CALLBACK_STRU pCallback);
#define DRV_PWRCTRL_SLEEP_CALLBACK_REG(Callback)  BSP_PWRCTRL_SleepCallbackRegister(Callback)
#else
#define DRV_PWRCTRL_SLEEP_CALLBACK_REG(Callback)  DRV_OK
#endif

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_AhbBakAddrAlloc
 功能描述  : arm上下电功能，需要备份ahb数据，该接口申请数据保存地址和长度
 输入参数  : length     保存申请空间大小的指针
 输出参数  : 无
 返 回 值  : 申请空间的地址 。
*****************************************************************************/
#if defined (VERSION_V3R2)
extern void * BSP_PWRCTRL_AhbBakAddrAlloc(unsigned int * length);
#define DRV_PWRCTRL_AHB_BAK_ADDR_ALLOC(length) BSP_PWRCTRL_AhbBakAddrAlloc(length)
#else
#define DRV_PWRCTRL_AHB_BAK_ADDR_ALLOC(length) DRV_OK
#endif



extern BSP_S32 BSP_MNTN_GetBattState(void);
#define DRV_MNTN_GET_BATT_STATE() BSP_MNTN_GetBattState()

/* Modified by z40661 for PC工程, 2012-02-25 begin */

typedef  unsigned long OSM_WDOG_ID;
typedef enum tagSYNC_MODULE_EX_E
{
    SYNC_MODULE_MEM     = 0,
    SYNC_MODULE_NANDC,
    SYNC_MODULE_PMU,
    SYNC_MODULE_SOCP,
    SYNC_MODULE_DMA,
    SYNC_MODULE_NV,
    SYNC_MODULE_YAFFS,
    SYNC_MODULE_MSP,
    SYNC_MODULE_BUTT
}SYNC_MODULE_E;
/* Modified by z40661 for PC工程, 2012-02-25 end */

/*****************************************************************************
*  Function:  drv_get_local_usr_connect_status
*  Description: 返回当前是否有USB连接或者WIFI用户连接
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*  Output:
*  Return:
*         1 : 有
*         0 : 无
*************************************************/
extern unsigned long  drv_get_local_usr_connect_status(void);
#define DRV_AT_GET_USER_EXIST_FLAG() drv_get_local_usr_connect_status()

/* Added by l60609 for B060 Project, 2012-2-20, Begin   */

void pwrctrl_wifi_lowpower_enter(void);
#define WIFI_POWER_OFF()  pwrctrl_wifi_lowpower_enter()
extern BSP_VOID oledPwrOff(BSP_VOID);

extern void BSP_PWRCTRL_UsbLowPowerEnter(void);

#define DRV_PWRCTRL_USB_LOWPOWER_ENTER() BSP_PWRCTRL_UsbLowPowerEnter()

/* Added by l60609 for B060 Project, 2012-2-20, End   */

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_DeepSleepForever
 功能描述  : AARM CARM 下电接口
 输入参数  : None
 输出参数  : None
 返 回 值  : None

*****************************************************************************/
extern void BSP_PWRCTRL_DeepSleepForever(void );
#define DRV_PWRCTRL_DEEPSLEEP_FOREVER()  BSP_PWRCTRL_DeepSleepForever()

/*****************************************************************
Function: wifi_get_status
Description:
    get wifi current mode
Input:
    N/A
Output:
    N/A
Return:
    0: wifi is off
    1: wifi is in normal mode
    2: wifi is in tcmd mode
*******************************************************************/

extern int wifi_get_status(void);
#define WIFI_GET_STATUS() wifi_get_status()

/*****************************************************************
Function: wifi_get_rx_packet_report
Description:
    get result of rx ucast&mcast packets
Input:
    N/A
Output:
     ucast,mcast
Return:
    N/A
*******************************************************************/
extern void  wifi_get_rx_packet_report(unsigned int *ucastPkts, unsigned int *mcastPkts);
#define WIFI_GET_RX_PACKET_REPORT(ucastPkts,mcastPkts)  wifi_get_rx_packet_report(ucastPkts,mcastPkts)

/*****************************************************************
Function: wifi_power_off
Description:
    shutdown wifi
Input:
    N/A
Output:
    N/A
Return:
    0: execute ok
    1: execute failed
*******************************************************************/
int wifi_power_off(void);
#define WIFI_POWER_SHUTDOWN()  wifi_power_off()

/*************************************************
*  Function:  wifi_athtestcmd
*  Description: WIFI Control API
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          cmdStr:     控制WIFI的命令码流
*  Output:
*         N/A
*  Return:
*         函数执行的状态结果值；
*************************************************/
void wifi_athtestcmd(char * cmdStr);
#define WIFI_TEST_CMD(cmdStr) wifi_athtestcmd(cmdStr)

/***********************************************************************************
 Function:          wifi_get_pa_curMode
 Description:      get the currrent PA mode of the wifi chip
 Calls:
 Input:              NA
 Output:            NA
 Return:            0  (WIFI_ONLY_PA_MODE) WIFI chip is in PA mode
                        1 (WIFI_ONLY_NOPA_MODE) WIFI chip is in no PA mode
                      -1: wifi chip is in abnormal mode
 ************************************************************************************/

extern int wifi_get_pa_curMode(void);
#define WIFI_GET_PA_CUR_MODE()  wifi_get_pa_curMode()

/***********************************************************************************
 Function:          wifi_get_pa_mode
 Description:      returen the support PA mode of wifi chip
 Calls:
 Input:              NA
 Output:            NA
 Return:            0  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
                        1 (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
                        2 (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
                      -1: failed
 ************************************************************************************/

extern int wifi_get_pa_mode(void);
#define WIFI_GET_PA_MODE()  wifi_get_pa_mode()


/***********************************************************************************
 Function:          wifi_set_pa_mode
 Description:      set the PA mode of wifi chip
 Calls:
 Input:              0  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
                        1 (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
                        2 (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
 Output:            NA
 Return:
              0: success
             -1: failed
 ************************************************************************************/

extern int wifi_set_pa_mode(int wifiPaMode);
#define WIFI_SET_PA_MODE(wifiPaMode)  wifi_set_pa_mode(wifiPaMode)

/*****************************************************************
Function: wifi_power_on
Description:
    start wifi
Input:
    N/A
Output:
    N/A
Return:
    0: execute ok
    1: execute failed
*******************************************************************/
int wifi_power_on();
#define WIFI_POWER_START(type)  wifi_power_on()


extern BSP_S32  BSP_StartModeGet(void);
#define DRV_START_MODEGET() BSP_StartModeGet()

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_GetCpuLoad
 功能描述  : arm cpu占有率查询函数
 输入参数  : 无
 输出参数  : ulAcpuLoad:Acpu占有率地址.
             ulCcpuLoad:Ccpu占有率地址.
 返 回 值  : 0/1 。
*****************************************************************************/
extern unsigned int BSP_PWRCTRL_GetCpuLoad(unsigned int *ulAcpuLoad,unsigned int *ulCcpuLoad);
#define DRV_PWRCTRL_GetCpuLoad(ulAcpuLoad,ulCcpuLoad) BSP_PWRCTRL_GetCpuLoad(ulAcpuLoad,ulCcpuLoad)

/* Modified by z40661 for PC工程, 2012-02-25 end */

/*****************************************************************************
*  Function:  drv_get_local_usr_connect_status
*  Description: 返回当前是否有USB连接或者WIFI用户连接
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*  Output:
*  Return:
*         1 : 有
*         0 : 无
*************************************************/
extern unsigned long  drv_get_local_usr_connect_status(void);
#define DRV_AT_GET_USER_EXIST_FLAG() drv_get_local_usr_connect_status()



/******************************************************************************
  函数名:       BSP_S32 nand_get_bad_block (BSP_U32 *len, BSP_U32 **ppBadBlock)
  函数描述:     查询整个NAND的所有FLASH 坏块
  输入参数:     无
  输出参数:     pNum       : 返回坏块个数
                ppBadBlock ：数组指针，返回所有坏块的index索引
  返回值:       0    : 查询成功
                负数 : 查询失败
*******************************************************************************/
extern BSP_S32 nand_get_bad_block (BSP_U32 *pNum, BSP_U32 **ppBadBlock);
#define NAND_GET_BAD_BLOCK(pNum, ppBadBlock) nand_get_bad_block(pNum, ppBadBlock)

/******************************************************************************
*
  函数名:       BSP_VOID  nand_free_bad_block_mem(BSP_U32* pBadBlock)
  函数描述:     通过nand_get_bad_block接口申请的坏块内存由底软申请，由协议栈调用
                该接口释放。
  输入参数:     pBadBlock
  输出参数:     无
  返回值:       BSP_VOID
*******************************************************************************/
extern BSP_VOID  nand_free_bad_block_mem(BSP_U32* pBadBlock);
#define NAND_FREE_BAD_BLOCK_MEM(pBadBlock)  nand_free_bad_block_mem(pBadBlock)


#define NAND_MFU_NAME_MAX_LEN    16
#define NAND_DEV_SPEC_MAX_LEN    32

typedef struct
{
    BSP_U32           MufId;                                         /* 厂商ID */
    BSP_U8            aucMufName[NAND_MFU_NAME_MAX_LEN];             /* 厂商名称字符串 */
    BSP_U32           DevId;                                         /* 设备ID */
    BSP_U8            aucDevSpec[NAND_DEV_SPEC_MAX_LEN];             /* 设备规格字符串 */
}NAND_DEV_INFO_S;

/******************************************************************************
  函数名:       BSP_S32 nand_get_dev_info (NAND_DEV_INFO_S *pNandDevInfo)
  函数描述:     查询NAND设备相关信息：包括厂商ID、厂商名称、设备ID、设备规格
  输入参数:     无
  输出参数:     pNandDevInfo    存储NAND设备相关信息的结构体
  返回值:       0    : 查询成功
                负数 : 查询失败
*******************************************************************************/
extern BSP_S32 nand_get_dev_info (NAND_DEV_INFO_S *pNandDevInfo);
#define NAND_GET_DEV_INFO(pNandDevInfo) nand_get_dev_info(pNandDevInfo)

/********************************************************************************************************
 函 数 名  : USB_otg_switch_signal_set
 功能描述  : 用于BALONG和K3对接时USB通道切换GPIO引脚控制
 输入参数  : 组号、引脚号和值
 输出参数  : 无

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_signal_set(UINT8 ucGroup, UINT8 ucPin, UINT8 ucValue);
#define DRV_GPIO_SET(group, pin, value) USB_otg_switch_signal_set(group, pin, value)

#define DRV_GPIO_USB_SWITCH     200
#define DRV_GPIO_HIGH           1
#define DRV_GPIO_LOW            0

/********************************************************************************************************
 函 数 名  : USB_otg_switch_set
 功能描述  : 用于BALONG和K3对接时MODEM侧开启或关闭USB PHY
 输入参数  : 开启或关闭
 输出参数  : 无

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_set(UINT8 ucValue);
#define DRV_USB_PHY_SWITCH_SET(value) USB_otg_switch_set(value)

#define AT_USB_SWITCH_SET_VBUS_VALID            1
#define AT_USB_SWITCH_SET_VBUS_INVALID          2

#define USB_SWITCH_ON       1
#define USB_SWITCH_OFF      0

/********************************************************************************************************
 函 数 名  : USB_otg_switch_get
 功能描述  : 用于BALONG和K3对接时MODEM侧USB PHY状态查询
 输入参数  : 无
 输出参数  : 返回USB PHY开启状态

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_get(UINT8 *pucValue);
#define DRV_USB_PHY_SWITCH_GET(value) USB_otg_switch_get(value)

/*************************查询模块是否支持 START**********************/
/*合入DrvInterface.c的时候请删除查询模块，DrvInterface.c中已有支持*/
/* 当前版本是否支持某个模块 */
typedef enum tagBSP_MODULE_SUPPORT_E
{
    BSP_MODULE_SUPPORT     = 0,
    BSP_MODULE_UNSUPPORT   = 1,
    BSP_MODULE_SUPPORT_BUTTOM
}BSP_MODULE_SUPPORT_E;

/* 查询的模块类型 */

/* 查询的模块类型 */
typedef enum tagBSP_MODULE_TYPE_E
{
    BSP_MODULE_TYPE_SD = 0x0,
    BSP_MODULE_TYPE_CHARGE,
    BSP_MODULE_TYPE_WIFI,
    BSP_MODULE_TYPE_OLED,
    BSP_MODULE_TYPE_HIFI,
    BSP_MODULE_TYPE_POWER_ON_OFF,
    BSP_MODULE_TYPE_HSIC,
    BSP_MODULE_TYPE_HSIC_NCM,
    BSP_MODULE_TYPE_LOCALFLASH,
    BSP_MODULE_TYPE_BUTTOM
}BSP_MODULE_TYPE_E;

/*****************************************************************************
 函 数 名  : BSP_CheckModuleSupport
 功能描述  : 查询模块是否支持
 输入参数  : enModuleType: 需要查询的模块类型
 输出参数  : 无
 返回值    ：BSP_MODULE_SUPPORT或BSP_MODULE_UNSUPPORT
*****************************************************************************/
extern BSP_MODULE_SUPPORT_E BSP_CheckModuleSupport(BSP_MODULE_TYPE_E enModuleType);

#define DRV_USB_HSIC_SUPPORT_NCM()    BSP_CheckModuleSupport(BSP_MODULE_TYPE_HSIC_NCM)

#define DRV_GET_LOCAL_FLASH_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_LOCALFLASH)

#define DRV_GET_BATTERY_SUPPORT()   BSP_CheckModuleSupport(BSP_MODULE_TYPE_CHARGE)

#define DRV_GET_WIFI_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_WIFI)

#define DRV_GET_SD_SUPPORT()        BSP_CheckModuleSupport(BSP_MODULE_TYPE_SD)

#define DRV_GET_OLED_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_OLED)

#define DRV_GET_HIFI_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_HIFI)

#define DRV_GET_HSIC_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_HSIC)

/*************************查询模块是否支持 END************************/




/********************************内存分配 start*******************************/
/* DDR中各个内存段的类型 */
typedef enum tagBSP_DDR_SECT_TYPE_E
{
    BSP_DDR_SECT_TYPE_TTF = 0x0,
    BSP_DDR_SECT_TYPE_ARMDSP,
    BSP_DDR_SECT_TYPE_UPA,
    BSP_DDR_SECT_TYPE_CQI,
    BSP_DDR_SECT_TYPE_APT,
    BSP_DDR_SECT_TYPE_ET,
    BSP_DDR_SECT_TYPE_BBPMASTER,
    BSP_DDR_SECT_TYPE_NV,
    BSP_DDR_SECT_TYPE_DICC,
    BSP_DDR_SECT_TYPE_WAN,
    BSP_DDR_SECT_TYPE_SHARE_MEM,
    BSP_DDR_SECT_TYPE_EXCP,
    BSP_DDR_SECT_TYPE_HIFI,
    BSP_DDR_SECT_TYPE_BBPSAMPLE,
    BSP_DDR_SECT_TYPE_BUTTOM
}BSP_DDR_SECT_TYPE_E;


/* AXI中各个内存段的类型 */
typedef enum tagBSP_AXI_SECT_TYPE_E
{
    BSP_AXI_SECT_TYPE_ACORE_DEEPSLEEP = 0x0,
    BSP_AXI_SECT_TYPE_FLASH_SEM,
    BSP_AXI_SECT_TYPE_IFC,
    BSP_AXI_SECT_TYPE_ICC,
    BSP_AXI_SECT_TYPE_MEMMGR_FLAG,
    BSP_AXI_SECT_TYPE_DYNAMIC,
    BSP_AXI_SECT_TYPE_SOFT_FLAG,
    BSP_AXI_SECT_TYPE_IPF,
    BSP_AXI_SECT_TYPE_TEMPERATURE,
    BSP_AXI_SECT_TYPE_TEMP_PROTECT,
    BSP_AXI_SECT_TYPE_ONOFF,
    BSP_AXI_SECT_TYPE_DICC,
    BSP_AXI_SECT_TYPE_HIFI,
    BSP_AXI_SECT_TYPE_PTABLE,
    BSP_AXI_SECT_TYPE_RESERVE,
    BSP_AXI_SECT_TYPE_DLOAD_AUTOINSTALL,
    BSP_AXI_SECT_TYPE_DLOAD,
    BSP_AXI_SECT_TYPE_BUTTOM
}BSP_AXI_SECT_TYPE_E;


/* 内存段属性 */
typedef enum tagBSP_DDR_SECT_ATTR_E
{
    BSP_DDR_SECT_ATTR_CACHEABLE = 0x0,
    BSP_DDR_SECT_ATTR_NONCACHEABLE,
    BSP_DDR_SECT_ATTR_BUTTOM
}BSP_DDR_SECT_ATTR_E;


/* 定义虚实地址是否相同的枚举 */
typedef enum tagBSP_DDR_SECT_PVADDR_E
{
    BSP_DDR_SECT_PVADDR_EQU = 0x0,
    BSP_DDR_SECT_PVADDR_NOT_EQU,
    BSP_DDR_SECT_PVADDR_BUTTOM
}BSP_DDR_SECT_PVADDR_E;


/* DDR内存段的查询结构 */
typedef struct tagBSP_DDR_SECT_QUERY
{
    BSP_DDR_SECT_TYPE_E     enSectType;
    BSP_DDR_SECT_ATTR_E     enSectAttr;
    BSP_DDR_SECT_PVADDR_E   enPVAddr;
    BSP_U32                 ulSectSize;
}BSP_DDR_SECT_QUERY;

/* DDR内存段的详细信息 */
typedef struct tagBSP_DDR_SECT_INFO
{
    BSP_DDR_SECT_TYPE_E    enSectType;
    BSP_DDR_SECT_ATTR_E    enSectAttr;
    BSP_U32                ulSectVirtAddr;
    BSP_U32                ulSectPhysAddr;
    BSP_U32                ulSectSize;
}BSP_DDR_SECT_INFO;


/* AXI内存段的详细信息 */
typedef struct tagBSP_AXI_SECT_INFO
{
    BSP_AXI_SECT_TYPE_E    enSectType;
    BSP_U32                ulSectVirtAddr;
    BSP_U32                ulSectPhysAddr;
    BSP_U32                ulSectSize;
}BSP_AXI_SECT_INFO;

/*************************IP基地址、中断号查询 START******************/

/* 定义所有需要查询的IP类型 */
typedef enum tagBSP_IP_TYPE_E
{
    BSP_IP_TYPE_SOCP = 0x0,
    BSP_IP_TYPE_CICOM0,
    BSP_IP_TYPE_CICOM1,
    BSP_IP_TYPE_HDLC,
    BSP_IP_TYPE_BBPMASTER,
    BSP_IP_TYPE_ZSP_ITCM,
    BSP_IP_TYPE_ZSP_DTCM,
    BSP_IP_TYPE_AHB,
    BSP_IP_TYPE_WBBP,
    BSP_IP_TYPE_WBBP_DRX,
    BSP_IP_TYPE_GBBP,
    BSP_IP_TYPE_GBBP_DRX,
    BSP_IP_TYPE_GBBP1,
    BSP_IP_TYPE_GBBP1_DRX,
    BSP_IP_TYPE_ZSPDMA,
    BSP_IP_TYPE_SYSCTRL,
    BSP_IP_TYPE_CTU,

    BSP_IP_TYPE_BUTTOM
}BSP_IP_TYPE_E;

/*****************************************************************************
 函 数 名  : BSP_GetIPBaseAddr
 功能描述  : IP基地址查询
 输入参数  : enIPType: 需要查询的IP类型
 输出参数  : 无
 返回值    ：查询到的IP基地址
*****************************************************************************/
BSP_U32 BSP_GetIPBaseAddr(BSP_IP_TYPE_E enIPType);
#define DRV_GET_IP_BASE_ADDR(enIPType)  BSP_GetIPBaseAddr(enIPType)

/* 需要查询的中断类型 */
typedef enum tagBSP_INT_TYPE_E
{
    BSP_INT_TYPE_RTC = 0,
    BSP_INT_TYPE_WDT,
    BSP_INT_TYPE_USBOTG,
    BSP_INT_TYPE_UICC,
    BSP_INT_TYPE_IPF_C,
    BSP_INT_TYPE_IPF_A,
    BSP_INT_TYPE_SOCP_A,
    BSP_INT_TYPE_SOCP_C,
    BSP_INT_TYPE_CICOM_UL,
    BSP_INT_TYPE_CICOM_DL,
    BSP_INT_TYPE_HDLC_DEF,
    BSP_INT_TYPE_HDLC_FRM,
    #if 1 //monan:not used yet
    BSP_INT_TYPE_BBPMASTER,
    BSP_INT_TYPE_GBBP,          /*G_BBP_INT_LEVEL*/
    BSP_INT_TYPE_GBBP_AWAKE,    /*G_BBP_AWAKE_INT_LEVEL*/
    BSP_INT_TYPE_WBBP_0MS,      /*W_BBP_0MS_INT_LEVEL*/
    BSP_INT_TYPE_WBBP_AWAKE,    /*W_BBP_AWAKE_INT_LEVEL*/
    BSP_INT_TYPE_WBBP_SWITCH,   /*W_BBP_CLOCK_SWITCH_INT_LEVEL*/
    BSP_INT_TYPE_INT_OSRTC,     /*RTC_DUAL_TIMER_INT_LEVEL*/
    BSP_INT_TYPE_INT_SIMI,      /*SOC_SMIM_INT_LEVEL*/
    BSP_INT_TYPE_INT_ZSP_DOG,   /*ZSP_WDG_INT_LEVEL*/
    BSP_INT_TYPE_INT_HIFI_DOG,  /*HIFI_WDG_INT_LEVEL*/
    #endif

    BSP_INT_TYPE_ODTOA_INT12,     	/*目标定位中断*/
    BSP_INT_TYPE_CTU_INT_W,       	/*CTU产生与W模相关的中断(W可以为主模，也可以为从模)*/
    BSP_INT_TYPE_CTU_INT_G,       	/*CTU产生与G模相关的中断(G可以为主模，也可以为从模)*/
    BSP_INT_TYPE_CTU_INT_LTE,     	/*CTU产生与LTE模相关的中断(LTE可以为主模，也可以为从模)*/
    BSP_INT_TYPE_CTU_INT_TDS,     	/*CTU产生与TDS模相关的中断(TDS可以为主模，也可以为从模)*/
    BSP_INT_TYPE_WBBP_BBPMST_INTR,  /*译码BBP Master上报中断*/
    BSP_INT_TYPE_WBBP_MULTI_SEARCH_INTR, /*多径搜索测量中断*/
    BSP_INT_TYPE_WBBP_CARD2_TIME_INTR,   /*card2定时中断*/
    BSP_INT_TYPE_WBBP_CARD1_TIME_INTR,   /*card1定时中断*/
    BSP_INT_TYPE_WBBP_DECODE_INTR,       /*译码数据上报中断*/
    BSP_INT_TYPE_WBBP_RAKE_INTR,         /*Rake解调上报中断*/
    BSP_INT_TYPE_WBBP_SEARCH_INTR,       /*小区搜索中断*/
    BSP_INT_TYPE_WBBP_CARD2_0MS_INTR,    /*Card2 0ms 中断*/
    BSP_INT_TYPE_WBBP_CLK_SW_INTR,       /*时钟切换中断*/
    BSP_INT_TYPE_WDRX_SLP_AWK_SW_INTR,   /*睡眠、唤醒中断*/
    BSP_INT_TYPE_WBBP_CARDL_0MS_INTR,    /*card1 0ms中断*/
    BSP_INT_TYPE_GBBP_GSML_DRX_INTRL,    /*ARM_32K时钟域中断（仅含非掉电区）*/
    BSP_INT_TYPE_GBBP_GSML_NODRX_INTRL,  /*ARM_104M时钟域中断（含掉电区与非掉电区）*/
    BSP_INT_TYPE_GBBP_GSML_RESERVED_INTR,/*PHY_32K时钟域中断(仅含非掉电区)*/
    BSP_INT_TYPE_GBBP_GSML_NODRX_INTR,   /*PHY_104M时钟域中断(含掉电区与非掉电区)*/
    BSP_INT_TYPE_BUTTOM
}BSP_INT_TYPE_E;

#if defined (VERSION_V3R2)
#define DRV_USIMMSCI_FUNC_REGISTER(omSciFuncPtr)    BSP_SCIFuncRegister(omSciFuncPtr)
#else
#define DRV_USIMMSCI_FUNC_REGISTER(omSciFuncPtr)    DRV_OK
#endif
/* Modify by f62575 for V7代码同步, 2012-04-07, Begin   */
extern unsigned int BSP_PWRCTRL_StandbyStateAcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime);
extern unsigned int BSP_PWRCTRL_StandbyStateCcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime);
#define DRV_PWRCTRL_STANDBYSTATEACPU(ulStandbyTime, ulSwitchTime)	BSP_PWRCTRL_StandbyStateAcpu(ulStandbyTime, ulSwitchTime)
#define DRV_PWRCTRL_STANDBYSTATECCPU(ulStandbyTime, ulSwitchTime)	BSP_PWRCTRL_StandbyStateCcpu(ulStandbyTime, ulSwitchTime)
/*****************************************************************************
* 函 数 名  : BSP_GetProductDloadName
*
* 功能描述  : 获取产品下载版本名称，一键升级据此判断是否允许升级
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 产品下载版本名称字符串指针
*
* 修改记录  : 2011-3-30 wuzechun creat
*
*****************************************************************************/
extern BSP_S32 BSP_GetProductDloadName(char * pProductDloadName, unsigned int ulLength);

/*****************************************************************************
* 函 数 名  : BSP_GetProductInnerName
*
* 功能描述  : 获取内部产品名称
*
* 输入参数  : char* pProductIdInter,字符串指针
*                             BSP_U32 ulLength,缓冲区长度
* 输出参数  : 无
*
 返 回 值  : 0：正确，非0: 失败
*
* 修改记录  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
extern BSP_S32 BSP_GetProductInnerName (char * pProductIdInter, unsigned int ulLength);

/*****************************************************************************
* 函 数 名  : BSP_GetProductName
*
* 功能描述  : 获取产品名称
*
* 输入参数  : char* pProductName,字符串指针，保证不小于32字节
*                             BSP_U32 ulLength,缓冲区长度
* 输出参数  : 无
*
 返 回 值  : 0：正确，非0: 失败
*
* 修改记录  : 2011-3-30 wuzechun creat
*
*****************************************************************************/
extern BSP_S32 BSP_GetProductName (char * pProductName, unsigned int ulLength);

/*****************************************************************************
* 函 数 名  : BSP_GetSupportBands
*
* 功能描述  : 获取支持的频段
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
extern BSP_U32 BSP_GetSupportBands( BSP_VOID );

/*****************************************************************************
* 函 数 名  : BSP_HwGetHwVersion
*
* 功能描述  : 获取硬件版本名称
*
* 输入参数  : BSP_CHAR* pHwVersion,字符串指针，保证不小于32字节
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
extern BSP_S32 BSP_HwGetHwVersion (char* pFullHwVersion, BSP_U32 ulLength);

/*****************************************************************************
函数名：   BSP_USB_PortTypeValidCheck
功能描述:  提供给上层查询设备端口形态配置合法性接口
           1、端口为已支持类型，2、包含PCUI口，3、无重复端口，4、端点数不超过16，
           5、第一个设备不为MASS类
输入参数： pucPortType  端口形态配置
           ulPortNum    端口形态个数
返回值：   0:    端口形态合法
           其他：端口形态非法
*****************************************************************************/
unsigned int BSP_USB_PortTypeValidCheck(unsigned char *pucPortType, unsigned long ulPortNum);
#define DRV_USB_PORT_TYPE_VALID_CHECK(pucPortType, ulPortNum)  \
                    BSP_USB_PortTypeValidCheck(pucPortType, ulPortNum)

/*****************************************************************************
函数名：   BSP_USB_GetAvailabePortType
功能描述:  提供给上层查询当前设备支持端口形态列表接口
输入参数： ulPortMax    协议栈支持最大端口形态个数
输出参数:  pucPortType  支持的端口形态列表
           pulPortNum   支持的端口形态个数
返回值：   0:    获取端口形态列表成功
           其他：获取端口形态列表失败
*****************************************************************************/
unsigned int BSP_USB_GetAvailabePortType(unsigned char *pucPortType,
                            unsigned long *pulPortNum, unsigned long ulPortMax);
#define DRV_USB_GET_AVAILABLE_PORT_TYPE(pucPortType, pulPortNum, ulPortMax)  \
                BSP_USB_GetAvailabePortType(pucPortType, pulPortNum, ulPortMax)
/*****************************************************************************
 函 数 名  : DRV_CHG_STATE_GET
 功能描述  :获取补电标志
 返 回 值      : 1:需要补电
			      0:不需要补
 输出参数  : 无
 返回值：    无
*****************************************************************************/
extern unsigned int  DRV_CHG_STATE_GET(void);

/*****************************************************************************
 函 数 名  : DRV_CHG_BATT_SPLY
 功能描述  :开始补电状态
 输入参数  :
 输出参数  :无
 返回值：    无
*****************************************************************************/
extern int DRV_CHG_BATT_SPLY(void);
/* Modify by f62575 for V7代码同步, 2012-04-07, End   */

/*****************************************************************************
 函 数 名  : BSP_AXI_GetSectInfo
 功能描述  : AXI内存段查询接口
 输入参数  : enSectType: 需要查询的内存段类型
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_AXI_GetSectInfo(BSP_AXI_SECT_TYPE_E enSectType, BSP_AXI_SECT_INFO *pstSectInfo);
#define DRV_GET_FIX_AXI_ADDR(enSectType, pstSectInfo) BSP_AXI_GetSectInfo(enSectType, pstSectInfo)

/*for create_crypto_key,hash algorithm enum*/
typedef enum
{
    CREATE_CRYPTO_KEY_ALGORITHM_MD5 = 0x0,
    CREATE_CRYPTO_KEY_ALGORITHM_SHA1,
    CREATE_CRYPTO_KEY_ALGORITHM_MAX,
}CREATE_CRYPTO_KEY_ALGORITHM;

/*for crypto_hash,hash algorithm enum*/
typedef enum
{
	CRYPTO_ALGORITHM_MD5 = 0x0,
	CRYPTO_ALGORITHM_SHA1,
	CRYPTO_ALGORITHM_SHA256,
	CRYPTO_ALGORITHM_MAX
}CRYPTO_HASH_ALGORITHM;

/*for crypto_encrypt,aes algorithm enum*/
typedef enum
{
    CRYPTO_ENCRYPT_ALGORITHM_AES_ECB = 0x0,
    CRYPTO_ENCRYPT_ALGORITHM_MAX,
}CRYPTO_ENCRYPT_ALGORITHM;

typedef enum tagPWC_COMM_MODEM_E
{
	PWC_COMM_MODEM_0 = 0,
	PWC_COMM_MODEM_1 = 1,
	PWC_COMM_MODEM_BUTT
}PWC_COMM_MODEM_E;

/*****************************************************************************
* 函 数 名  : create_crypto_key
*
* 功能描述  : 使用输入的数据和HUK，生成密钥。
*当前支持MD5、和SHA-1算法。生成密钥的方法：把HUK和输入
*的数据连接起来作为MD5或SHA-1算法的输入，计算其HASH值
*
* 输入参数  : data：输入参数。存放用于生成密钥的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           algorithm：输入参数。用于产生密钥的算法。
*                           key：输出参数。存放生成的密钥。
*            注意：其长度必须不小于16字节。因为密钥为16字节。
*                           klen：输入输出参数。作为输入参数，存放key的
*            缓冲区的长度。作为输出参数，存放生成的密钥的长度。(字节)
* 输出参数  :
*
* 返 回 值  :  BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  :内存由调用者申请
*
*****************************************************************************/
int create_crypto_key(char *data, int len, CREATE_CRYPTO_KEY_ALGORITHM algorithm, char *key, int *klen);
#define CREATE_CRYPTO_KEY(data,len,algorithm,key,klen)  create_crypto_key(data,len,algorithm,key,klen)

/*****************************************************************************
* 函 数 名  : crypto_hash
*
* 功能描述  : 计算输入的数据的HASH值。
*                           当前支持MD5、和SHA-1算法。输出HASH值长度是16字节
*
* 输入参数  : data：输入参数。存放用于需要计算HASH值的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           algorithm：输入参数。HASH算法。
*                           hash：输出参数。存放生成的HASH值。
*                           hlen：输入输出参数。作为输入参数，存放HASH值的缓冲区的长度。(字节)
*                           作为输出参数，存放生成的HASH值的长度。
* 输出参数  :
*
* 返 回 值  : BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_hash(char *data, int len, CRYPTO_HASH_ALGORITHM algorithm, char *hash, int *hlen);
#define CRYPTO_HASH(data,len,algorithm,hash,hlen)  crypto_hash(data,len,algorithm,hash,hlen)

/*****************************************************************************
* 函 数 名  : crypto_encrypt
*
* 功能描述  : 使用指定的密钥和指定的算法对输入的数据加密，输出加密后的数据。
                             当前支持AES-ECB算法。
*
* 输入参数  : data：输入参数。存放需要加密的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           algorithm：输入参数。HASH算法。
*                           key：输入参数。存放加密密钥。
*                           klen：输入参数。key的长度。(字节)
*                           cipher_data：输出参数。存放加密后的数据。
*                           cipher_len：输入输出参数。作为输入参数，存放密文的缓冲区的长度。(字节)
*               作为输出参数，存放生成的密文的长度。
* 输出参数  :
*
* 返 回 值  :  BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_encrypt (char *data, int len, CRYPTO_ENCRYPT_ALGORITHM algorithm, char *key, int klen, char *cipher_data, int *cipher_len);
#define CRYPTO_ENCRYPT(data,len,algorithm,key,klen,cipher_data,cipher_len)  \
crypto_encrypt(data,len,algorithm,key,klen,cipher_data,cipher_len)

/*****************************************************************************
* 函 数 名  : crypto_rsa_encrypt
*
* 功能描述  : 使用保存在NV中的改制用RSA公钥（读取时需要同样进行签名验证）
*           对输入的数据加密，输出加密后的数据。
* 输入参数  : data：输入参数。存放需要加密的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           rsa_key:RSA公钥
*                           rsa_len:RSA公钥长度(字节)
*                           cipher_data：输出参数。存放加密后的数据。
*                           cipher_len：输入输出参数。作为输入参数，存放密文的缓冲区的长度。(字节)
*               作为输出参数，存放生成的密文的长度。
* 输出参数  :
*
* 返 回 值  :  BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_rsa_encrypt (char *data, int len, char *rsa_key, int rsa_klen, char *cipher_data, int *cipher_len);
#define CRYPTO_RSA_ENCRYT(data,len,rsa_key,rsa_klen,cipher_data,cihper_len) \
crypto_rsa_encrypt(data,len,rsa_key,rsa_klen,cipher_data,cihper_len)

/*****************************************************************************
* 函 数 名  : crypto_rsa_decrypt
*
* 功能描述  : 使用保存在NV中的改制用RSA公钥，对输入的数据解密
*               输出解密后的数据。
* 输入参数  : cipher_data：输入参数。存放加密数据。
*                           cipher_len：输入参数。存放密文的缓冲区的长度。(字节)
*                           rsa_key:RSA公钥
*                           rsa_len:RSA公钥长度(字节)
*                           data：输出参数。存放需解密后的数据。
*                           len：输入输出参数。作为输入参数，存放解密后的缓冲区的长度(字节)
*               作为输出参数，存放生成的明文的长度
* 输出参数  :
*
* 返 回 值  :  BSP_OK--解密成功;BSP_ERROR--解密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_rsa_decrypt (char *cipher_data, int cipher_len, char *rsa_key, int rsa_klen, char *data, int *len);
#define CRYPTO_RSA_DECRYPT(cipher_data,cihper_len,rsa_key,rsa_klen,data,len) \
crypto_rsa_decrypt(cipher_data,cihper_len,rsa_key,rsa_klen,data,len)

/*****************************************************************************
* 函 数 名  : crypto_rand
*
* 功能描述  : 随机数生成接口
* 输入参数  : rand_data:随机数存放buffer
*                           len:期望得到的随机数字节数
*
* 输出参数  :
*
* 返 回 值  :  BSP_OK--获取随机数成功;BSP_ERROR--获取失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_rand (char *rand_data, int len);
#define CRYPTO_RAND(rand_data,len)  crypto_rand(rand_data,len)


int efuseWriteHUK(char *pBuf,unsigned int len);
#define EFUSE_WRITE_HUK(pBuf,len) efuseWriteHUK(pBuf,len)


int CheckHukIsValid(void);
#define DRV_CHECK_HUK_IS_VALID() CheckHukIsValid()

extern void BSP_PWC_SetTimer4WakeSrc(void);
extern void BSP_PWC_DelTimer4WakeSrc(void);

#define EXCH_TASK_NAME_PART_LEN     12
#define EXCH_CB_NAME_SIZE           32

typedef struct
{
    char   aucName[EXCH_CB_NAME_SIZE];
    unsigned char * pucData;
    unsigned int ulDataLen;
}cb_buf_t;

typedef void (*exchCBReg)(void *);

extern int  DRV_EXCH_TASK_INFO_ADD(int taskPid);
extern int  DRV_EXCH_CUST_FUNC_REG(exchCBReg cb);

/*  SIM  state*/
typedef enum tagSCI_CARD_STATE_E
{
    SCI_CARD_STATE_READY = 0,           	/* Ready */
    SCI_CARD_STATE_NOCARD,                	/* No card */
    SCI_CARD_STATE_BUSY,               		/* In initialization*/
	SCI_CARD_STATE_BUTT,               		/* Butt*/
} SCI_CARD_STATE_E;
extern int BSP_Modem_OS_Status_Switch(int enable);
#define DRV_OS_STATUS_SWITCH(enable) BSP_Modem_OS_Status_Switch(enable)

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetNVBackupFlag
*
* 功能描述  : 获取升级前是否进行NV备份标志
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP_TRUE  :备份NV
*            BSP_FALSE :不备份NV
*
* 其它说明  : 此接口只对非一键式升级方式（SD升级/在线升级）有效，一键式升级会发AT命令设置
*
*****************************************************************************/
BSP_BOOL BSP_DLOAD_GetNVBackupFlag(BSP_VOID);
#define DRV_DLOAD_GETNVBACKUPFLAG() BSP_DLOAD_GetNVBackupFlag()

extern BSP_VOID BSP_RunDsp(BSP_VOID);
extern BSP_S32 BSP_LoadDsp(BSP_VOID);

typedef unsigned int (*PWRCTRLFUNCPTR)(unsigned int arg);     /* ptr to function returning int */

#define DRV_NOT_START_UMTS (0x55aa5a5a)
extern unsigned long DRV_BBP_GPIO_GET(void);

BSP_S32 BSP_DDR_GetSectInfo(BSP_DDR_SECT_QUERY *pstSectQuery, BSP_DDR_SECT_INFO *pstSectInfo);
#define DRV_GET_FIX_DDR_ADDR(pstSectQuery, pstSectInfo) BSP_DDR_GetSectInfo(pstSectQuery, pstSectInfo)

extern BSP_S32 BSP_PMU_AptEnable(BSP_VOID);
#define DRV_PMU_APT_ENABLE()    BSP_PMU_AptEnable()

extern BSP_S32 BSP_PMU_AptDisable(BSP_VOID);
#define DRV_PMU_APT_DISABLE()    BSP_PMU_AptDisable()

BSP_S32 BSP_GetIntNO(BSP_INT_TYPE_E enIntType);
#define DRV_GET_INT_NO(enIntType)    BSP_GetIntNO(enIntType)

/*用于区分在Modem/HIFI复位前还是复位后调用回调函数*/
typedef enum
{
    DRV_RESET_CALLCBFUN_RESET_BEFORE,   /*Modem/HIFI 复位前*/
    DRV_RESET_CALLCBFUN_RESET_AFTER,    /*Modem/HIFI 复位后*/
    DRV_RESET_CALLCBFUN_MOEMENT_INVALID
} DRV_RESET_CALLCBFUN_MOMENT;

/*****************************************************************************
 函 数 名  : pdrv_reset_cbfun
 功能描述  : 组件需要注册的回调函数定义.
 输入参数  : DRV_RESET_CALLCBFUN_MOMENT eparam,表示复位前还是复位后,
                int userdata,用户数据
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             非0：操作失败。
*****************************************************************************/
typedef int (*pdrv_reset_cbfun)(DRV_RESET_CALLCBFUN_MOMENT eparam, int userdata);

/*****************************************************************************
 函 数 名  : ccorereset_regcbfunc
 功能描述  : 提供上层应用程序注册Modem单独复位前/后的回调接口函数。
 输入参数  : pname：上层组件注册的名字，注意不包括结束符最长9个字符，底软负责存储。
             pcbfun：回调函数指针。
             userdata:上层组件数据，在调用回调函数时，作为入参传给用户。
             priolevel: 回调函数调用优先级，0-49
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int ccorereset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel);
#define DRV_CCORERESET_REGCBFUNC(pname,pcbfun, userdata, priolevel)\
                ccorereset_regcbfunc(pname,pcbfun, userdata, priolevel)

/*****************************************************************************
 函 数 名  : hifireset_regcbfunc
 功能描述  : 提供上层应用程序注册HIFI单独复位的回调接口函数。
 输入参数  : pname：上层组件注册的名字，注意不包括结束符最长9个字符，底软负责存储。
             pcbfun：回调函数指针。
             puserdata:上层组件数据，在调用回调函数时，作为入参传给用户。
             priolevel: 回调函数调用优先级，0-49
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int hifireset_regcbfunc(const char *pname, pdrv_reset_cbfun pcbfun, int userdata, int priolevel);
#define DRV_HIFIRESET_REGCBFUNC(pname,pcbfun, userdata, priolevel)\
                hifireset_regcbfunc(pname,pcbfun, userdata, priolevel)

typedef enum tagHKADC_CONV_MODE_E
{
    HKADC_CONV_DELAY = 0,
	HKADC_CONV_NO_DELAY,
	HKADC_CONV_BUTT

}HKADC_CONV_MODE_E;

/*****************************************************************************
 函 数 名  : DRV_HKADC_GET_TEMP
 功能描述  : 获取设备的温度值和电压值
 输入参数  : enMode, *pusVolt, *psTemp
 输出参数  : *pusVolt, *psTemp
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int DRV_HKADC_GET_TEMP(HKADC_TEMP_PROTECT_E enMode, unsigned short* pusVolt, short* psTemp, HKADC_CONV_MODE_E enConvMode);

extern unsigned int BSP_GetSliceValue(BSP_VOID);
#define DRV_GET_SLICE()   BSP_GetSliceValue()

extern BSP_VOID DRV_IPC_SEMGIVE(BSP_U32 u32SignalNum);
extern BSP_S32 DRV_IPC_SEMTAKE(BSP_U32 u32SignalNum, BSP_S32 s32timeout);

#define NV_NAME_LENGTH          8       /*NV name maximum length*/
#define NVE_NV_DATA_SIZE        104     /*NV data maximum length*/

#define NV_READ                 1       /*NV read  operation*/
#define NV_WRITE                0       /*NV write operation*/

typedef struct nve_info_stru {
	BSP_U32 nv_operation;              /*0-write,1-read*/
	BSP_U32 nv_number;                 /*NV number you want to visit*/
	BSP_S8 nv_name[NV_NAME_LENGTH];
	BSP_U32 valid_size;
	BSP_U8 nv_data[NVE_NV_DATA_SIZE];
}NVE_INFO_S;

/*****************************************************************************
* 函 数 名  : BSP_NVE_DirectAccess
*
* 功能描述  : 访问LINUX NVE（喂狗）
*
* 输入参数  : nve  ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2013年6月27日  v1.00  yuanqinshun  创建
*****************************************************************************/
BSP_S32 BSP_NVE_DirectAccess(NVE_INFO_S *nve);
#define DRV_NVE_ACCESS(nve)    BSP_NVE_DirectAccess(nve)

/*****************************************************************************
 函 数 名  : DRV_PASTAR_EXC_CHECK
 功能描述  : 通信模块检查PASTAR是否有异常接口
 输入参数  : modem_id       卡号
 输出参数  : 无
 返 回 值  : BSP_OK          没有异常
             BSP_ERROR       存在异常
*****************************************************************************/
extern BSP_S32 DRV_PASTAR_EXC_CHECK(PWC_COMM_MODEM_E modem_id);


extern BSP_S32 PWRCTRL_DfsQosUpdate(BSP_S32 qos_id, BSP_S32 req_id, BSP_U32 req_value);


extern BSP_S32 PWRCTRL_DfsQosRequest(BSP_S32 qos_id, BSP_U32 req_value, BSP_S32* req_id);


/* UART 端口切换 */
#define UART_A_SHELL  (0x5A5A5A5A)
#define UART_C_SHELL  (0xA5A5A5A5)

/*****************************************************************************
 函 数 名  : DRV_OUTER_RFSWITCH_SET
 功能描述  : NAS专用接口
             外接Modem的天线选择控制接口
             当前单板不支持此功能（对应NV项为全0），或者其他异常 返回-1
 输入参数  : status 1:外接Modem天线功能，0：非外接Modem天线功能
 输出参数  : 无
 返 回 值  : 0：成功，-1：失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年12月30日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
extern int outer_rfswitch_set(BSP_U32 status);
#define  DRV_OUTER_RFSWITCH_SET(status) outer_rfswitch_set(status)

/*****************************************************************************
 函 数 名  : DRV_OUTER_RFSWITCH_GET
 功能描述  : NAS专用接口
             外接Modem的天线选择查询接口
             当前单板不支持此功能（对应NV项为全0），或者其他异常 返回-1
 输入参数  : 无
 输出参数  : status 1:外接Modem天线功能，0：非外接Modem天线功能
 返 回 值  : 0：成功，-1：失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年12月30日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
extern int outer_rfswitch_get(BSP_U32 *status);
#define  DRV_OUTER_RFSWITCH_GET(status) outer_rfswitch_get(status)


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of __DRV_INTERFACE_H__ */


