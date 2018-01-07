
#ifdef __cplusplus
extern "C"
{
#endif

#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <asm/io.h>
#include "bsp_om.h"
#include "BSP.h"
#include <linux/slab.h>

#include "soc_baseaddr_interface.h"
#include "soc_peri_sctrl_interface.h"
#include <soc_ao_sctrl_interface.h>
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#if defined BSP_FE
#include "BSP_DRV_END.h"
#endif


#include "bsp_mailbox.h"

static IFC_GEN_CALL2(MAILBOX_IFC_ACPU_TO_CCPU_GET_EFUSE_CHIPID, get_efuse_chipid_value,
              IFC_BIVAR, unsigned char *, pu8Buf, 0,
              IFC_INCNT, unsigned int, uiLength, 0)
static IFC_GEN_CALL2(MAILBOX_IFC_ACPU_TO_CCPU_GET_EFUSE_DIEID, get_efuse_dieid_value,
	          IFC_BIVAR, unsigned char *, pu8Buf, 0,
	          IFC_INCNT, unsigned int, uiLength, 0)

#define logMsg printk

#define BSP_OM_CHECK_RET(ret) \
do{\
    if (OK != ret)\
    {\
        logMsg("OM Check Error, line:%d, return:%d\n", __LINE__, ret,0,0,0,0);\
        return ret;\
    }\
}while(0)


extern void systemError(int modId, int arg1, int arg2, char * arg3, int arg3Length);
extern void* exchMemMalloc(unsigned int ulSize);

/*2009-06-18 wangxuesong Add for BJ9D01071 end*/

#if 0
static char* g_OM_shellTaskName = "tShell0";

static UE_SW_BUILD_VER_INFO_STRU g_BootLoaderVersion =
{
    1,      // V 部分
    1,      // R 部分
    1,      // C 部分
    0,      // B 部分
    0,      // SPC 部分
    0,      // 自定义

    // 产品类型编号，即不同外设组合的硬件平台
    (BSP_U32)UE_SW_BUILD_VER_PRODUCT_UNKNOWN,
    __DATE__,
    __TIME__
};
#endif

static UE_SW_BUILD_VER_INFO_STRU g_BspBuildVersion =
{
    800,      // V 部分
    100,      // R 部分
    0,      // C 部分
    0,      // B 部分
    0,      // SPC 部分
    0,      // 自定义

    // 产品类型编号，即不同外设组合的硬件平台
    (BSP_U32)UE_SW_BUILD_VER_PRODUCT_UNKNOWN,
    __DATE__,
    __TIME__
};

/* 单板主类型命令*/
const BSP_CHAR BoardMainName[8][BOARD_MAINNAME_LEN] =
{
    "H69DUSA",
    "H69DUSB",
    "H69DUSC",
    "H69DUSD",
    "RSV",
    "RSV",
    "RSV",
    "RSV"
};

/* 单板子类型命令 */
const BSP_CHAR BoardSubName[4][BOARD_SUBNAME_LEN] =
{
    "VA",
    "VB",
    "VC",
    "VD"
};
/* LTE协议栈MAC地址 */
BSP_U8  g_mac_address_pstable[] =
{
    //0x00,0x1a,0x2b,0x3c,0x4d,0x5f
    0x4c,0x54,0x99,0x45,0xe5,0xd5
};
/*
只包括RAM检查
*/
BSP_S32 OM_RAMCheckVolid(BSP_U32* u32CheckedAddr)
{
    return OK;
}

BSP_S32 OM_GetVersionInfo(OM_VERSION_INFO_S *pstVersionInfo)
{
    if(!pstVersionInfo)
    {
        return ERROR;
    }

    return ERROR;
}

/*****************************************************************\
5.2.1 BSP_OM_RegRead
【描述】
  读寄存器值
【原型】
   INT32 BSP_OM_RegRead(UINT32 ulRegAddr, ENADDRTYPE enAddrType, UINT32* pulValue)

参数	含义	输入/输出
ulRegAddr	待查询的寄存器地址	输入参数
enAddrType	读取的寄存器类型(8、16、32位)	输入参数
pulValue	返回的寄存器值	输出参数

【返回值】
返回值	含义
OK	处理成功
ERROR	处理失败
\****************************************************************/
BSP_S32 BSP_OM_RegRead(BSP_U32* u32RegAddr,
    ENADDRTYPE enAddrType, BSP_U32 *pu32Value)
{

    BSP_S32 s32Ret;
    s32Ret = OK;
    if(OK != s32Ret)
    {
        return ERROR;
    }

    switch(enAddrType)
    {
        case ADDRTYPE8BIT:
            *pu32Value = *((volatile BSP_U8 *)u32RegAddr);
            return OK;

        case ADDRTYPE16BIT:
            *pu32Value = *((volatile BSP_U16 *)u32RegAddr);
            return OK;

        case ADDRTYPE32BIT:
            *pu32Value = *((volatile BSP_U32 *)u32RegAddr);
            return OK;

        default:
            return ERROR;
    }
}

/*****************************************************************\
5.2.1 BSP_OM_RegWrite
【描述】
  写寄存器值
【原型】
   INT32 BSP_OM_RegWrite(UINT32 u32RegAddr, ENADDRTYPE enAddrType, UINT32 u32Value)

参数	含义	输入/输出
u32RegAddr	待写入的寄存器地址	输入参数
enAddrType	写入的寄存器类型(8、16、32位)	输入参数
u32Value	返回的寄存器值	输出参数

【返回值】
返回值	含义
OK	处理成功
ERROR	处理失败
\****************************************************************/
BSP_S32 BSP_OM_RegWrite(BSP_U32* u32RegAddr,
    ENADDRTYPE enAddrType, BSP_U32 u32Value)
{
    BSP_S32 s32Ret;
    s32Ret = OK;
    if(OK != s32Ret)
    {
        return ERROR;
    }

    switch(enAddrType)
    {
        case ADDRTYPE8BIT:
            *((volatile BSP_U8 *)u32RegAddr) = (BSP_U8)u32Value;
            return OK;

        case ADDRTYPE16BIT:
            *((volatile BSP_U16 *)u32RegAddr) = (BSP_U16)u32Value;
            return OK;

        case ADDRTYPE32BIT:
            *((volatile BSP_U32 *)u32RegAddr) = u32Value;
            return OK;

        default:
            return ERROR;
    }
}



/*****************************************************************\
5.2.8 BSP_OM_SoftReboot
【描述】
 单板复位
【原型】
   void BSP_OM_SoftReboot()
【返回值】
无

\****************************************************************/
BSP_VOID BSP_OM_SoftReboot(BSP_VOID)
{
    //BSP_WDT_reboot();
    return;
}

/*****************************************************************\
5.2.9 BSP_OM_Shutdown
【描述】
 单板关闭
【原型】
   void BSP_OM_ShutDown()
【返回值】
无

\****************************************************************/

BSP_VOID BSP_OM_ShutDown(BSP_VOID)
{
}

/*****************************************************************\
5.2.10 BSP_OM_SetBootFlag
【描述】
 设置BootLoader启动标志
【原型】
   INT32 BSP_OM_SetBootFlag(ENBOOTFLAG enBootFlag)
参数	含义	输入/输出
ulFlag	BOOT_FAST 快速启动
BOOT_SLOW 慢速启动	输入参数

【返回值】
返回值	含义
OK	处理成功
ERROR	处理失败

\****************************************************************/
/*BSP_S32 BSP_OM_SetBootFlag(ENBOOTFLAG enBootFlag)
{
    if((enBootFlag == BOOT_FAST) || (enBootFlag == BOOT_SLOW))
    {
        g_enBootFlag = enBootFlag;
        return OK;
    }
    return ERROR;
}
*/
/*****************************************************************\
5.2.11 BSP_OM_ComVersionCtrl
【描述】
 组件版本读写接口
【原型】
INT32 BSP_OM_ComVersionCtrl(INT32* pulValue, UINT32 ulLen,
    UINT32 ulType,UINT32 ulMode)

参数	含义	输入/输出
pulValue	读模式时：为调用者待保存返回的组件版本信息的内存地址。
写模式时：为调用者准备写入的组件版本信息的地址。	输入/输出参数
ulLen	读模式时：为调用者待保存返回的组件版本信息的内存大小。
写模式时：为调用者准备写入的组件版本信息的字符数，不包括"\0"时最大30个字符。	输入参数
ulType	版本信息ID
ulMode	0：读取指定ID的组件版本信息。
1：写入指定ID的组件版本信息。	输入参数

【返回值】
返回值	含义
OK	处理成功
ERROR	处理失败

\****************************************************************/
/*BSP_S32 BSP_OM_ComVersionCtrl(BSP_S32 *ps32Value,
    BSP_U32 u32Len, BSP_U32 u32Type, BSP_U32 u32Mode)
{
    return ERROR;
}*/

/*****************************************************************\
5.2.12 BSP_OM_ComVersionQuery
【描述】
查询所有组件的版本号。
【原型】
INT32 BSP_OM_ComVersionQuer(void ** ppVersionInfo,UINT32 * ulLen)
参数	含义	输入/输出
ppVersionInfo	待保存的版本信息地址	输出参数
ulLen	待返回数据的字节数。	输出参数
【返回值】
返回值	含义
OK	处理成功
ERROR	处理失败

\****************************************************************/
/*BSP_S32 BSP_OM_ComVersionQuery(BSP_VOID **ppVersionInfo,
    BSP_U32 u32Len)
{
    return ERROR;
}*/

/*****************************************************************\
5.2.13 BSP_OM_GetPaPower
【描述】
读取PA发射功率。
【原型】
int BSP_OM_GetPaPower(INT32 *pulData, INT32 *pulTable)
参数	含义	输入/输出
pulData	存放发射功率的输出值。	输出参数
pulTable	电压功率转换表	输入参数

【返回值】
返回值	含义
OK	处理成功。
ERROR	处理失败。

\****************************************************************/
/*BSP_S32 BSP_OM_GetPaPower(BSP_S32 *pu32Data, BSP_U32 *ps32Table)
{
    return ERROR;
}
*/
/*****************************************************************\
5.2.14 BSP_OM_GetPaTemperature
【描述】
读取PA当前温度。
【原型】
int BSP_OM_GetPaTemperature (INT32 *pulData, INT32 *pulTable)
参数	含义	输入/输出
pulData	存放发射功率的输出值。	输出参数
pulTable	电压温度转换表	输入参数

【返回值】
返回值	含义
OK	处理成功。
ERROR	处理失败。

\****************************************************************/
/*BSP_S32 BSP_OM_GetPaTemperature(BSP_S32 *ps32Data, BSP_S32 *ps32Table)
{
    return ERROR;
}*/

/*2009-09-17 wangxuesong added for BJ9D01877 begin*/
/*
*函数名: BSP_OM_GetFPGAVer
*
*输入:  无
*
*输出:  无
*/
BSP_S32 BSP_OM_GetFPGAVer(BSP_VOID)
{
    return OK;
}
/*2009-09-17 wangxuesong added for BJ9D01877 end*/

BSP_S32 BSP_OM_ShellLock(BSP_BOOL bRequest)
{
    return BSP_OK;
}

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
BSP_S32 BSP_OM_GetFlashSpec(BSP_U32 *pu32FlashTotalSize, BSP_U32 *pu32FlashBlkSize, BSP_U32 *pu32FlashPageSize)
{
    if(!pu32FlashTotalSize)
    {
        logMsg("null pointer(pu32FlashTotalSize)\n");
        return BSP_ERROR;
    }

    if(!pu32FlashBlkSize)
    {
        logMsg("null pointer(pu32FlashBlkSize)\n");
        return BSP_ERROR;
    }

    if(!pu32FlashPageSize)
    {
        logMsg("null pointer(pu32FlashPageSize)\n");
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_OM_NET
*
* 功能描述  : 网口可维可测接口
*
* 输入参数  : BSP_OM_NET_S *pstNetOm
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_VOID BSP_OM_NET(BSP_OM_NET_S *pstNetOm)
{

}/*lint !e715*/

/*****************************************************************************
* 函 数 名  : BSP_InitBootVersion
*
* 功能描述  : 初始化BOOT版本全局变量
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_VOID BSP_InitBootVersion(VOID)
{
    return;
}

/*****************************************************************************
* 函 数 名  : BSP_AtoX
*
* 功能描述  : 将字符串转换为整数(16进制)
*
* 输入参数  : pStr:待转换字符串
*
* 输出参数  : 转换后的整数
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_U32 BSP_AtoX(BSP_U8* pStr)
{
	BSP_U8* p = pStr;
    BSP_U32 n;

	n = 0;
	for(;;p++)
    {
		switch(*p)
        {
		case ' ':
		case '\t':
		case '0':
        case 'x':
        case 'X':
			p++;
            continue;
        default:
            break;
		}
		break;
	}

	while( (*p >= '0' && *p <= '9') ||
           (*p >= 'a' && *p <= 'f') ||
           (*p >= 'A' && *p <= 'F') )
    {
        if (*p >= '0' && *p <= '9')
        {
            n = n*16 + *p++ - '0';
        }
        else if (*p >= 'a' && *p <= 'f')
        {
            n = n*16 + (*p++ - 'a' + 10);
        }
        else /* A ~ F */
        {
            n = n*16 + (*p++ - 'A' + 10);
        }
    }
	return (n);
}


/*****************************************************************************
* 函 数 名  : BSP_InitBspVersion
*
* 功能描述  : 初始化BSP版本全局变量
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 BSP_InitBspVersion(VOID)
{
    return 0;
}

/*****************************************************************************
* 函 数 名  : BSP_GetBootBuildVersion
*
* 功能描述  : 获取Boot版本
*
* 输入参数  : 无
*
* 输出参数  : BOOT版本结构体指针
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
const UE_SW_BUILD_VER_INFO_STRU* BSP_GetBootBuildVersion(VOID)
{
    return NULL;
}

/*****************************************************************************
* 函 数 名  : BSP_GetBuildVersion
*
* 功能描述  : 获取BSP版本
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP版本结构体指针
*
* 其它说明  : 无
*
*****************************************************************************/
const UE_SW_BUILD_VER_INFO_STRU* BSP_GetBuildVersion(VOID)
{
    return &g_BspBuildVersion;
}

/*****************************************************************************
* 函 数 名  : BSP_GetBuildVersion
*
* 功能描述  : 获取BSP版本
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP版本结构体指针
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 BSP_OM_GetHwVersion(BSP_VOID)
{
    return OK;
}


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
BOARD_TYPE_E BSP_OM_GetBoardType(BSP_VOID)
{
    return BOARD_TYPE_GUL;
}


/*****************************************************************************
* 函 数 名  : BBPGetCurTime
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
BSP_U32 BBPGetCurTime(BSP_U64 *pCurTime)
{
    BSP_U64 u64TempTime[4];
    void __iomem* sctrl_addr = (void __iomem*)HISI_VA_ADDRESS(SOC_AO_SCTRL_BASE_ADDR);

    if (NULL == pCurTime)
    {
        printk("BSP_BBPGetCurTime Wrong Para. line:%d\n", __LINE__);
        return BSP_ERR_INVALID_PARA;
    }

    u64TempTime[0] = readl(SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_ADDR(sctrl_addr));
    u64TempTime[1] = readl(SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT1_ADDR(sctrl_addr));
    u64TempTime[2] = readl(SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT0_ADDR(sctrl_addr));
    u64TempTime[3] = readl(SOC_AO_SCTRL_SC_SYSTEST_SLICER_CNT1_ADDR(sctrl_addr));

    if (u64TempTime[2] < u64TempTime[0])
    {
        *pCurTime = ((u64TempTime[3] - 1) << 32) | u64TempTime[0];
    }
    else
    {
        *pCurTime = (u64TempTime[1] << 32) | u64TempTime[0];
    }

    return 0;
}
#if 0
/*****************************************************************************
 函 数 名  : DRV_GET_BBP_TIMER_VALUE
 功能描述  : 获取BBP定时器的值。用于OAM 时戳
 输入参数  : 无。
 输出参数  : pulHigh32bitValue:BBP定时器的值获取高32bit,本参数如为空，
                                                      则只返回低32bit的值
                            pulLow32bitValue:BBP定时器的值获取低 32bit,不能为空，否则会返回失败。
 返 回 值  : 0-成功
                       -1 -失败

*****************************************************************************/
 int DRV_GET_BBP_TIMER_VALUE(unsigned int  *pulHigh32bitValue,  unsigned int  *pulLow32bitValue)
 {
    BSP_U64 u64TempTime[4];
    unsigned long sctrl_addr = (unsigned long)HISI_VA_ADDRESS(SOC_SC_OFF_BASE_ADDR);

    if (NULL == pulLow32bitValue)
    {
        printk("DRV_GET_BBP_TIMER_VALUE Wrong Para. line:%d\n", __LINE__);
        return BSP_ERR_INVALID_PARA;
    }

    u64TempTime[0] = readl(SOC_SCTRL_SC_BBPABB_SUBSYS_STAT3_ADDR(sctrl_addr));
    u64TempTime[1] = readl(SOC_SCTRL_SC_BBPABB_SUBSYS_STAT4_ADDR(sctrl_addr));
    u64TempTime[2] = readl(SOC_SCTRL_SC_BBPABB_SUBSYS_STAT3_ADDR(sctrl_addr));
    u64TempTime[3] = readl(SOC_SCTRL_SC_BBPABB_SUBSYS_STAT4_ADDR(sctrl_addr));

    if (u64TempTime[2] < u64TempTime[0])
    {
        if (pulHigh32bitValue != NULL) {
            *pulHigh32bitValue = (u64TempTime[3] - 1);
        }

        *pulLow32bitValue = u64TempTime[0];
    }
    else
    {
        if (pulHigh32bitValue != NULL) {
            *pulHigh32bitValue = u64TempTime[1];
        }

        *pulLow32bitValue = u64TempTime[0];
    }

    return 0;
 }
#endif

void* BSP_MNTN_ExchMemMalloc(unsigned int ulSize)
{
    return exchMemMalloc(ulSize);
}

void BSP_MNTN_SystemError(int modId, int arg1, int arg2, char * arg3, int arg3Length)
{
    systemError( modId, arg1, arg2, arg3, arg3Length);
}
/*****************************************************************************
* 函 数 名  : BSP_GetMacAddr
*
* 功能描述  : 获取ps mac地址
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : VOID
*
* 修改记录  : 2009年9月2日   liumengcun  creat
*****************************************************************************/
UINT8* BSP_GetMacAddr(BSP_VOID)
{
    return g_mac_address_pstable;
}

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
		BSP_U8 u8MacAddr3,BSP_U8 u8MacAddr4,BSP_U8 u8MacAddr5)
{
	g_mac_address_pstable[0] = u8MacAddr0;
    g_mac_address_pstable[1] = u8MacAddr1;
    g_mac_address_pstable[2] = u8MacAddr2;
    g_mac_address_pstable[3] = u8MacAddr3;
    g_mac_address_pstable[4] = u8MacAddr4;
    g_mac_address_pstable[5] = u8MacAddr5;

	return OK;
}

#if 0
const UE_SW_BUILD_VER_INFO_STRU* BSP_GetBootBuildVersion(VOID)
{
    return NULL;
}
#endif


unsigned int   * MemAddr32 = 0x00000000;

/*******************************************************************************
*
* writeM - write a unsigned int value to perihical address
*
*
* RETURNS: N/A
*/
void writeM(unsigned int* pAddr, unsigned int value)
{
    *pAddr = value;
	return;
}


/***********************************************************
 Function: memRead32--read memory and display the value
 Input:    the start address
 return:  void
 see also:memRead16/memRead8
 History:
 1.    2004.9.30   Creat
 2.    2007.3.20   Code stardand
************************************************************/
void memRead32 (unsigned int * uiMemAddr32)
{
    if (0 != uiMemAddr32)
    {
         MemAddr32 = (unsigned int *)((unsigned long)uiMemAddr32 & (~0x3));
    }
	
	printk("0x%p: 0x%x   0x%x   0x%x   0x%x\n",
            MemAddr32, *MemAddr32, *((unsigned char*)MemAddr32+sizeof(void*)), *((unsigned char*)MemAddr32+sizeof(void*)*2), 
            *((unsigned char*)MemAddr32+sizeof(void*)*3));
    printk("0x%p: 0x%x   0x%x   0x%x   0x%x\n",
            ((unsigned char*)MemAddr32+sizeof(void*)), *((unsigned char*)MemAddr32+sizeof(void*)*4), *((unsigned char*)MemAddr32+sizeof(void*)*5), 
            *((unsigned char*)MemAddr32+sizeof(void*)*6), *((unsigned char*)MemAddr32+sizeof(void*)*7));
    printk("0x%p: 0x%x   0x%x   0x%x   0x%x\n",
            ((unsigned char*)MemAddr32+sizeof(void*)*2), *((unsigned char*)MemAddr32+sizeof(void*)*8), *((unsigned char*)MemAddr32+sizeof(void*)*9), 
            *((unsigned char*)MemAddr32+sizeof(void*)*0xa), *((unsigned char*)MemAddr32+sizeof(void*)*0xb));
    printk("0x%p: 0x%x   0x%x   0x%x   0x%x\n",
            ((unsigned char*)MemAddr32+sizeof(void*)*3), *((unsigned char*)MemAddr32+sizeof(void*)*0xc), *((unsigned char*)MemAddr32+sizeof(void*)*0xd),
            *((unsigned char*)MemAddr32+sizeof(void*)*0xe), *((unsigned char*)MemAddr32+sizeof(void*)*0xf));
	
    MemAddr32 = (unsigned int*)((unsigned char*)MemAddr32 + 0x10);
}

/***********************************************************
 Function: r--simple read memory command
 Input:    the start address
 return:  void
 see also:memRead16/memRead8/memRead32
 History:
 1.    2004.9.30   Creat
 2.    2007.3.20   Code stardand
************************************************************/
void r(unsigned int * newMemAddr32)
{
    memRead32(newMemAddr32);
}


int DRV_GET_DIEID(unsigned char* buf,int lenth)
{
#ifdef AT_DBG
	int index = 0;
#endif	
	int ret = 0;
	
    if(NULL == buf)
    {
        printk("the chip buf is null\n");
        return BUF_ERROR;
    }

    if( 0 >= lenth )
    {
        printk("the lenth is litter :%d\n",lenth);
        return LEN_ERROR;
    }

	if (DIEID_BYTE_LEN > lenth){
		printk("the lenth is litter :%d\n",lenth);
		return LEN_ERROR;
	}
	
    ret = get_efuse_dieid_value(buf, DIEID_BYTE_LEN, 1000);
    if (ret < 0) {
        pr_err("%s: %d: efuse_get_DieID failed.\n", __func__, __LINE__);
		return READ_EFUSE_DIEID_ERROR;
    }    

#ifdef AT_DBG
    printk(KERN_DEBUG "\n==========in DRV_GET_DIEID============\n");
	for(; index < DIEID_BYTE_LEN; index++)
	{
		printk(KERN_DEBUG "%#x ",buf[index]);
	}
    printk(KERN_DEBUG "\n==========in DRV_GET_DIEID============\n");
#endif
    return OK;
}


int DRV_GET_CHIPID(unsigned char* buf,int lenth)
{
#ifdef AT_DBG
	int index = 0;
#endif
	int ret = 0;

    if(NULL == buf)
    {
        printk("the die buf is null\n");
        return BUF_ERROR;
    }

    if( 0 >= lenth )
    {
        printk("the lenth is litter :%d\n",lenth);
        return LEN_ERROR;
    }    
	
	if (CHIPID_BYTE_LEN > lenth){
		printk("the lenth is litter :%d\n",lenth);
		return LEN_ERROR;
	}

    ret = get_efuse_chipid_value(buf, CHIPID_BYTE_LEN, 1000);
	if (ret < 0) {
		pr_err("%s: %d: efuse_read_chipID failed.\n", __func__, __LINE__);
		return READ_EFUSE_CHIPID_ERROR;
	}

#ifdef AT_DBG
    printk(KERN_DEBUG "\n==========in DRV_GET_CHIPID============\n");

	for(; index < CHIPID_BYTE_LEN; index++)
	{
		printk(KERN_DEBUG "%#x ",buf[index]);
	}
    printk(KERN_DEBUG "\n==========in DRV_GET_CHIPID============\n");
#endif
    return OK;
}



void Test_DIEID(void)
{
	unsigned char buf[20] = {0};
	int index;
	int result;
    printk("Test_DIEID\n");

	result = DRV_GET_DIEID(buf, DIEID_BYTE_LEN);

    printk("\n==========CALL DRV_GET_DIEID============\n");
	if(OK == result)
    {
		for(index = 0; index < DIEID_BYTE_LEN; index++)
		{
			printk("%#x ", buf[index]);
		}
	}
	printk("\n==========CALL DRV_GET_DIEID============\n");
	return ;
}


void Test_CHIPID(void)
{
	unsigned char buf[8] = {0};
	int index;
	int result;
    printk("Test_CHIPID\n");

	result = DRV_GET_CHIPID(buf, CHIPID_BYTE_LEN);

    printk("\n==========CALL DRV_GET_CHIPID============\n");
	if(OK == result)
	{
		for(index = 0; index < CHIPID_BYTE_LEN; index++)
		{
			printk("%#x ", buf[index]);
		}
	}
	printk("\n==========CALL DRV_GET_CHIPID============\n");
	return ;
}

EXPORT_SYMBOL(Test_CHIPID);
EXPORT_SYMBOL(Test_DIEID);

EXPORT_SYMBOL(DRV_GET_DIEID);
EXPORT_SYMBOL(DRV_GET_CHIPID);
EXPORT_SYMBOL(BSP_GetBootBuildVersion);
EXPORT_SYMBOL(BSP_OM_RegRead);
EXPORT_SYMBOL(BBPGetCurTime);
EXPORT_SYMBOL(BSP_OM_ShellLock);
EXPORT_SYMBOL(BSP_OM_GetBoardType);
EXPORT_SYMBOL(BSP_GetBuildVersion);
EXPORT_SYMBOL(BSP_OM_RegWrite);
EXPORT_SYMBOL(BSP_OM_SoftReboot);
EXPORT_SYMBOL(BSP_MNTN_ExchMemMalloc);
EXPORT_SYMBOL(BSP_MNTN_SystemError);
EXPORT_SYMBOL(BSP_GetMacAddr);
EXPORT_SYMBOL(BSP_SetPsMacAddr);
#ifdef __cplusplus
}
#endif

