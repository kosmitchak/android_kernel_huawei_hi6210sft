
#ifndef __DRV_IPCM_H__
#define __DRV_IPCM_H__
#include "drv_global.h"
#include "drv_ipc_enum.h"
#ifdef __OS_RTOSCK__

#if defined(BSP_CORE_MODEM) 
#include <sre_base.h>
#endif

#else 

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM) || defined(__VXWORKS__)
#include <vxWorks.h>
#endif

#endif


#define INTSRC_NUM                     32


/*****************************************************************************
* 函 数 名  : BSP_DRV_IPCIntInit_Lt
*
* 功能描述  : IPC模块初始化,L模专用
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  :
*****************************************************************************/
int BSP_DRV_IPCIntInit_Lt(void);

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntEnable_Lt
*
* 功能描述  : 使能某个中断,L模专用
*
* 输入参数  :   IPC_INT_CORE_E enCoreNum 要使能中断的core
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :
*****************************************************************************/
int BSP_IPC_IntEnable_Lt (IPC_INT_LEV_E ulLvl);

/*****************************************************************************
 * 函 数 名  : BSP_INT_Disable_Lt
 *
 * 功能描述  : 去使能某个中断
 *
 * 输入参数  : IPC_INT_CORE_E enCoreNum 要使能中断的core
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  :
 *****************************************************************************/
int BSP_IPC_IntDisable_Lt (IPC_INT_LEV_E ulLvl);

/*****************************************************************************
 * 函 数 名  : BSP_IPC_IntConnect_Lt
 *
 * 功能描述  : 注册某个中断,L模专用
 *
 * 输入参数  : IPC_INT_CORE_E enCoreNum 要使能中断的core
               BSP_U32 ulLvl 要使能的中断号，取值范围0～31
               VOIDFUNCPTR routine 中断服务程序
 *             BSP_U32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  :
 *****************************************************************************/
int BSP_IPC_IntConnect_Lt(IPC_INT_LEV_E ulLvl, VOIDFUNCPTR routine, unsigned int parameter);

/*****************************************************************************
 * 函 数 名  : BSP_IPC_IntDisonnect_Lt
 *
 * 功能描述  : 取消注册某个中断,L模专用
 *
 * 输入参数  :
 *              BSP_U32 ulLvl 要使能的中断号，取值范围0～31
 *              VOIDFUNCPTR routine 中断服务程序
 *             BSP_U32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  :
 *****************************************************************************/
 int BSP_IPC_IntDisonnect_Lt (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, unsigned int parameter);

 /*****************************************************************************
 * 函 数 名  : BSP_DRV_IpcIntHandler_Lt
 *
 * 功能描述  : 中断处理函数,L模专用
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  :
 *****************************************************************************/
void BSP_DRV_IpcIntHandler_Lt(void);

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntSend_Lt
*
* 功能描述  : 发送中断,L模专用
*
* 输入参数  :
                IPC_INT_CORE_E enDstore 要接收中断的core
                BSP_U32 ulLvl 要发送的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :
*****************************************************************************/
int BSP_IPC_IntSend_Lt(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);



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
int BSP_DRV_IPCIntInit(void);


/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMCREATE
*
* 功能描述  : 信号量创建函数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
int BSP_IPC_SemCreate(unsigned int u32SignalNum);
extern int DRV_IPC_SEMCREATE(unsigned int u32SignalNum);

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMDELETE
*
* 功能描述  : 删除信号量
*
* 输入参数  :   BSP_U32 u32SignalNum 要删除的信号量编号

* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
int BSP_IPC_SemDelete(unsigned int u32SignalNum);
extern int DRV_IPC_SEMDELETE(unsigned int u32SignalNum);

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
int BSP_IPC_IntEnable (IPC_INT_LEV_E ulLvl);
extern int DRV_IPC_INTENABLE(IPC_INT_LEV_E ulLvl);

/*****************************************************************************
* 函 数 名  : DRV_IPC_INTDISABLE
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
int BSP_IPC_IntDisable (IPC_INT_LEV_E ulLvl);
extern int DRV_IPC_INTDISABLE(IPC_INT_LEV_E ulLvl);

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntConnect
*
* 功能描述  : 注册某个中断
*
* 输入参数  :
           BSP_U32 ulLvl 要使能的中断号，取值范围0～31
           VOIDFUNCPTR routine 中断服务程序
*             BSP_U32 parameter      中断服务程序参数
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
int BSP_IPC_IntConnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, unsigned int parameter);
extern int DRV_IPC_INTCONNECT(IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, unsigned int parameter);

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntDisonnect
*
* 功能描述  : 取消注册某个中断
*
* 输入参数  :
*              BSP_U32 ulLvl 要使能的中断号，取值范围0～31
*              VOIDFUNCPTR routine 中断服务程序
*             BSP_U32 parameter      中断服务程序参数
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
int BSP_IPC_IntDisonnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, unsigned int parameter);
extern int DRV_IPC_INTDISCONNECT  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, unsigned int parameter);

/*****************************************************************************
* 函 数 名  : DRV_IPC_INTSEND
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
int BSP_IPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);
extern int DRV_IPC_INTSEND(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMTAKE
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
int BSP_IPC_SemTake(unsigned int u32SignalNum,int s32timeout);
extern int DRV_IPC_SEMTAKE(unsigned int u32SignalNum, int s32timeout);

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMGIVE
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
void BSP_IPC_SemGive(unsigned int u32SignalNum);
extern void DRV_IPC_SEMGIVE(unsigned int u32SignalNum);


void BSP_IPC_SemOwner(unsigned int u32SignalNum, IPC_INT_CORE_E *coreId);

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
void BSP_IPC_SpinLock (unsigned int u32SignalNum);
extern void DRV_SPIN_LOCK (unsigned int u32SignalNum);

/*****************************************************************************
* 函 数 名  : DRV_SPIN_UNLOCK
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
void BSP_IPC_SpinUnLock (unsigned int u32SignalNum);
extern void DRV_SPIN_UNLOCK(unsigned int u32SignalNum);


int BSP_SGI_Connect(unsigned int ulLvl,VOIDFUNCPTR routine, unsigned int parameter);
int BSP_SGI_IntSend(unsigned int ulLvl);

#endif

