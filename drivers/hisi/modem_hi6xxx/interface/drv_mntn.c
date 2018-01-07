/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_mntn.c
  版 本 号   : 初稿
  作    者   : c61362
  生成日期   : 2012年3月2日
  最近修改   :
  功能描述   : 底软给上层软件封装的接口层
  修改历史   :
  1.日    期   : 2012年3月2日
    作    者   : c61362
    修改内容   : 新建Drvinterface.c

  2.日    期   : 2013年2月19日
    作    者   : 蔡喜 220237
    修改内容   : 由Drvinterface.c拆分所得

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BSP.h"
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#include "board_diff.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern int exch_notifier_chain_register(exchCBReg cb);
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*******************************************************************************
 函 数 名: DRV_EXCH_CUST_FUNC_REG
 功能描述: 临终遗言模块提供对外接口，供上层注册回调，用于复位时保存上层指定信息
 输入参数: cb 上层指定回调接口
 输出参数: 无
 返 回 值: 0-添加成功，其他-添加失败
 说明    :  pucData :指定记录信息地址，请注意不要指定栈等临时数据区，堆空间数据，
                     使用者自己维护，包括空间使用完，自己释放。建议全局变量。
            aucName :名称
            ulDataLen: 数据长度

规格约束:
            缓冲区大小:64K。共分8个通道，每个通道均值最大不得超过8k.
            注册函数返回值为:
                正常:    ulDataLen data数据长度；
                异常:    0

返回值列表:
    CUST_REG_OK = 0,                        添加成功
    CUST_REG_PARM_NULL,                     入参为空
    CUST_REG_EXCH_NOT_READY,                EXCH模块尚未初始化
    CUST_REG_EXCH_REBOOT_CONTEXT,           已经进入复位流程
    CUST_REG_EXCH_QUEUE_NOT_INIT,           注册队列尚未初始化
    CUST_REG_EXCH_QUEUE_FULL,               队列已满
    CUST_REG_EXCH_ALREADY_IN_QUEUE,         重复注册
    CUST_REG_EXCH_MUTEX_REJECT,             MUTEX REJECT
    CUST_REG_EXCH_QUEUE_IN_ERR              入队列失败

请通过返回值定位解决错误

*******************************************************************************/
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
int DRV_EXCH_CUST_FUNC_REG(exchCBReg cb)
{
    return exch_notifier_chain_register(cb);
}
#endif

/*****************************************************************************
 函 数 名  : DRV_MEM_READ
 功能描述  : 按32位宽度查询内存，输出4字节数据。可维可测，V7先打桩
 输入参数  : ulAddress：查询地址空间地址，地址需在内存范围内，否则返回-1。
 输出参数  : pulData：指向查询内容的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
int DRV_MEM_READ (unsigned int ulAddress, unsigned int* pulData)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_MEM_READ(ulAddress, pulData);
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

void drvShutdown(DRV_SHUTDOWN_REASON_ENUM eReason)
{
    return;   /* 打桩 */
}

/*****************************************************************************
 函 数 名  : DRV_SHUT_DOWN
 功能描述  : 单板关机
 输入参数  : 无
 输出参数  : eReason：        关机原因
 返 回 值  : 无
*****************************************************************************/
void DRV_SHUT_DOWN( DRV_SHUTDOWN_REASON_ENUM eReason )
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#if( FEATURE_POWER_ON_OFF == FEATURE_ON )
    drvShutdown(eReason);
#else
    DRV_PRINTF("Function DRV_SHUT_DOWN unsupport!\n");
#endif
#endif

#if (defined BSP_CORE_APP)
    return;   /* 打桩 */
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_MSP_PROC_REG
*
* 功能描述  : DRV提供给OM的注册函数
*
* 输入参数  : MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*****************************************************************************/
void DRV_MSP_PROC_REG(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    BSP_MspProcReg(eFuncID, pFunc);
#endif
#if (defined BSP_CORE_APP)
    /*BSP_MspProcReg(eFuncID, pFunc);*/
#endif
}


#if 1 //shenwei shenwei she

/*************************************************
 函 数 名   : BSP_MNTN_ProductTypeGet
 功能描述   : 获取当前产品类型
 输入参数   : 无
 输出参数   :
 返 回 值   :
    PROD_STICK  =0,
    PROD_MOD    =1,
    PROD_E5     =2,
    PROD_CPE    =3,
 调用函数   :
 被调函数   :
 修改历史   :
 日    期   : 2011年4月18日
 作    者   : C54299
 修改内容   : 新生成函数

*************************************************/
BSP_U32 BSP_MNTN_ProductTypeGet(void)
{
   return 0;/*stub*/
}
#endif

/*****************************************************************************
 函 数 名  : BSP_MNTN_GETSDSTATUS
 功能描述  : 返回SD卡在位状态
 输入参数  : 无。
 输出参数  : 无。
 返回值：   1在位
            0不在位

*****************************************************************************/
int BSP_SDMMC_GetSDStatus(void)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return 0;
#endif
}

/*****************************************************************************
* 函 数 名  : BSP_OM_GetChipType
*
* 功能描述  : 获取芯片类型,HSO数采使用
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP芯片类型枚举
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_CHIP_TYPE_E BSP_OM_GetChipType(void)
{
    /*后续需要和北京hids核对*/
    return V8R1_PILOT_CHIP;
}

/*****************************************************************************
* 函 数 名  : BSP_TFUP_CompleteDeal
* 功能描述  : 装备使用,TL使用
* 输入参数  :
* 输出参数  :
* 返 回 值  :
* 修改记录  :
*****************************************************************************/
BSP_VOID BSP_TFUP_CompleteDeal(void)
{
   return ; /* 打桩 */
}

/*****************************************************************************
* 函 数 名  : BSP_TBAT_Read
* 功能描述  : 装备使用,TL使用
* 输入参数  :
* 输出参数  :
* 返 回 值  :
* 修改记录  :
*****************************************************************************/
BSP_S32 BSP_TBAT_Read(ENUM_CHG_ATCM_TYPE atID, void *pItem)
{
    return  0; /* 打桩 */
}

/*****************************************************************************
* 函 数 名  : BSP_TBAT_Write
* 功能描述  : 装备使用,TL使用
* 输入参数  :
* 输出参数  :
* 返 回 值  :
*****************************************************************************/
BSP_S32 BSP_TBAT_Write(ENUM_CHG_ATCM_TYPE atID, void *pItem)
{
    return 0; /* 打桩 */
}

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetSoftwareVer
* 功能描述  : 获取软件版本号
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
* 返 回 值  : BSP_ERROR     :输入参数非法
*             其它          :返回字符串长度
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetSoftwareVer(BSP_CHAR *str, BSP_U32 len)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_MNTN_MemVersionCtrl((signed char*)str, (unsigned char)len, (unsigned char)VER_SOFTWARE, (unsigned char)VERIONREADMODE);
#endif

#if (defined BSP_CORE_APP)
    return 0;
#endif
}

/*****************************************************************************
* 函 数 名  : BSP_BBPDMA_DrxBakReg
* 功能描述  :
* 输入参数  :
* 输出参数  :
* 返 回 值  :
* 修改记录  :
*****************************************************************************/
BSP_S32 BSP_BBPDMA_DrxBakReg(void)
{
    return  0; /* 打桩 */
}

/*****************************************************************************
* 函 数 名  : BSP_CPU_GetTotalUtilize
* 功能描述  :
* 输入参数  :
* 输出参数  :
* 返 回 值  :
*****************************************************************************/
BSP_U32 BSP_CPU_GetTotalUtilize(void)
{
    return  0; /* 打桩 */
}

/*****************************************************************************
* 函 数 名  : nand_isbad
* 功能描述  :
* 输入参数  :
* 输出参数  :
* 返 回 值  :
*****************************************************************************/
BSP_U32 nand_isbad(BSP_U32 blockID, BSP_U32 *flag)
{
    return 0; /* 打桩,返回NAND_OK */
}

/*****************************************************************************
* 函 数 名  : BSP_PM_SetWakeSrc
* 功能描述  : 动态设置唤醒源
* 输入参数  : eWakeSrc 唤醒源枚举值
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
BSP_VOID BSP_PM_SetWakeSrc(DRV_PM_WAKE_SRC_ENUM wake_src)
{
    /*V9R1打桩处理*/
    return;
}

/*****************************************************************************
* 函 数 名  : BSP_PM_UnsetWakeSrc
* 功能描述  : 动态设置唤醒源
* 输入参数  : eWakeSrc 唤醒源枚举值
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
BSP_VOID BSP_PM_UnsetWakeSrc(DRV_PM_WAKE_SRC_ENUM wake_src)
{
    /*V9R1打桩处理*/
    return;
}

/********************************************************************************************************
 函 数 名  : BSP_FS_GetDskspc
 功能描述  : 得到文件系统空间。(Modem Core,/yaffs0)
 输入参数  : path：文件系统路径。
 输出参数  : DskSpc：总空间。
                           UsdSpc：用过空间。
                           VldSpc：空闲空间。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
int BSP_FS_GetDskspc(const char *path,unsigned int *DskSpc,unsigned int *UsdSpc,  unsigned int *VldSpc)
{
    return 0;
}

/* stub for v8r1 link */
#if defined (CHIP_BB_HI6210)
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
int uart_core_send(UART_CONSUMER_ID uPortNo, unsigned char * pDataBuffer, unsigned int uslength)
{
    return BSP_ERROR;
}

int uart_core_recv_handler_register(UART_CONSUMER_ID uPortNo, pUARTRecv pCallback)
{
    return BSP_ERROR;
}
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

