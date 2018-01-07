/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWrXNs8MZYk9cpFfgXfKX0g1VVtQNxHnHCKsK8caFMiW8LvCCkwIW
Tc6iJlHGFi3nUeJwZnaqeEUBwq/bzU5cp8JMIZQR60CPwhsVGkig7D46L1mTcjaBXLwtoHt/
6v3DaoJKLOkHnsHop7n+e+dpNEZbcEUANAhREkDhyi9k9JiD8innzTXtzoD3G9IhJgTHUk7k
ETG2NI2nqdzZcPC3BMtcGARKklVHocpE39IMocylCJhLsvboEP3ws+F39pdOlQ==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/**
 * @file sre_hw.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * 描述：硬件相关模块的对外头文件。 \n
 */

 /**@defgroup SRE_hw 多实例支持
   *@ingroup SRE_kernel
 */

#ifndef _SRE_HW_H
#define _SRE_HW_H

#include "sre_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/**
 * @ingroup SRE_hw
 * 内存错误码：配置实例启动参数时传入的参数为NULL
 *
 * 值: 0x02002000
 *
 * 解决方案: 请确保传入的参数不为NULL
 */
#define OS_ERRNO_HW_START_PARAM_CFG_NULL                      SRE_ERRNO_OS_ERROR(OS_MID_HW, 0x00)

/**
 * @ingroup SRE_hw
 * 内存错误码：配置实例启动参数时，实例内各个核配置不一致
 *
 * 值: 0x02002001
 *
 * 解决方案: 请确保实例内所有核配置的启动参数一致
 */
#define OS_ERRNO_HW_START_PARAM_INCONSISTENT                  SRE_ERRNO_OS_ERROR(OS_MID_HW, 0x01)

/**
 * @ingroup SRE_hw
 * 内存错误码：核启动时，未在#SRE_HardBootInit中调用#SRE_StartParamCfg接口进行启动参数配置
 *
 * 值: 0x02002002
 *
 * 解决方案: 请确保有在#SRE_HardBootInit中调用#SRE_StartParamCfg接口进行启动参数配置
 */
#define OS_ERRNO_HW_START_PARAM_NOT_CFG                       SRE_ERRNO_OS_ERROR(OS_MID_HW, 0x02)

/**
 * @ingroup SRE_hw
 * 内存错误码：设备ID错误
 *
 * 值: 0x02002003
 *
 * 解决方案: 设备ID的取值[0 ,2]
 */
#define OS_ERRNO_HW_DEVICE_ID_ERR                             SRE_ERRNO_OS_ERROR(OS_MID_HW, 0x03)

/**
 * @ingroup SRE_hw
 * 内存错误码：uwCommVmid参数传入错误
 *
 * 值: 0x02002004
 *
 * 解决方案: COMM VMID的取值范围[0 ,7]
 */
#define OS_ERRNO_HW_VMID_ID_ERR                               SRE_ERRNO_OS_ERROR(OS_MID_HW, 0x04)

/**
 * @ingroup SRE_Hw
 * 硬件相关信息。
 *
 */
typedef struct tagHwModInfo
{
    UINT32 uwGiccCtrlOffset;      /**< 芯片GIC CPU Interface地址偏移，Cortex_AX系列芯片有效 */
} HW_MOD_INFO_S;

#if ((OS_HARDWARE_PLATFORM == OS_DSP220) || (OS_HARDWARE_PLATFORM == OS_RISC220) || (OS_HARDWARE_PLATFORM == OS_HACCC))
/**@ingroup SRE_Hw
 * 启动参数结构体定义
 */

typedef struct tagStartParam
{
    UINT16 usInstID;              /**< 实例ID，保留这个配置项用于扩展             */
    UINT16 usReserved;            /**< 保留 */
    UINT16 usGHwSemStartID;       /**< 本实例管理的Global Hwsem起始物理ID */
    UINT16 usGHwSemNum;           /**< 本实例管理的Global Hwsem个数*/
    UINT16 usCpuLHwSemStartID;    /**< 本实例管理的CPU(SD6183 DSP2) cluster Local Hwsem起始物理ID */
    UINT16 usCpuLHwSemNum;        /**< 本实例管理的CPU(SD6183 DSP2) cluster Local Hwsem个数*/
    UINT16 usHacLHwSemStartID;    /**< 本实例管理的HACCC cluster Local Hwsem起始物理ID */
    UINT16 usHacLHwSemNum;        /**< 本实例管理的HACCC cluster Local Hwsem个数*/
    UINT16 usDsp0LHwSemStartID;   /**< 本实例管理的DSP0 cluster Local Hwsem起始物理ID */
    UINT16 usDsp0LHwSemNum;       /**< 本实例管理的DSP0 cluster Local Hwsem个数*/
    UINT16 usDsp1LHwSemStartID;   /**< 本实例管理的DSP1 cluster Local Hwsem起始物理ID */
    UINT16 usDsp1LHwSemNum;       /**< 本实例管理的DSP1 cluster Local hwsem个数*/
    UINT16 usNorGMsgmBufStart;    /**< 本实例管理的Global MSGM普通消息缓冲起始地址(SD6183无效)*/
    UINT16 usNorGMsgmBufLen;      /**< 本实例管理的Global MSGM普通消息缓冲长度(SD6183无效)*/
    UINT16 usNorLMsgmBufStart;    /**< 本实例管理的Local MSGM普通消息缓冲起始地址(SD6183无效)*/
    UINT16 usNorLMsgmBufLen;      /**< 本实例管理的Local MSGM普通消息缓冲长度(SD6183无效)*/
 } START_PARAM_S;
#elif ((OS_HARDWARE_PLATFORM == OS_HIDSP))

/**@ingroup SRE_Hw
 * 启动参数结构体定义
 */
typedef struct tagStartParam
{
    UINT16 usInstID;             /**< 实例ID，保留这个配置项用于扩展     */
    UINT16 usReserved;           /**< 保留 */
    UINT16 usGHwSemStartID;      /**< 本实例管理的Global Hwsem起始物理ID */
    UINT16 usGHwSemNum;          /**< 本实例管理的Global Hwsem个数*/
    UINT16 usDsp0LHwSemStartID;  /**< 本实例管理的DSP cluster0 Local Hwsem起始物理ID */
    UINT16 usDsp0LHwSemNum;      /**< 本实例管理的DSP cluster0 Local Hwsem个数*/
    UINT16 usDsp1LHwSemStartID;  /**< 本实例管理的DSP cluster1 Local Hwsem起始物理ID */
    UINT16 usDsp1LHwSemNum;      /**< 本实例管理的DSP cluster1 Local Hwsem个数*/
    UINT16 usDsp2LHwSemStartID;  /**< 本实例管理的DSP cluster2 Local Hwsem起始物理ID */
    UINT16 usDsp2LHwSemNum;      /**< 本实例管理的DSP cluster2 Local Hwsem个数*/
    UINT16 usHacLHwSemStartID;   /**< 本实例管理的HACCC cluster Local Hwsem起始物理ID */
    UINT16 usHacLHwSemNum;       /**< 本实例管理的HACCC cluster Local Hwsem个数*/
 } START_PARAM_S;
#endif

#if ((OS_HARDWARE_PLATFORM == OS_HIDSP) || (OS_HARDWARE_PLATFORM == OS_DSP220) \
|| (OS_HARDWARE_PLATFORM == OS_RISC220) || (OS_HARDWARE_PLATFORM == OS_HACCC))
/**
 * @ingroup  SRE_hw
 * @brief 配置实例启动参数。
 *
 * @par 描述:
 * 根据pstStartParam的内容配置实例启动参数，OS根据该配置将Hwsem和MSGM队列资源按实例进行分段管理。
 *
 * @attention
 * <ul>
 * <li>只支持SD6182/SD6157/SD6183平台。</li>
 * <li>用户必须在#SRE_HardBootInit中且在DDR代码加载后调用该函数，否则初始化失败。</li>
 * <li>实例间配置的资源分段不能重叠，该合法性由用户保证。</li>
 * <li>OS固定使用0号硬件信号量作为实例间互斥，用户无需通过该API将0号硬件信号量配置给OS。</li>
 * </ul>
 *
 * @param  pstStartParam   [IN] 类型#START_PARAM_S，启动参数。
 *
 * @retval #OS_ERRNO_HW_START_PARAM_CFG_NULL          0x02002000，配置实例启动参数时传入的参数为NULL。
 * @retval #OS_ERRNO_HW_START_PARAM_INCONSISTENT      0x02002001，配置实例启动参数时，实例内各个核配置不一致。
 * @retval #OS_ERRNO_HW_START_PARAM_NOT_CFG           0x02002002，核启动时，未在#SRE_HardBootInit中调用#SRE_StartParamCfg接口进行启动参数配置。
 * @retval #OS_ERRNO_HWSEM_CFG_GLB_START_ID_INVALID   0x02002425，配置实例启动参数时传入的全局硬件信号量起始ID非法。
 * @retval #OS_ERRNO_HWSEM_CFG_GLB_NUM_INVALID        0x02002426，配置实例启动参数时传入的全局硬件信号量个数非法
 * @retval #OS_ERRNO_HWSEM_CFG_GLB_REGION_INVALID     0x02002427，配置实例启动参数时配置的全局硬件信号量区间非法。
 * @retval #OS_ERRNO_HWSEM_CFG_LOCAL_START_ID_INVALID 0x02002428，配置实例启动参数时传入的局部硬件信号量起始ID过大。
 * @retval #OS_ERRNO_HWSEM_CFG_LOCAL_NUM_INVALID      0x02002429，配置实例启动参数时传入的局部硬件信号量个数非法。
 * @retval #OS_ERRNO_HWSEM_CFG_LOCAL_REGION_INVALID   0x0200242a，配置实例启动参数时配置的局部硬件信号量区间非法。
 * @retval #OS_ERRNO_MSGM_CFG_ADDR_OR_LEN_NOT_ALIGN   0x02002b3f，配置实例启动参数时传入的MSGM队列起始地址或长度非4字节对齐。
 * @retval #OS_ERRNO_MSGM_CFG_START_ADDR_INVALID      0x02002b40，配置实例启动参数时传入的MSGM队列起始地址非法。
 * @retval #OS_ERRNO_MSGM_CFG_QUEUE_LEN_INVALID       0x02002b41，配置实例启动参数时传入的MSGM队列长度非法。
 * @retval #OS_ERRNO_MSGM_CFG_QUEUE_REGION_INVALID    0x02002b42，配置实例启动参数时传入的MSGM队列起始地址和长度决定的区间非法。
 * @retval #SRE_OK                                    0x00000000，启动参数配置成功。
 * @par 依赖:
 * <ul><li>sre_hw.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see
 **/
extern UINT32 SRE_StartParamCfg(START_PARAM_S  *pstStartParam);
#endif

#if (OS_HARDWARE_PLATFORM == OS_HIDSP)
/**
 * @ingroup sre_hwsem
 * 设备编号
 */
typedef enum _tagOsDevice
{
    OS_DEVICE_HWSEM_ID = 0,/**< 硬件信号量ID*/
    OS_DEVICE_MSGM_ID = 1, /**< MSGM ID     */
    OS_DEVICE_TIMER_ID = 2 /**< 全局定时器ID*/
} OS_DEVICE_TYPE_E;

/**
 * @ingroup SRE_hw
 * @brief Comm VMID配置接口。
 *
 * @par 描述:
 * 设置COMM VMID并使能VMID检查。
 * @attention
 * <ul>
 * <li>适用于SD6183平台。</li>
 * </ul>
 *
 * @param uwDeviceID    [IN] 类型#UINT32，设备ID，取值范围[0,2],含义参考#OS_DEVICE_TYPE_E。
 * @param uwCommVmid    [IN] 类型#UINT32，Comm Vmid值，取值范围[0,7]。
 *
 * @retval #OS_ERRNO_HW_VMID_ID_ERR      0x02002004，uwCommVmid参数传入错误。
 * @retval #OS_ERRNO_HW_DEVICE_ID_ERR    0x02002003，设备ID错误。
 * @retval #SRE_OK      0x00000000，返回成功。
 *
 * @par 依赖:
 * <ul><li>sre_hw.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R003C00
 * @see 无
 */
extern UINT32 SRE_CommVmidSet(UINT32 uwDeviceID, UINT32 uwCommVmid);
#endif
#if (OS_HARDWARE_PLATFORM == OS_CORTEX_AX)
/**
 * @ingroup  SRE_hw
 * @brief ARM的DSB功能
 *
 * @par 描述:
 * ARM的DSB功能，等待内存操作完毕，内存操作未结束之前，CPU不会继续操作内存和寄存器。
 *
 * @attention
 * <ul>
 * <li>只支持Cortex-AX平台。</li>
 * </ul>
 *
 * @param  无
 *
 * @retval 无
 * @par 依赖:
 * <ul><li>sre_hw.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see SRE_DMB
 **/
#ifndef _WIN32
#define SRE_DSB() OS_EMBED_ASM("dsb")
#else
#define SRE_DSB()
#endif

/**
 * @ingroup  SRE_hw
 * @brief ARM的DMB功能
 *
 * @par 描述:
 * ARM的DMB功能，等待内存操作完毕，内存操作未结束之前，CPU不会继续操作内存。
 *
 * @attention
 * <ul>
 * <li>只支持Cortex-AX平台。</li>
 * </ul>
 *
 * @param  无
 *
 * @retval 无
 * @par 依赖:
 * <ul><li>sre_hw.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see SRE_DSB
 **/
#ifndef _WIN32
#define SRE_DMB() OS_EMBED_ASM("dmb")
#else
#define SRE_DMB()
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#endif /* _SRE_HW_H */

/**
 * History:
 * 2013-05-10 w00181871: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */

