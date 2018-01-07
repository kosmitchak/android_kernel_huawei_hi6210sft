/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWrXNs8MZYk9cpFfgXfKX0g1VVtQNxHnHCKsK8caFMiW8LvCCkwIW
Tc6iJlHGFi3nUeJwZnaqeEUBwq/bzU5cp8JMIZQR60CPwhsVGkig7D46L1mTcjaBXLwtoHt/
6v3DaneHn/PPsaTkENSiCcGkl8kpPTA+l4hwnhrch+dojc2qdty7g4iKsR0T1Z1/77poPwVR
JysM6QG6mcIX6Ayw9cLtombhz7CMiwpAwxG/lspGRXM6J9nOSwe5NfxnYrlD+w==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/**
 * @file sre_sys.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * 描述：系统模块的对外头文件。 \n
 */

/** @defgroup SRE_sys 系统基本功能接口
 *@ingroup SRE_kernel
 */

#ifndef _SRE_SYS_H
#define _SRE_SYS_H

#include "sre_base.h"
#include "sre_cache.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/**
 * @ingroup SRE_sys
 * 系统初始化阶段各状态定义
 * uwInitPhase的高16位即bit[31-16]为下列定义的系统初始化阶段状态值。
*/

/*********************************************系统初始化阶段状态********************************************************/
/**
 * @ingroup SRE_sys
 * 表示初始态。
 *
 */
#define OS_DEFAULT_PHASE                                        (0x00)

/**
 * @ingroup SRE_sys
 * 表示进入SRE_HardBootInit。
 *
 */
#define OS_BOOT_PHASE                                           (0x01)

/**
 * @ingroup SRE_sys
 * 表示开始多实例启动参数配置。
 *
 */
#define OS_STARTPARAMCFG_PHASE                                  (0x02)

/**
 * @ingroup SRE_sys
 * 表示退出SRE_HardBootInit。
 *
 */
#define OS_AFTER_BOOT_PHASE                                     (0x03)

/**
 * @ingroup SRE_sys
 * 表示进入BSS段初始化。
 *
 */
#define OS_BSSINIT_PHASE                                        (0x04)

/**
 * @ingroup SRE_sys
 * 表示Dcache初始化完毕。
 *
 */
#define OS_DCACHEINIT_PHASE                                     (0x05)

/**
 * @ingroup SRE_sys
 * 表示进入C lib库初始化。
 *
 */
#define OS_LIBCINIT_PHASE                                       (0x06)

/**
 * @ingroup SRE_sys
 * 表示系统在进行OS模块注册阶段，匹配MOUDLE_ID之后，标记进入MODULE_ID的注册。
 *
 */
#define OS_REGISTER_PHASE                                       (0x07)

/**
 * @ingroup SRE_sys
 * 表示系统在进行OS模块初始化阶段，匹配MOUDLE_ID之后，标记进入MODULE_ID的初始化。
 *
 */
#define OS_INITIALIZE_PHASE                                     (0x08)

/**
 * @ingroup SRE_sys
 * 表示系统在进行产品驱动初始化阶段，匹配MOUDLE_ID之后，标记进入MODULE_ID的初始化。
 *
 */
#define OS_DEVDRVINIT_PHASE                                     (0x09)

/**
 * @ingroup SRE_sys
 * 表示系统在进行OS启动阶段，匹配MOUDLE_ID之后，标记进入MODULE_ID的启动。
 *
 */
#define OS_START_PHASE                                          (0x10)

/*
 * 系统模块的错误码定义。
*/

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：指针参数为空。
 *
 * 值: 0x02000001
 *
 * 解决方案: 请检查入参是否为空
 */
#define OS_ERRNO_SYS_PTR_NULL                               SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x01)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：系统主频配置非法。
 *
 * 值: 0x02000002
 *
 * 解决方案: 在sre_config.h中配置合理的系统主频。
 */
#define OS_ERRNO_SYS_CLOCK_INVALID                          SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x02)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：用户的配置选项OS_SYS_CORE_MAX_NUM有误，应该和芯片匹配，且OS_SYS_CORE_MAX_NUM值不能超过该芯片支持的最大核数。
 *
 * 值: 0x02000003
 *
 * 解决方案:修改配置选项OS_SYS_CORE_MAX_NUM，和芯片匹配。
 *
 */
#define OS_ERRNO_SYS_MAXNUMOFCORES_IS_INVALID               SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x03)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：钩子已达到最大允许的注册数。
 *
 * 值: 0x02000005
 *
 * 解决方案:增大允许注册的IDLE钩子数。
 *
 */
#define OS_ERRNO_SYS_HOOK_IS_FULL                           SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x05)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：获取系统时间函数Cortex-AX平台配置为NULL。
 *
 * 值: 0x02000006
 *
 * 解决方案:配置获取系统时间函数时，Cortex-AX平台必须配置为非NULL。
 *
 */
#define OS_ERRNO_SYS_TIME_HOOK_NULL                         SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x06)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码:镜像的个数配置错误。
 *
 * 值: 0x02000007
 *
 * 解决方案: 查看每个镜像配置的镜像个数是否不相等或者镜像个数为0或者大于最大镜像个数。
 *
 */
#define OS_ERRNO_SYS_IMAGE_NUM_INVALID                     SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x07)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码:镜像的镜像ID配置不可用。
 *
 * 值: 0x02000008
 *
 * 解决方案: 查看每个镜像配置的镜像ID是否有重复或者配置值错误。镜像ID值不能大于等于镜像个数。
 *
 */
#define OS_ERRNO_SYS_IMAGE_ID_INVALID                      SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x08)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：用户的配置选项OS_SYS_CORE_MAX_NUM有误，应该和芯片匹配，且OS_SYS_CORE_MAX_NUM值不能超过该芯片支持的最大核数。
 *
 * 值: 0x02000009
 *
 * 解决方案:修改配置选项OS_SYS_CORE_MAX_NUM，和芯片匹配。
 *
 */
#define OS_ERRNO_SYS_CORERUNNUM_IS_INVALID                  SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x09)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：依赖的模块没有打开。
 *
 * 值: 0x0200000a
 *
 * 解决方案:请打开相应的模块。
 *
 */
#define OS_ERRNO_SYS_MODE_CLOSE                             SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x0a)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：配置的系统栈过小。
 *
 * 值: 0x0200000b
 *
 * 解决方案:请增大配置的系统栈大小。
 *
 */
#define OS_ERRNO_SYS_STACK_SIZE_NOT_ENOUGH                  SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x0b)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：任务(栈)监控功能控依赖的功能任务未打开。
 *
 * 值: 0x0200000c
 *
 * 解决方案:检查依赖的任务裁剪开关是否打开。
 *
 */
#define OS_ERRNO_SYS_TASK_CLOSE                             SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x0c)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：任务饿死撑死功能控依赖的功能任务监控未打开。
 *
 * 值: 0x0200000d
 *
 * 解决方案:检查依赖的任务监控裁剪开关是否打开。
 *
 */
#define OS_ERRNO_SYS_TASKMON_CLOSE                          SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x0d)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：任务监功能控依赖的功能TICK未打开。
 *
 * 值: 0x0200000e
 *
 * 解决方案:检查依赖的TICK裁剪开关是否打开。
 *
 */
#define OS_ERRNO_SYS_TICK_CLOSE                             SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x0e)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：CPUP告警功能控依赖的功能CPUP未打开。
 *
 * 值: 0x0200000f
 *
 * 解决方案:检查依赖的CPUP裁剪开关是否打开。
 *
 */
#define OS_ERRNO_SYS_CPUP_CLOSE                             SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x0f)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：配置的用户模式栈过小。
 *
 * 值: 0x0200000e
 *
 * 解决方案:请增大配置的用户模式栈大小。
 *
 */
#define OS_ERRNO_SYS_USER_STACK_SIZE_NOT_ENOUGH             SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x0e)

/**
 * @ingroup SRE_sys
 * 系统基本功能错误码：配置的系统模式不正确。
 *
 * 值: 0x0200000f
 *
 * 解决方案:系统模式必须为super模式或者user模式。
 *
 */
#define OS_ERRNO_SYS_MODE_INVALID                         SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x0f)

/**
 * @ingroup SRE_sys
 * 核号错误
 *
 * 值: 0x02000010
 *
 * 解决方案:核号大于等于#OS_MAX_CORE_NUM
 *
 */
#define OS_ERRNO_SYS_COREID_INVALID                         SRE_ERRNO_OS_ERROR(OS_MID_SYS, 0x10)


/**
 * @ingroup SRE_sys
 * 每秒毫秒数
 */
#define SRE_SYS_MS_PER_SECOND         1000

/**
 * @ingroup SRE_sys
 * 每秒微秒数
 */
#define SRE_SYS_US_PER_SECOND         1000000

/**
 * @ingroup SRE_sys
 * OS版本号
 */
#define OS_SYS_OS_VER_LEN             48

/**
 * @ingroup SRE_sys
 * 产品版本号
 */
#define OS_SYS_APP_VER_LEN            64

/**
 * @ingroup SRE_sys
 * 系统时钟
 */
extern UINT32 g_uwSystemClock;

/**
 * @ingroup SRE_sys
 * 操作系统Tick计数值
 */
extern UINT64 g_ullUniTicks;

/**
 * @ingroup  SRE_sys
 * @brief 转换cycle为毫秒。
 *
 * @par 描述:
 * 转换cycle为毫秒。
 *
 * @attention 无
 *
 * @param  udwCycle    [IN] 类型为#UINT64，cycle数。
 *
 * @retval [0,0xFFFFFFFFFFFFFFFF] 转换后的毫秒数。
 * @par 依赖:
 * <ul><li>SRE_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see SRE_Cycle2US
 */
OS_SEC_ALW_INLINE INLINE UINT64 SRE_Cycle2MS(UINT64 udwCycle)
{
    return (UINT64)((udwCycle/(g_uwSystemClock/SRE_SYS_MS_PER_SECOND)));
}

/**
 * @ingroup  SRE_sys
 * @brief 转换cycle为微秒。
 *
 * @par 描述:
 * 转换cycle为微秒。
 *
 * @attention 无
 *
 * @param  udwCycle    [IN] 类型为#UINT64，cycle数。
 *
 * @retval [0,0xFFFFFFFFFFFFFFFF] 转换后的微秒数。
 * @par 依赖:
 * <ul><li>SRE_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_Cycle2MS
 */
OS_SEC_ALW_INLINE INLINE UINT64 SRE_Cycle2US(UINT64 udwCycle)
{
    return (UINT64)((udwCycle/(g_uwSystemClock/SRE_SYS_US_PER_SECOND)));
}

/**
 * @ingroup  SRE_sys
 * @brief 获取本核的tick计数私有地址。
 *
 * @par 描述:
 * 获取本核的tick计数私有地址。
 *
 * @attention 无
 *
 * @par 依赖:
 * <ul><li>SRE_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see 无
 */
#define SRE_GetTickAddr()   ((UINT32)(UINT32 *)&g_ullUniTicks)

/**
 * @ingroup  SRE_sys
 * @brief 获取当前的tick计数。
 *
 * @par 描述:
 * 获取当前的tick计数。
 *
 * @attention 无
 *
 * @param  无
 *
 * @retval  [0,0xFFFFFFFFFFFFFFFF] 当前的tick计数。
 * @par 依赖:
 * <ul><li>SRE_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_CycleCountGet
 */
extern UINT64 SRE_TickCountGet(VOID);

/**
 * @ingroup SRE_sys
 * @brief 获取每个tick对应的硬件cycle数。
 *
 * @par 描述:
 * 获取每个tick对应的硬件cycle数。
 *
 * @attention 无
 *
 * @param  无
 *
 * @retval [0,0xFFFFFFFF] 每个tick对应的硬件cycle数。
 * @par 依赖:
 * <ul><li>SRE_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see 无
 */
extern UINT32 SRE_CyclePerTickGet(VOID);

/**
 * @ingroup SRE_sys
 * @brief 延迟时间(单位cycle)。
 *
 * @par 描述:
 * 延迟时间(单位cycle)。
 *
 * @attention 无
 *
 * @param  uwCycles    [IN] 类型为#UINT32，延迟cycle数
 *
 * @retval 无
 * @par 依赖:
 * <ul><li>SRE_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see 无
 */
extern VOID SRE_DelayCycle(UINT32 uwCycles);

/**
 * @ingroup SRE_sys
 * @brief 延迟时间(单位微秒)。
 *
 * @par 描述:
 * 延迟时间(单位微秒)。
 *
 * @attention 无
 *
 * @param  uwDelay    [IN] 类型为#UINT32，延迟微秒数。
 *
 * @retval 无
 * @par 依赖:
 * <ul><li>SRE_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_DelayMs
 */
extern VOID SRE_DelayUs(UINT32 uwDelay);

/**
 * @ingroup SRE_sys
 * @brief 延迟时间(单位毫秒)。
 *
 * @par 描述:
 * 延迟时间(单位毫秒)。
 *
 * @attention 无
 *
 * @param  uwDelay    [IN] 类型为#UINT32，延迟毫秒数。
 *
 * @retval 无
 * @par 依赖:
 * <ul><li>SRE_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_DelayUs
 */
extern VOID SRE_DelayMs(UINT32 uwDelay);

/**
 * @ingroup SRE_sys
 * @brief 系统重启钩子函数的类型定义。
 *
 * @par 描述:
 * 用户通过系统重启钩子函数的类型定义系统重启钩子，在系统重启之前调用该钩子。
 *
 * @attention 无。
 *
 * @param  无
 *
 * @retval 无
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see 无。
*/
typedef VOID ( *SYS_REBOOT_FUNC )(VOID);

/**
 * @ingroup SRE_sys
 * @brief IDLE循环钩子的类型定义。
 *
 * @par 描述:
 * 用户通过IDLE循环钩子的函数类型定义函数，系统在进入IDLE循环之前调用该钩子。
 *
 * @attention 无。
 *
 * @param  无
 *
 * @retval 无
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see 无。
*/
typedef VOID (* OS_IDLE_HOOK)(VOID);

/**
 * @ingroup SRE_sys
 * @brief 计算系统绝对时间钩子函数的类型定义。
 *
 * @par 描述:
 * 由于在Cortex-AX平台OS没有接管硬件定时器，需要用户提供计算系统时间的钩子函数。
 *
 * @attention 获取的是64bit cycles 数据。
 *
 * @param  无
 *
 * @retval 系统绝对时间
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see 无。
*/
typedef UINT64 ( *SYS_TIME_FUNC )(VOID);

/**
 * @ingroup SRE_sys
 * 系统模块配置信息的结构体定义。
 *
 * 保存系统模块的配置项信息。
 */
typedef struct tagSysModInfo
{
    UINT32 uwSystemClock;                            /**< CPU主频，时钟周期              */
    UINT8  ucCoreID;                                 /**< 当前核ID                       */
    UINT8  ucCoreRunNum;                             /**< 实际运行的核数                 */
    UINT8  ucCoreMaxNum;                             /**< 最大支持的核数                 */
    UINT8  ucCoreMaster;                             /**< 主核ID                         */
    SYS_REBOOT_FUNC pfnRebootHook;                   /**< 用户注册的单板复位函数         */
    OS_IDLE_HOOK pfnIdleHook;                        /**< 用户注册的IDLE钩子函数         */
    SYS_TIME_FUNC pfnSysTimeHook;                    /**< 用户注册的获取系统时间函数     */
    UINT8  ucCpuType;                                /**< CPU type                       */
    UINT8  ucReserve;                                /**< 补齐预留                       */
    UINT16 usIdleHookMaxNum;                         /**< 用户注册最大IDLE任务钩子数     */
    UINT16 usImageNum;                               /**< 最大镜像个数，所有镜像要一致   */
    UINT16 usImageId;                                /**< 当前镜像ID                     */
} SYS_MOD_INFO_S;

#if (OS_CPU_TYPE == OS_SD6182 || OS_CPU_TYPE == OS_SD6157)
/**
 * @ingroup SRE_sys
 * 系统栈配置信息的结构体定义。
 *
 * 保存系统模块的配置项信息。
 */
typedef struct tagSysStackInfo
{
    UINT32 uwSystemStackAddr;                        /**< 系统栈起始地址                 */
    UINT32 uwSystemStackSize;                        /**< 系统栈大小                     */
} SYS_STACK_INFO_S;
#endif

#if (OS_CPU_TYPE == OS_SD6183)
/**
 * @ingroup SRE_sys
 * 系统栈配置信息的结构体定义。
 *
 * 保存系统模块的配置项信息。
 */
typedef struct tagUserModeStackInfo
{
    UINT32 uwUserModeStackAddr;                        /**< 用户模式栈起始地址           */
    UINT32 uwUserModeStackSize;                        /**< 用户模式栈大小               */
} SYS_USER_MODE_STACK_INFO_S;
#endif

/**
 * @ingroup SRE_sys
 * 系统模块线程类型获取枚举结构定义
 *
 * 系统模块线程类型获取
 */
typedef enum
{
    SYS_HWI,       /**<当前线程类型属于硬中断*/
    SYS_SWI,       /**<当前线程类型属于软中断*/
    SYS_TASK,      /**<当前线程类型属于任务*/
    SYS_KERNEL,    /**<当前线程类型属于内核*/
    SYS_BUTT
}OS_THREAD_TYPE_E;

/**
 * @ingroup SRE_sys
 * @brief 复位单板。
 *
 * @par 描述:
 * 复位单板，程序重新加载执行。
 *
 * @attention
 * <ul>
 * <li>并非直接复位单板，而是关中断，等待看门狗复位。</li>
 * <li>用户可以通过配置项OS_SYS_REBOOT_HOOK挂接复位函数。</li>
 * </ul>
 *
 * @param  无
 *
 * @retval 无
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see 无
 */
extern VOID SRE_Reboot(VOID);

/**
 * @ingroup SRE_sys
 * 系统初始化阶段信息解析结构体定义。
 *
 * 用于标记当前系统运行到哪个阶段,uwInitPhase = OS_INIT_PHASE | MODULE_ID;
 * OS_INIT_PHASE表示系统初始化阶段状态bit[31-16]; MODULE_ID表示当前系统初始化的模块号bit[0-15]，具体模块号在sre_module.h中有定义。
 * 记录系统模块是否完成初始化，0-63位用来记录已进行初始化的所有模块的模块号，若某个模块进行了初始化操作，将其模块号对应的位置为1。
 * auwInitTrace[0] : 标示模块号为0~31，auwInitTrace[1] : 标示模块号为32~63,跟踪的是当前系统中各模块的初始化完成情况以及系统初始化
 * 化阶段状态的实时值。
 */
typedef struct tagSysTraceInfo
{
    UINT32 uwInitMagicWord;         /**< 初始化阶段供CDA解析信息的魔术字                                                                    */
    UINT32 uwInitPhase;             /**< 当前核初始化阶段状态标识用于标记当前系统运行到哪个阶段,g_auwInitPhase = OS_INIT_PHASE | MODULE_ID;
                                         OS_INIT_PHASE表示系统初始化阶段状态bit[31-16]，MODULE_ID表示当前系统初始化的模块号bit[0-15]。      */
    UINT32 auwInitTrace[2];         /**< 当前核初始化的所有模块进行跟踪标识记录系统模块是否完成初始化，[0]bit0-31,[1]bit32~63用来记录
                                         已进行初始化的所有模块的模块号，若某个模块进行了初始化操作，将其模块号对应位设置1。                */
    UINT32 uwErrNo;                 /**< 初始化阶段错误码返回值记录                                                                         */
}SYS_INIT_TRACE_INFO_S;

/**
 * @ingroup SRE_sys
 * 系统初始化Trace信息。
 *
 * 系统初始化Trace信息。
 */
extern SYS_INIT_TRACE_INFO_S  stSysInitTraceInfo;

/**
 * @ingroup SRE_sys
 * 当前核ID。
 *
 * 核ID为硬件寄存器中的ID号。
 */
extern UINT8 g_ucLocalCoreID;

/**
 * @ingroup  SRE_sys
 * @brief 获取当前核ID。
 *
 * @par 描述:
 * 获取当前核ID。
 *
 * @attention
 * <ul>
 * <li>获取的核ID为硬件寄存器中的ID号。</li>
 * </ul>
 *
 * @param  无
 *
 * @retval 物理核ID。
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_GetNumberOfCores | SRE_GetMasterCore
 */
#define SRE_GetCoreID()          ((UINT8)(g_ucLocalCoreID + (UINT8)0))

/**
 * @ingroup SRE_sys
 * 主核ID。
 *
 * 核ID为硬件寄存器中的ID号。
 */
extern UINT8 g_ucMasterCoreID;

/**
 * @ingroup  SRE_sys
 * @brief 获取主核ID。
 *
 * @par 描述:
 * 获取主核ID。
 *
 * @attention
 * <ul>
 * <li>获取的核ID为硬件寄存器中的ID号。</li>
 * </ul>
 *
 * @param  无
 *
 * @retval 物理核ID。
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_GetNumberOfCores | SRE_GetCoreID
 */
#define SRE_GetMasterCore()      ((UINT8)(g_ucMasterCoreID + (UINT8)0))

 /**
 * @ingroup SRE_sys
 * 实际运行的核数。
 *
 * 实际运行的核数，而不是RTOSck最大支持的核数。
 */
extern UINT8 g_ucNumOfCores;

 /**
 * @ingroup SRE_sys
 * 最大可支持运行的核数。
 *
 * 为RTOSck最大支持的核数，而不是RTOSck实际运行的核数。
 */
extern UINT8 g_ucMaxNumOfCores;
/**
 * @ingroup  SRE_sys
 * @brief 获取实际运行的核数。
 *
 * @par 描述:
 * 获取实际运行的核数。
 *
 * @attention
 * <ul>
 * <li>获取的为实际运行的核数，而不是OS最大支持的核数。</li>
 * </ul>
 *
 * @param  无
 *
 * @retval 核数。
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since  RTOSck V100R001C01
 * @see SRE_GetCoreID | SRE_GetMasterCore
 */
#define SRE_GetNumberOfCores()       ((UINT8)(g_ucNumOfCores + (UINT8)0))

/**
 * @ingroup  SRE_sys
 * @brief 获取最大可支持运行的核数。
 *
 * @par 描述:
 * 获取最大可支持运行的核数。
 *
 * @attention
 * <ul>
 * <li>获取的为OS最大支持的核数，而不是OS实际运行的核数。</li>
 * </ul>
 *
 * @param  无
 *
 * @retval 核数。
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since  RTOSck V100R001C01
 * @see SRE_GetCoreID | SRE_GetMaxNumberOfCores
 */
#define SRE_GetMaxNumberOfCores()     ((UINT8)(g_ucMaxNumOfCores + (UINT8)0))

/**
 * @ingroup  SRE_sys
 * @brief 获取系统的主频。
 *
 * @par 描述:
 * 获取系统的主频。
 *
 * @attention
 *
 * @param
 *
 * @retval 系统主频
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_GetCoreID
 */
extern UINT32 SRE_GetSysClk(VOID);

/**
 * @ingroup  SRE_sys
 * @brief 获取OS的版本号。
 *
 * @par 描述:
 * 获取OS的版本号。版本编号格式为RTOSck Vx00R00xCxx SVNID(XXX)。
 *
 * @attention 无
 *
 * @param  无
 *
 * @retval 指向OS版本号的字符串指针。 如：RTOSck V100R001C01 SVNID (XXX) 。
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_SetAppVersion | SRE_GetAppVersion
 */
extern CHAR * SRE_GetOsVersion(VOID);

/**
 * @ingroup  SRE_sys
 * @brief 注册应用程序的版本号。
 *
 * @par 描述:
 * 注册应用程序的版本号。
 *
 * @attention
 * <ul>
 * <li>系统APP版本号保存在全局数组中，可以设置的APP版本号最大长度为64字节(包括结束字符)。</li>
 * </ul>
 *
 * @param  pcAppVersion [IN] 类型#CHAR *，指向应用程序版本号的字符串指针。
 *
 * @retval #OS_ERRNO_SYS_PTR_NULL           0x02000001，指针参数为空。
 * @retval #SRE_OK                          0x02000000，注册APP版本号成功。
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_GetAppVersion | SRE_GetOsVersion
 */
extern UINT32 SRE_SetAppVersion(CHAR *pcAppVersion);

/**
 * @ingroup  SRE_sys
 * @brief 获取应用程序的版本号。
 *
 * @par 描述:
 * 获取应用程序的版本号。
 *
 * @attention 若之前未注册，则返回指向"Unknown"的指针。
 *
 * @param  无
 *
 * @retval 指向应用程序版本号的字符串指针。
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_SetAppVersion | SRE_GetOsVersion
 */
extern CHAR * SRE_GetAppVersion(VOID);

/**
 * @ingroup  SRE_sys
 * @brief 将cycle值转换成毫秒。
 *
 * @par 描述:
 * 将cycle值转换成毫秒。
 *
 * @attention
 * <ul>
 * <li>当系统主频不为1000整数倍时，该接口获取到的ms数有误差。</li>
 * </ul>
 *
 * @param  pstCpuTick [IN] 类型#CPU_TICK *，待转换的cycle计数地址。
 * @param  puwMsHi [OUT] 类型#UINT32 *，保存毫秒的高32位的地址。
 * @param  puwMsLo [OUT] 类型#UINT32 *，保存毫秒的低32位的地址。
 *
 * @retval #OS_ERRNO_SYS_PTR_NULL           0x02000001，指针参数为空。
 * @retval #SRE_OK                          0，操作成功。
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_CpuTick2US
 */
extern UINT32 SRE_CpuTick2MS(CPU_TICK *pstCpuTick, UINT32 *puwMsHi, UINT32 *puwMsLo);

/**
 * @ingroup  SRE_sys
 * @brief 将cycle值转换成微秒。
 *
 * @par 描述:
 * 将cycle值转换成微秒。
 *
 * @attention 无
 *
 * @param  pstCpuTick [IN] 类型#CPU_TICK *，待转换的cycle计数地址。
 * @param  puwUsHi [OUT] 类型#UINT32 *，保存微秒的高32位的地址。
 * @param  puwUsLo [OUT] 类型#UINT32 *，保存微秒的低32位的地址。
 *
 * @retval #OS_ERRNO_SYS_PTR_NULL           0x02000001，指针参数为空。
 * @retval #SRE_OK                          0x00000000，操作成功。
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_CpuTick2MS
 */
extern UINT32 SRE_CpuTick2US(CPU_TICK *pstCpuTick, UINT32 *puwUsHi, UINT32 *puwUsLo);

/**
 * @ingroup  SRE_sys
 * @brief 获取系统当前线程类型。
 *
 * @par 描述:
 * 获取系统当前线程类型。
 *
 * @attention 无
 *
 * @param  无。
 *
 * @retval #OS_THREAD_TYPE_E                    线程类型。
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see 无
 */
extern OS_THREAD_TYPE_E  SRE_CurThreadType(VOID);

/**
 * @ingroup  SRE_sys
 * @brief 注册IDLE循环进入前调用的钩子。
 *
 * @par 描述:
 * 为本核注册IDLE循环进入前调用的钩子，该钩子只会被调用一次。
 *
 * @attention
 * <ul>
 * <li>注册的钩子只在进入IDLE循环前执行一次。</li>
 * <li>若任务未裁剪，则作用的是任务IDLE循环；若任务被裁剪，则作用的是软中断IDLE循环。</li>
 * <li>IDLE任务钩子中使用矢量寄存器需要在前置钩子中调用#SRE_TaskCpSaveCfg接口设置矢量操作上下文保护区。</li>
 * <li>IDLE软中断钩子中使用矢量寄存器需要在前置钩子中调用#SRE_SwiCpSaveCfg接口设置矢量操作上下文保护区。</li>
 * </ul>
 *
 * @param  pfnHook    [IN] 类型为#OS_IDLE_HOOK，钩子函数。
 *
 * @retval #OS_ERRNO_SYS_PTR_NULL           0x02000001，指针参数为空。
 * @retval #SRE_OK                          0x00000000，操作成功。
 *
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R001C01
 * @see SRE_IdleHookAdd
 */
extern UINT32 SRE_IdlePrefixHookAdd(OS_IDLE_HOOK pfnHook);

/**
 * @ingroup  SRE_sys
 * @brief 注册IDLE循环中调用的钩子
 *
 * @par 描述:
 * 注册在IDLE任务或IDLE软中断循环中调用的钩子函数。
 *
 * @attention
 * <ul>
 * <li>钩子中不能调用引起任务阻塞或切换的函数。</li>
 * </ul>
 *
 * @param  pfnHook [OUT] 类型#OS_IDLE_HOOK，IDLE钩子函数，该参数不能为空。
 *
 * @retval #OS_ERRNO_HOOK_TYPE_INVALID      0x02001600，HOOK类型错误。
 * @retval #OS_ERRNO_HOOK_PTR_NULL          0x02001602，HOOK指针空。
 * @retval #SRE_OK                          0x00000000，操作成功。
 *
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see SRE_IdleHookDel
 */
#define SRE_IdleHookAdd(pfnHook) SRE_HookAdd(OS_HOOK_IDLE_PERIOD, (OS_VOID_FUNC)(OS_IDLE_HOOK)pfnHook)

/**
 * @ingroup  SRE_sys
 * @brief 删除IDLE循环中调用的钩子
 *
 * @par 描述:
 * 删除在IDLE任务或IDLE软中断循环中调用的钩子函数。
 *
 * @attention
 *<ul>
 * 无
 * </ul>
 *
 * @param  pfnHook [OUT] 类型#OS_IDLE_HOOK，IDLE钩子函数，该参数不能为空。
 *
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see SRE_IdleHookAdd
 */
#define SRE_IdleHookDel(pfnHook) SRE_HookDel(OS_HOOK_IDLE_PERIOD, (OS_VOID_FUNC)(OS_IDLE_HOOK)pfnHook)

#if (OS_OPTION_INIT_PHASE_QUERY == YES)
/**
 * @ingroup SRE_sys
 * 初始化的阶段
 */
typedef enum
{
    PHASE_IN_PRE_STARTUP = 0,   /**<当前OS在预启动阶段,状态未赋值前     */   
    PHASE_IN_MAIN_ENTRY,        /**<当前OS在进入MAIN的阶段              */
    PHASE_IN_EXC_INIT_END,      /**<当前OS异常可全量接管阶段            */
    PHASE_IN_OS_START,           /**<当前OS完成所有初始化，准备切入任务  */
    PHASE_IN_BUTT               /**<非法阶段                            */ 
} INIT_PHASE_E;

/**
 * @ingroup  SRE_sys
 * @brief 实例间初始化阶段获取
 *
 * @par 描述:
 * 根据输入的核号获取对应核的初始化阶段
 *
 * @attention
 * <ul>
 * <li>只支持SD6182/SD6157平台。</li>
 * <li>用户必须在进入main之后调用该函数。</li>
 * </ul>
 *
 * @param  uwCoreID   [IN]  类型#UINT32，核号。
 * @param  penPhase   [OUT] 类型#INIT_PHASE_E，初始化阶段。
 *
 * @retval #OS_ERRNO_SYS_COREID_INVALID          0x02000010，核号错误。
 * @retval #OS_ERRNO_SYS_PTR_NULL                0x02000001，指针为空。
 * @retval #SRE_OK                               0x00000000，成功。
 * @par 依赖:
 * <ul><li>sre_hw.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R003C00
 * @see SRE_InitPhaseClr
 **/
extern UINT32 SRE_InitPhaseGet(UINT32 uwCoreID, INIT_PHASE_E *penPhase);

/**
 * @ingroup  SRE_sys
 * @brief 实例间初始化阶段清除
 *
 * @par 描述:
 * 根据输入的核号，清除对应核的阶段。
 *
 * @attention
 * <ul>
 * <li>只支持SD6182/SD6157平台。</li>
 * <li>用户必须在进入main之后调用该函数。</li>
 * <li>用户须在复位从核前调用此接口，清除对应核的初始化阶段。</li>
 * </ul>
 *
 * @param  uwCoreID   [IN]  类型#UINT32，核号。
 *
 * @retval #OS_ERRNO_SYS_COREID_INVALID          0x02000010，核号错误。
 * @retval #SRE_OK                               0x00000000，成功。
 * @par 依赖:
 * <ul><li>sre_hw.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R003C00
 * @see SRE_InitPhaseGet
 **/
extern UINT32 SRE_InitPhaseClr(UINT32 uwCoreID);
#endif

#if (OS_CPU_TYPE == OS_SD6183)
/**
 * @ingroup  SRE_sys
 * @brief 获取当前的Context ID
 *
 * @par 描述:
 * 获取当前的Context ID。
 *
 * @attention
 * <ul>
 * </ul>
 *
 * @retval #任意值，当前的context ID。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see
 */
extern UINT32 SRE_GetCurCntxId();

typedef enum
{
    OS_SUPER_MODE         ,
    OS_USER_MODE
}OS_SYS_MODE;

/**
 * @ingroup  SRE_sys
 * @brief Context ID切换
 *
 * @par 描述:
 * Context ID切换。
 *
 * @attention
 * <ul>
 * </ul>
 *
 * @param  enMode    [IN] 类型为#OS_SYS_MODE，系统模式。
 * @param  ucCntxID  [IN] 类型为#UINT8，Context ID的值。
 *
 * @retval #SRE_OK                          0x00000000，操作成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see
 */
extern UINT32 SRE_CntxIdSwitch(OS_SYS_MODE enMode, UINT8 ucCntxID);


/**
 * @ingroup SRE_sys
 * @brief HIDSP模式切换钩子的类型定义。
 *
 * @par 描述:
 * 用户通过HIDSP模式切换钩子的函数类型定义函数，系统在进行模式切换时调用该钩子。
 *
 * @attention 无。
 *
 * @param  无
 *
 * @retval 无
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R003C00
 * @see 无。
*/
typedef VOID (* OS_MODE_SWITCH_HOOK)(VOID);

/**
 * @ingroup  SRE_sys
 * @brief HIDSP核进入用户模式。
 *
 * @par 描述:
 * 通过掉用该接口，把当前的系统模式由Supervisor模式切换到用户模式。
 *
 * @attention 无
 *
 * @param  无。
 *
 * @retval VOID
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R003C00
 * @see 无
 */
extern VOID  SRE_UserModeEntry(VOID);

/**
 * @ingroup  SRE_sys
 * @brief HIDSP核退出用户模式。
 *
 * @par 描述:
 * 通过掉用该接口，把当前的系统模式由用户模式切换Supervisor模式。
 *
 * @attention 无
 *
 * @param  无。
 *
 * @retval VOID
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R003C00
 * @see 无
 */
extern VOID  SRE_UserModeExit(VOID);

/**
 * @ingroup  SRE_sys
 * @brief 系统模式切换钩子注册。
 *
 * @par 描述:
 * 注册用户钩子，在用户模式和系统模式切换时调用。
 *
 * @attention 注册的钩子默认关中断，不能在钩子内开中断。
 *
 * @param  无。
 *
 * @retval #UINT32     错误码。
 * @par 依赖:
 * <ul><li>sre_sys.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R003C00
 * @see 无
 */
extern UINT32  SRE_SysModeSwitchHookReg(OS_MODE_SWITCH_HOOK pfnHook);
#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _SRE_SYS_H */

/**
 * History:
 * 2008-02-20 l66919: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */

