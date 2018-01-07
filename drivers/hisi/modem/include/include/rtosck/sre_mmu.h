/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWrXNs8MZYk9cpFfgXfKX0g1VVtQNxHnHCKsK8caFMiW8LvCCkwIW
Tc6iJlHGFi3nUeJwZnaqeEUBwq/bzU5cp8JMIZQR60CPwhsVGkig7D46L1mTcjaBXLwtoHt/
6v3Dai4TlBVBFwOWTchUiOJxeE2wLdYyzQH7bxMqpVN8PSJBz2SkZe8/R3vqPuDVSL3eRcCf
iuACuPF273ydsJiweg0V5uYDiA/YMCCCeRuu/gYxAlc9blR458CAxlyMKocqZw==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/**
 * @file sre_mmu.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * 描述：mmu模块的对外头文件。 \n
 */
 /**@defgroup SRE_mmu MMU
  *@ingroup SRE_kernel
 */

#ifndef _SRE_MMU_H
#define _SRE_MMU_H

#include "sre_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#define MMU_VA_T VOID*
#define MMU_PA_T VOID*

/**
 * @ingroup SRE_mmu
 * 错误码：虚拟地址、物理地址或长度非page对齐。
 *
 * 值: 0x02001201
 *
 * 解决方案: 检查入参虚拟地址、物理地址、长度是否page对齐。
 */
#define OS_ERRNO_MMU_NOT_PAGE_ALIGNED                            SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x1)

/**
 * @ingroup SRE_mmu
 * 错误码：长度非section对齐。
 *
 * 值: 0x02001202
 *
 * 解决方案: 检查长度是否section对齐。
 */
#define OS_ERRNO_MMU_NOT_SECT_ALIGNED                            SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x2)

/**
 * @ingroup SRE_mmu
 * 错误码：未申请到一级页表内存。
 *
 * 值: 0x02001203
 *
 * 解决方案: 配置足够大的内存分区。
 */
#define OS_ERRNO_MMU_NO_LEVEL1_MEMORY                            SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x3)

/**
 * @ingroup SRE_mmu
 * 错误码：映射虚拟地址空间已经映射或者部分已经映射。
 *
 * 值: 0x02001204
 *
 * 解决方案: 检查所映射的虚拟地址空间是否已经映射或部分已经映射。
 */
#define OS_ERRNO_MMU_ADDR_MAPPED                                 SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x4)

/**
 * @ingroup SRE_mmu
 * 错误码：未申请到二级页表内存。
 *
 * 值: 0x02001205
 *
 * 解决方案: 配置足够大的内存分区。
 */
#define OS_ERRNO_MMU_NO_LEVEL2_MEMORY                            SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x5)

/**
 * @ingroup SRE_mmu
 * 错误码：未申请到页表映射信息内存。
 *
 * 值: 0x02001206
 *
 * 解决方案: 配置足够大的内存分区。
 */
#define OS_ERRNO_MMU_NO_MAP_INFO_MEMORY                          SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x6)

/**
 * @ingroup SRE_mmu
 * 错误码：映射或内存属性设置地址溢出。
 *
 * 值: 0x02001209
 *
 * 解决方案: 检查入参地址和长度是否合理。
 */
#define OS_ERRNO_MMU_ADDR_OVERFLOW                               SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x09)

/**
 * @ingroup SRE_mmu
 * 错误码：虚拟地址未映射。
 *
 * 值: 0x02001210
 *
 * 解决方案: 检查虚拟地址是否映射或MMU初始化地址是否包含内存分区所有地址。
 */
#define OS_ERRNO_MMU_ADDR_NOT_MAPPED                             SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x10)

/**
 * @ingroup SRE_mmu
 * 错误码：页表拆分时内存空间不足。
 *
 * 值: 0x02001211
 *
 * 解决方案: 配置空间足够的0号分区。
 */
#define OS_ERRNO_MMU_DEOPTIMIZE_FAIL                             SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x11)

/**
 * @ingroup SRE_mmu
 * 错误码：ATTRIBUTE属性配错。
 *
 * 值: 0x02001217
 *
 * 解决方案: 检查ATTRIBUTE属性是否为属性宏的一种。
 */
#define OS_ERRNO_MMU_ATTRIBUTE_ERR                               SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x17)

/**
 * @ingroup SRE_mmu
 * 错误码：ACCESS属性配错。
 *
 * 值: 0x02001218
 *
 * 解决方案: 检查写属性是否或上读属性，检查可执行属性是否或上读属性。
 */
#define OS_ERRNO_MMU_ACCESS_ERR                                  SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x18)

/**
 * @ingroup SRE_mmu
 * 错误码：入参错误。
 *
 * 值: 0x0200121b
 *
 * 解决方案: 检查入参。
 */
#define OS_ERRNO_MMU_PARAM_INVALID                              SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x1b)

/**
 * @ingroup  SRE_mmu
 * @brief 虚地址转实地址。
 *
 * @par 描述:
 * 虚地址转实地址。
 *
 * @attention
 * <ul>
 * <li>83平台若入参为未映射地址，会产生异常。</li>
 * <li>AX平台若入参为未映射地址，会返回错误码，查询到的物理地址为虚拟地址本身。</li>
 * <li>AX平台只允许MMU模块初始化完成后调用。</li>
 * </ul>
 *
 * @param  pVAddr  [IN] 类型#MMU_VA_T，虚拟地址。
 * @param  ppPAddr  [OUT] 类型#MMU_PA_T，转换后的物理地址。
 *
 * @retval #OS_ERRNO_MMU_PARAM_INVALID       0x0200121b，入参为空。
 * @retval #OS_ERRNO_MMU_ADDR_NOT_MAPPED     0x02001210，地址未映射。
 * @retval #SRE_OK                           0x00000000，转换成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R003C00
 * @see
 */
extern UINT32 SRE_MmuVA2PA(MMU_VA_T pVAddr, MMU_PA_T *ppPAddr);

/**
 * @ingroup  SRE_mmu
 * @brief 实地址转虚地址。
 *
 * @par 描述:
 * 实地址转虚地址。
 *
 * @attention
 * <ul>
 * <li>AX平台只允许MMU模块初始化完成后MMU模式下调用。</li>
 * <li>AX平台若入参为未映射地址，会返回错误码，查询到的虚拟地址为物理地址本身。</li>
 * </ul>
 *
 * @param  pPAddr  [IN] 类型#MMU_PA_T，物理地址。
 * @param  ppVAddr [OUT] 类型#MMU_VA_T，转换后的虚拟地址。
 *
 * @retval #OS_ERRNO_MMU_PARAM_INVALID       0x0200121b，入参为空。
 * @retval #OS_ERRNO_MMU_ADDR_NOT_MAPPED     0x02001210，地址未映射。
 * @retval #SRE_OK                           0x00000000，转换成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R003C00
 * @see
 */
extern UINT32 SRE_MmuPA2VA(MMU_PA_T pPAddr, MMU_VA_T *ppVAddr);

/**
 * @ingroup  SRE_mmu
 * @brief 映射虚拟地址到物理地址。
 *
 * @par 描述:
 * 根据传入参数pVStart、pPStart、uwLen进行虚拟、物理地址映射，并按照uwAttr设置内存属性。
 *
 * @attention
 * <ul>
 * <li>uwLen为0时不进行地址映射。</li>
 * <li>AX平台只允许MMU模块初始化完成后调用。</li>
 * <li>AX平台代码段和分配给OS使用的内存区映射方式必须虚拟地址和物理地址一致。</li>
 * <li>AX平台地址映射时，从uwVirtAddr开始对page逐个进行映射。某一页映射不成功时，已经映射成功的地址保持映射关系。</li>
 * <li>83平台地址映射时，优先映射成大页。</li>
 * </ul>
 *
 * @param  pVStart       [IN] 类型#MMU_VA_T，映射虚拟地址起始地址。
 * @param  pPStart       [IN] 类型#MMU_PA_T，映射物理地址起始地址。
 * @param  uwLen         [IN] 类型#UINT32，映射的内存大小。
 * @param  uwAttr        [IN] 类型#UINT32，内存属性，83平台uwAttr有效bit位为(bit7-0)，bit2-0:read,write,execute; bit3:shareable; bit4:speculative; bit7-5:cache attributes;
 *
 * @retval #OS_ERRNO_MMU_NOT_PAGE_ALIGNED       0x02001201，传入参数虚拟地址、物理地址或长度非page对齐。
 * @retval #OS_ERRNO_MMU_NOT_SECT_ALIGNED       0x02001202，传入参数长度非section对齐。
 * @retval #OS_ERRNO_MMU_NO_LEVEL1_MEMORY       0x02001203，未申请到一级页表内存。
 * @retval #OS_ERRNO_MMU_ADDR_MAPPED            0x02001204，传入虚拟地址已经映射过。
 * @retval #OS_ERRNO_MMU_NO_LEVEL2_MEMORY       0x02001205，未申请到二级页表内存。
 * @retval #OS_ERRNO_MMU_NO_MAP_INFO_MEMORY     0x02001206，未申请到页表映射信息内存。
 * @retval #OS_ERRNO_MMU_ADDR_OVERFLOW          0x02001209，地址溢出。
 * @retval #OS_ERRNO_MEM_FREE_SH_DAMAGED        0x02000311，内存释放时要释放的内存块的头被破坏，或已释放。
 * @retval #OS_ERRNO_MMU_ATTRIBUTE_ERR          0x02001217，入参uwState的ATTRIBUTE属性配置不正确。
 * @retval #OS_ERRNO_MMU_ACCESS_ERR             0x02001218，入参uwState的ACCESS属性配置不正确。
 * @retval #SRE_OK                              0x00000000，虚拟、物理地址映射成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R003C00
 * @see NA
 */
extern UINT32 SRE_MmuAssign(MMU_VA_T pVStart, MMU_PA_T pPStart,
                                  UINT32 uwLen, UINT32 uwAttr);

#if(OS_HARDWARE_PLATFORM == OS_CORTEX_AX)
/**
 * @ingroup SRE_mmu
 * MMU内存属性-可读。
 */
#define OS_MMU_STATE_READ                                      0x1

/**
 * @ingroup SRE_mmu
 * MMU内存属性-可写。
 *
 * 该属性宏配置时需要或上OS_MMU_STATE_READ属性
 */
#define OS_MMU_STATE_WRITE                                     0x2

/**
 * @ingroup SRE_mmu
 * MMU内存属性-可执行。
 *
 * 该属性宏配置时需要或上OS_MMU_STATE_READ属性
 */
#define OS_MMU_STATE_EXE                                       0x4

/**
 * @ingroup SRE_mmu
 * MMU内存属性-strong ordered。
 */
#define OS_MMU_STATE_STRONGLY_ORDERED                          0x8

/**
 * @ingroup SRE_mmu
 * MMU内存属性-shared device。
 */
#define OS_MMU_STATE_SHARED_DEVICE                             0x10

/**
 * @ingroup SRE_mmu
 * MMU内存属性-device。
 */
#define OS_MMU_STATE_DEVICE                                    0x20

/**
 * @ingroup SRE_mmu
 * MMU内存属性-不可cache。
 */
#define OS_MMU_STATE_CACHE_OFF                                 0x40

/**
 * @ingroup SRE_mmu
 * MMU内存属性-cache wbwa。
 */
#define OS_MMU_STATE_CACHE_WBWA                                0x80

/**
 * @ingroup SRE_mmu
 * MMU内存属性-cache wbnwa。
 */
#define OS_MMU_STATE_CACHE_WBNWA                               0x100

/**
 * @ingroup SRE_mmu
 * MMU内存属性-cache wtnwa。
 */
#define OS_MMU_STATE_CACHE_WTNWA                               0x200

/**
 * @ingroup SRE_mmu
 * MMU内存属性-shareable。
 */
#define OS_MMU_STATE_SHARE                                     0x400

/**
 * @ingroup SRE_mmu
 * MMU内存属性掩码-ACCESS。
 */
#define OS_MMU_STATEMASK_ACCESS                                (OS_MMU_STATE_READ | OS_MMU_STATE_WRITE | OS_MMU_STATE_EXE)

/**
 * @ingroup SRE_mmu
 * MMU内存属性掩码-Normal。
 */
#define OS_MMU_STATEMASK_NORMAL                                (OS_MMU_STATE_CACHE_OFF | OS_MMU_STATE_CACHE_WBWA | OS_MMU_STATE_CACHE_WBNWA | OS_MMU_STATE_CACHE_WTNWA)

/**
 * @ingroup SRE_mmu
 * MMU内存属性掩码-ATTRIBUTES。
 */
#define OS_MMU_STATEMASK_ATTRIBUTES                            (OS_MMU_STATEMASK_NORMAL | OS_MMU_STATE_STRONGLY_ORDERED | OS_MMU_STATE_SHARED_DEVICE | OS_MMU_STATE_DEVICE)

/**
 * @ingroup SRE_mmu
 * MMU内存属性掩码-SHARE。
 */
#define OS_MMU_STATEMASK_SHARE                                 OS_MMU_STATE_SHARE

/**
 * @ingroup SRE_mmu
 * 错误码：MMU模块初始化内存映射信息重复。
 *
 * 值: 0x02001207
 *
 * 解决方案: 检查MMU初始化内存映射信息是否重复。
 */
#define OS_ERRNO_MMU_INIT_ADDR_OVERLAPPED                        SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x7)

/**
 * @ingroup SRE_mmu
 * 错误码：长度不足合并为大页表。
 *
 * 值: 0x02001212
 *
 * 解决方案: 检查入参虚拟地址和长度是否正确。
 */
#define OS_ERRNO_MMU_OPTIMIZE_NO_NEED                            SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x12)

/**
 * @ingroup SRE_mmu
 * 错误码：0号分区映射的虚拟地址和物理地址不一致。
 *
 * 值: 0x02001214
 *
 * 解决方案: 检查初始化时内存分区地址虚拟地址和物理地址是否一致。
 */
#define OS_ERRNO_MMU_MEM_PT_NOT_FLAT                             SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x14)

/**
 * @ingroup SRE_mmu
 * 错误码：初始化映射信息未包含整个0号分区信息。
 *
 * 值: 0x02001215
 *
 * 解决方案: 检查初始化时内存分区地址是否均包含在映射信息中。
 */
#define OS_ERRNO_MMU_MEM_PT_NOT_INCLUDE                          SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x15)

/**
 * @ingroup SRE_mmu
 * 错误码：初始化映射信息0号分区内存属性不一致。
 *
 * 值: 0x02001216
 *
 * 解决方案: 检查初始化时内存分区属性是否一致。
 */
#define OS_ERRNO_MMU_MEM_PT_STATE_DIFFERENT                      SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x16)

/**
 * @ingroup SRE_mmu
 * 错误码：初始化映射信息0号分区属性未配置成可cache。
 *
 * 值: 0x02001219
 *
 * 解决方案: 检查初始化时内存分区地址属性是否配成wbwa或wbnwa。
 */
#define OS_ERRNO_MMU_MEM_PT_STATE_ERR                            SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x19)

/**
 * @ingroup SRE_mmu
 * 错误码：初始化映射信息未配置或映射数组起始地址、元素个数配置成0。
 *
 * 值: 0x0200121a
 *
 * 解决方案: 检查config配置项中MMU初始化映射信息是否配置，检查映射数组起始地址、元素个数是否配置为0。
 */
#define OS_ERRNO_MMU_INIT_INFO_NOT_CONFIG                        SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x1a)

/**
 * @ingroup SRE_mmu
 * mmu用户配置信息的结构体定义。
 *
 * mmu用户配置信息。
 */
typedef struct tagVirtPhysMem
{
    UINT32 uwVirtualAddr;                                   /**< 虚拟地址 */
    UINT32 uwPhysicalAddr;                                  /**< 物理地址 */
    UINT32 uwLen;                                           /**< 长度     */
    UINT32 uwInitialStateMask;                              /**< 状态掩码 */
    UINT32 uwInitialState;                                  /**< 状态     */
} VIRT_PHYS_MEM_S;

/**
 * @ingroup SRE_mmu
 * MMU模块配置信息的结构体定义。
 *
 * 保存MMU模块的配置项信息。
 */
typedef struct tagMmuModInfo
{
    UINT32 uwMmuInitAddr;                                   /**< MMU初始化结构体数组首地址       */
    UINT32 uwMmuInitNum ;                                   /**< MMU初始化结构体数组元素个数     */
} MMU_MOD_INFO_S;

/**
 * @ingroup  SRE_mmu
 * @brief 内存属性设置。
 *
 * @par 描述:
 * 设置虚拟地址uwVirtAddr，长度uwLen的内存属性为uwState。
 *
 * @attention
 * <ul>
 * <li>该操作仅在Cortex-AX平台下支持。</li>
 * <li>只允许MMU模块初始化完成后调用。</li>
 * <li>不允许设置0号分区内存属性。</li>
 * <li>uwLen为0时不进行内存属性设置。</li>
 * <li>内存属性设置时，从uwVirtAddr开始对section、page逐个进行设置。某一页内存属性设置不成功时，已经设置成功的保持设置属性。</li>
 * </ul>
 *
 * @param  uwVirtAddr  [IN] 类型#UINT32，虚拟地址起始地址。
 * @param  uwLen       [IN] 类型#UINT32，设置内存属性长度。
 * @param  uwStateMask [IN] 类型#UINT32，内存属性掩码，区分平台属性用，当前为无效参数，可忽略。
 * @param  uwState     [IN] 类型#UINT32，内存属性。
 *
 * @retval #OS_ERRNO_MMU_NOT_PAGE_ALIGNED       0x02001201，传入参数虚拟地址、物理地址或长度非page对齐。
 * @retval #OS_ERRNO_MMU_ADDR_NOT_MAPPED        0x02001210，传入虚拟地址未映射过。
 * @retval #OS_ERRNO_MMU_ADDR_OVERFLOW          0x02001209，地址溢出。
 * @retval #OS_ERRNO_MMU_DEOPTIMIZE_FAIL        0x02001211，页表拆分时内存不足。
 * @retval #OS_ERRNO_MMU_ATTRIBUTE_ERR          0x02001217，入参uwState的ATTRIBUTE属性配置不正确。
 * @retval #OS_ERRNO_MMU_ACCESS_ERR             0x02001218，入参uwState的ACCESS属性配置不正确。
 * @retval #SRE_OK                              0x00000000，内存属性设置成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */
extern UINT32 SRE_MmuStateSet(UINT32 uwVirtAddr, UINT32 uwLen, UINT32 uwStateMask, UINT32 uwState);

/**
 * @ingroup  SRE_mmu
 * @brief 使能MMU。
 *
 * @par 描述:
 * 使能MMU。
 *
 * @attention
 * <ul>
 * <li>该操作仅在Cortex-AX平台下支持。</li>
 * <li>只允许MMU模块初始化完成后调用。</li>
 * </ul>
 *
 * @param 无
 *
 * @retval #OS_ERRNO_MMU_ADDR_NOT_MAPPED        0x02001210，虚拟地址未映射。
 * @retval #SRE_OK                              0x00000000，MMU使能成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see SRE_MmuDisable
 */
extern UINT32 SRE_MmuEnable(VOID);

/**
 * @ingroup  SRE_mmu
 * @brief 除能MMU。
 *
 * @par 描述:
 * 除能MMU。
 *
 * @attention
 * <ul>
 * <li>该操作仅在Cortex-AX平台下支持。</li>
 * <li>只允许MMU模块初始化完成后调用。</li>
 * </ul>
 *
 * @param 无
 *
 * @retval #SRE_OK                              0x00000000，MMU除能成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see SRE_MmuEnable
 */
extern UINT32 SRE_MmuDisable(VOID);

/**
 * @ingroup  SRE_mmu
 * @brief 页表合并。
 *
 * @par 描述:
 * 合并uwVirtAddr开始，长度为uwLen的内存空间页表。
 *
 * @attention
 * <ul>
 * <li>该操作仅在Cortex-AX平台下支持。</li>
 * <li>只允许MMU模块初始化完成后调用。</li>
 * <li>该接口遍历入参地址空间，对物理地址连续、内存属性一致的内存进行合并。</li>
 * </ul>
 *
 * @param  uwVirtAddr  [IN] 类型#UINT32，合并起始地址。
 * @param  uwLen       [IN] 类型#UINT32，合并长度。
 *
 * @retval #OS_ERRNO_MMU_NOT_PAGE_ALIGNED       0x02001201，起始地址或长度非page对齐。
 * @retval #OS_ERRNO_MMU_ADDR_OVERFLOW          0x02001209，地址溢出。
 * @retval #OS_ERRNO_MMU_OPTIMIZE_NO_NEED       0x02001212，长度不足合并为大页表。
 * @retval #SRE_OK                              0x00000000，合并成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */
extern UINT32 SRE_MmuOptimize(UINT32 uwVirtAddr, UINT32 uwLen);

/************************************************HIDSP interface*************************************************/
#elif(OS_HARDWARE_PLATFORM == OS_HIDSP)
/************************* Permission 属性(可以将宏组合) *********************/
/**
 * @ingroup SRE_mmu
 * MMU内存permission属性-可读。
 */
#define OS_MMU_STATE_READ                                      0x4

/**
 * @ingroup SRE_mmu
 * MMU内存permission属性-可写。
 */
#define OS_MMU_STATE_WRITE                                     0x2

/**
 * @ingroup SRE_mmu
 * MMU内存permission属性-可执行。
 */
#define OS_MMU_STATE_EXE                                       0x1

/**************************** Share属性(为0表示不可共享) *********************/
/**
 * @ingroup SRE_mmu
 * MMU内存共享属性-可共享。
 */
#define OS_MMU_STATE_SHARED                                    0x8

/*************************** Speculative 属性 (为0表示不可投机)***************/
/**
 * @ingroup SRE_mmu
 * MMU内存投机属性-可投机。
 */
#define OS_MMU_STATE_SPEC                                      0x10

/***************************** Cache   属性 (以下宏只能选择一个)**************/
/**
 * @ingroup SRE_mmu
 * MMU内存cache属性-不可cache不可buffer。
 */
#define OS_MMU_STATE_NONCACHE_NONBUFFER                        0x0

/**
 * @ingroup SRE_mmu
 * MMU内存cache属性-不可cache可buffer。
 */
#define OS_MMU_STATE_NONCACHE_BUFFERABLE                       0x1

/**
 * @ingroup SRE_mmu
 * MMU内存cache属性-writeback,no allocate。
 */
#define OS_MMU_STATE_WRITEBACK_NOALLOC                         0x3

/**
 * @ingroup SRE_mmu
 * MMU内存cache属性-writeback, writeallocate。
 */
#define OS_MMU_STATE_WRITEBACK_ALLOC                           0x7

/**
 * @ingroup SRE_mmu
 * MMU内存cache属性-writethrough。
 */
#define OS_MMU_STATE_WRITETHROUGH                              0x2

/**
 * @ingroup SRE_mmu
 * TLB无效操作时指定的无效方式:页无效、context无效、无效ALL.
 *
 * TLB无效方式。
 */
typedef enum
{
    NON_INV     = 0      ,              /**< none opreation   */
    PAGE_INV    = 1      ,              /**< page invalidate    */
    CONTEXT_INV = 2      ,              /**< context invalidate */
    ALL_INV     = 3      ,              /**< all invalidate     */
    RESERVED    = 4
}OS_TLB_INV_POLICY;

/**
 * @ingroup SRE_mmu
 * MMU的3种模式:BYPASS模式、MMU模式、BACKDOOR模式.
 *
 * MMU模式。
 */
typedef enum
{
    SYS_MODE_BYPASS     = 0b00,         /**< BYPASS模式*/
    SYS_MODE_MMU        = 0b01,         /**< MMU模式*/
    SYS_MODE_BACKDOOR   = 0b11          /**< BACKDOOR模式*/
}OS_SYS_MODE_E;


/**
 * @ingroup SRE_mmu
 * MMU的8种映射大小方式:4K、8K、16K、64K、256K、1M、4M、16M.
 *
 * MMU映射的页大小。
 */
typedef enum
{
    MMU_PAGE_4K    = 0b0000,
    MMU_PAGE_8K    = 0b0001,
    MMU_PAGE_16K   = 0b0010,
    MMU_PAGE_64K   = 0b0011,
    MMU_PAGE_256K  = 0b0100,
    MMU_PAGE_1M    = 0b0101,
    MMU_PAGE_4M    = 0b0110,
    MMU_PAGE_16M   = 0b0111,
    MMU_INVALID    = 0b1000,
}MMU_PAGE_SIZE_E;

/**
 * @ingroup SRE_mmu
 * TLB的三种类型:L1 ITLB、L1 DTLB、L2 TLB.
 *
 * TLB类型。
 */
typedef enum
{
    L1_ITLB         ,            /**< L1_ITLB*/
    L1_DTLB         ,            /**< L1_DTLB*/
    L2_TLB                       /**< L2_TLB */
}OS_TLB_TYPE;

/**
 * @ingroup SRE_mmu
 * 一级TLB的输入字段.
 *
 * 一级TLB的输入字段.
 */
typedef struct tagL1TlbVirInput
{
    UINT32          uwContextID : 8;
    MMU_PAGE_SIZE_E uwPageSize  : 4;
    UINT32          uwVirAddr   : 20;
}L1_TLB_VIR_INPUT_S;

/**
 * @ingroup SRE_mmu
 * 一级ITLB的输入字段.
 *
 * 一级TLB的输入字段.
 */
typedef struct tagL1ITlbPhyInput
{
    UINT32                  uwEn     : 1;
    UINT32                  uwPermit : 3;
    UINT32                  uwRsv0   : 3;
    UINT32                  uwShare  : 1;
    UINT32                  uwSpec   : 1;
    UINT32                  uwCA     : 2;
    UINT32                  uwRsv1   : 1;
    UINT32                  uwPhyAddr: 20;
}L1_ITLB_PHY_INPUT_S;

typedef struct tagL1DTlbPhyInput
{
    UINT32                  uwEn     : 1;
    UINT32                  uwPermit : 3;
    UINT32                  uwRsv0   : 3;
    UINT32                  uwShare  : 1;
    UINT32                  uwSpec   : 1;
    UINT32                  uwCA     : 3;
    UINT32                  uwPhyAddr: 20;
}L1_DTLB_PHY_INPUT_S;

/**
 * @ingroup SRE_mmu
 * 错误码：MMU模式配置错误。
 *
 * 值: 0x0200121c
 *
 * 解决方案: MMU模式错误，只能是BYPASS,MMU三种模式。
 */
#define OS_ERRNO_MMU_MODE_INVALID                               SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x1c)

/**
 * @ingroup SRE_mmu
 * 错误码：TLB 内容查询时index错误。
 *
 * 值: 0x0200121d
 *
 * 解决方案: TLB查询时index有效范围是0~31。
 */
#define OS_ERRNO_MMU_TLB_INDEX_INVALID                          SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x1d)

/**
 * @ingroup SRE_mmu
 * 错误码：TLB 类型错误。
 *
 * 值: 0x0200121e
 *
 * 解决方案: TLB 类型错误，只能是L1 ITLB,L1 DTLB,L2 TLB三种。
 */
#define OS_ERRNO_MMU_TLB_TYPE_INVALID                           SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x1e)

/**
 * @ingroup SRE_mmu
 * 错误码：TLB 无效策略非法。
 *
 * 值: 0x0200121f
 *
 * 解决方案: TLB 无效策略非法，只能是PAGE_INV,LINE_INV,ALL_INV三种。
 */
#define OS_ERRNO_MMU_TLB_INV_POLICY_INVALID                     SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x1f)

/**
 * @ingroup SRE_mmu
 * 错误码：TLB way number非法。
 *
 * 值: 0x02001220
 *
 * 解决方案: WAY number只能是0~7。
 */
#define OS_ERRNO_MMU_WAY_NUMBER_INVALID                         SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x20)

/**
 * @ingroup SRE_mmu
 * 错误码：获取的对应index的L1TLB内容无效。
 *
 * 值: 0x02001221
 *
 * 解决方案: 确保该index输入正确以及对应的TLB内容未被无效或者替换。
 */
#define OS_ERRNO_MMU_TLB_DATA_INVALID                          SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x21)

/**
 * @ingroup SRE_mmu
 * 错误码：Page Size非法。
 *
 * 值: 0x02001222
 *
 * 解决方案: 修改Page Size。
 */
#define OS_ERRNO_MMU_PAGE_SIZE_INVALID                          SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x22)

/**
 * @ingroup SRE_mmu
 * 错误码：SYSTEM MODE非法。
 *
 * 值: 0x02001223
 *
 * 解决方案: 只能为SUPER_MODE或者USER_MODE。
 */
#define OS_ERRNO_MMU_SYS_MODE_INVALID                           SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x23)

/**
 * @ingroup SRE_mmu
 * 错误码：设置的内存属性非法。
 *
 * 值: 0x02001224
 *
 * 解决方案: 使用OS提供的属性宏设置内存的属性。
 */
#define OS_ERRNO_MMU_PAGEMAP_STATE_INVALID                      SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x24)

/**
 * @ingroup SRE_mmu
 * 错误码：虚拟地址、物理地址或长度非page对齐。
 *
 * 值: 0x02001225
 *
 * 解决方案: 检查入参虚拟地址、物理地址、长度是否page对齐。
 */
#define OS_ERRNO_MMU_NOT_PAGE_ALIGNED                            SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x25)

/**
 * @ingroup SRE_mmu
 * 错误码：映射或内存属性设置地址溢出。
 *
 * 值: 0x02001226
 *
 * 解决方案: 检查入参地址和长度是否合理。
 */
#define OS_ERRNO_MMU_ADDR_OVERFLOW                               SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x26)

/**
 * @ingroup SRE_mmu
 * 错误码：MMU模式下，实地址转虚地址失败。
 *
 * 值: 0x02001227
 *
 * 解决方案: MMU模式下，实地址未进行映射。
 */
#define OS_ERRNO_MMU_PA2VA_ERROR                                SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x27)

/**
 * @ingroup SRE_mmu
 * 错误码：地址转换接口，入参虚拟地址、物理地址非4字节对齐。
 *
 * 值: 0x02001228
 *
 * 解决方案: 检查入参虚拟地址、物理地址是否4字节对齐。
 */
#define OS_ERRNO_MMU_NOT_WORD_ALIGNED                            SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x28)

/**
 * @ingroup SRE_mmu
 * 错误码：二级页表内存不够。
 *
 * 值: 0x02001229
 *
 * 解决方案: 扩大二级页表空间
 */
#define OS_ERRNO_MMU_SIZE_ERROR                                SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x29)

/**
 * @ingroup SRE_mmu
 * 错误码：L2TLB setindex number非法。
 *
 * 值: 0x02001230
 *
 * 解决方案: setindex只能是0~127。
 */
#define OS_ERRNO_MMU_SET_NUMBER_INVALID                         SRE_ERRNO_OS_ERROR(OS_MID_MMU, 0x30)

/**
 * @ingroup  SRE_mmu
 * @brief 设置系统模式。
 *
 * @par 描述:
 * 支持MMU、BYPASS两种模式。
 *
 * @attention
 * <ul>
 * </ul>
 *
 * @param  enSysMode  [IN] 类型#UINT32，系统模式。
 *
 * @retval 无。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see
 */
extern UINT32 SRE_MmuModeSet(OS_SYS_MODE_E enSysMode);

/**
 * @ingroup  SRE_mmu
 * @brief 读取L1 ITLB内容。
 *
 * @par 描述:
 * 读取L1 ITLB内容。
 *
 * @attention
 * <ul>
 * </ul>
 *
 * @param  uwTlbIndx  [IN] 类型#UINT32，TLB index, 范围在0~31
 * @param  pstL1TlbVir  [OUT] 类型#L1_TLB_VIR_INPUT_S*, ITLB虚拟地址信息
 * @param  pstL1ITlbPhy  [OUT] 类型#L1_ITLB_PHY_INPUT_S*，ITLB物理地址信息
 *
 * @retval #SRE_OK 读取成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see
 */
extern UINT32 SRE_L1ITlbEntryRead(UINT32 uwTlbIndx, L1_TLB_VIR_INPUT_S *pstL1TlbVir, L1_ITLB_PHY_INPUT_S *pstL1ITlbPhy);

/**
 * @ingroup  SRE_mmu
 * @brief 读取L1 DTLB内容。
 *
 * @par 描述:
 * 读取L1 DTLB内容。
 *
 * @attention
 * <ul>
 * </ul>
 *
 * @param  uwTlbIndx     [IN] 类型#UINT32，TLB index, 范围在0~31
 * @param  pstL1TlbVir   [OUT] 类型#L1_TLB_VIR_INPUT_S*, DTLB虚拟地址信息
 * @param  pstL1DTlbPhy  [OUT] 类型#L1_DTLB_PHY_INPUT_S*，DTLB物理地址信息
 *
 * @retval SRE_OK 读取成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see
 */
extern UINT32 SRE_L1DTlbEntryRead(UINT32 uwTlbIndx, L1_TLB_VIR_INPUT_S *pstL1TlbVir, L1_DTLB_PHY_INPUT_S *pstL1DTlbPhy);

/**
 * @ingroup  SRE_mmu
 * @brief 无效L1 TLB。
 *
 * @par 描述:
 * 无效L1 TLB。
 *
 * @attention
 * <ul>
 * </ul>
 *
 * @param  enTlbType    [IN] 类型#OS_TLB_TYPE，ITLB或者DTLB或者L2 TLB
 * @param  enInvPolicy  [IN] 类型#OS_TLB_INV_POLICY，page inv,context inv,all inv
 * @param  uwVirAddr    [IN] 类型#UINT32，虚拟地址,针对page inv方式有效
 * @param  uwContextID  [IN] 类型#UINT32，context ID，针对page inv和context inv有效
 *
 * @retval SRE_OK 成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see
 */
extern UINT32 SRE_TlbInv(OS_TLB_TYPE enTlbType , OS_TLB_INV_POLICY enInvPolicy,
                           UINT32 uwVirAddr , UINT32 uwContextID);

typedef struct tagPageMemAttr
{
    UINT32      enPermit : 3;
    UINT32      uwShare  : 1;
    UINT32      uwSpec   : 1;
    UINT32      enCA     : 3;
    UINT32      uwRsv    : 24;

}PAGE_MEM_ATTR_S;

/**
 * @ingroup  SRE_mmu
 * @brief 映射虚拟地址到物理地址。
 *
 * @par 描述:
 * 根据传入参数uwVirtAddr、uwPhysAddr、enPageSize进行虚拟、物理地址映射，并按照pstAttr设置内存属性。
 *
 * @attention
 * <ul>
 * <li>只允许MMU模块初始化完成后调用。</li>
 * <li>地址映射时，每次映射一页，enPageSize必须为8种size之一。</li>
 * <li>uwTransTblAddr为页表首地址，每张页表为4M,首地址为1M对齐。/li>
 * </ul>
 *
 * @param  uwTransTblAddr  [IN] 类型#UINT32，页表首地址。
 * @param  pVStart         [IN] 类型#MMU_VA_T，映射虚拟地址起始地址。
 * @param  pPStart         [IN] 类型#MMU_PA_T，映射物理地址起始地址。
 * @param  enPageSize      [IN] 类型#MMU_PAGE_SIZE_E，映射Page类型。
 * @param  uwAttr          [IN] 类型#UINT32，内存属性，uwAttr有效bit位为(bit7-0)，bit2-0:read,write,execute; bit3:shareable; bit4:speculative; bit7-5:cache attributes;
 *
 * @retval #SRE_OK         0x00000000，虚拟、物理地址映射成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */

extern UINT32 SRE_MmuSingleMap(UINT32 uwTransTblAddr, MMU_VA_T pVStart, MMU_PA_T pPStart,
                            MMU_PAGE_SIZE_E enPageSize,  UINT32 uwAttr);

/**
 * @ingroup  SRE_mmu
 * @brief 映射虚拟地址到物理地址。
 *
 * @par 描述:
 * 映射连续的uwPageNum个page size相同且为enPageSize的页，并按照uwAttr设置内存属性。
 *
 * @attention
 * <ul>
 * <li>只允许MMU模块初始化完成后调用。</li>
 * <li>地址映射时，enPageSize必须为8种size之一。</li>
 * <li>uwTransTblAddr为页表首地址，每张页表为4M,首地址为1M对齐。/li>
 * </ul>
 *
 * @param  uwTransTblAddr  [IN] 类型#UINT32，页表首地址。
 * @param  uwVirtAddr      [IN] 类型#UINT32，映射虚拟地址起始地址。
 * @param  uwPhysAddr      [IN] 类型#UINT32，映射物理地址起始地址。
 * @param  enPageSize      [IN] 类型#MMU_PAGE_SIZE_E，映射的页类型。
 * @param  uwPageNum       [IN] 类型#UINT32，映射的页数目。
 * @param  uwAttr          [IN] 类型#UINT32，内存属性，uwAttr有效bit位为(bit7-0)，bit2-0:read,write,execute; bit3:shareable; bit4:speculative; bit7-5:cache attributes;
 *
 * @retval #SRE_OK                              0x00000000，虚拟、物理地址映射成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */
extern UINT32 SRE_MmuGroupMap(UINT32 uwTransTblAddr, MMU_VA_T pVStart, MMU_PA_T pPStart,
                               MMU_PAGE_SIZE_E enPageSize, UINT32 uwPageNum, UINT32 uwAttr);

/**
 * @ingroup  SRE_mmu
 * @brief 内存属性设置。
 *
 * @par 描述:
 * 设置基地址为uwTransTblAddr的页表，虚拟地址uwVirtAddr，长度uwLen的内存属性为pstAttr。
 *
 * @attention
 * <ul>
 * <li>只允许MMU模块初始化完成后调用。</li>
 * <li>内存属性设置时，从uwVirtAddr开始对section、page逐个进行设置。某一页内存属性设置不成功时，已经设置成功的保持设置属性。</li>
 * </ul>
 *
 * @param  uwTransTblAddr   [IN] 类型#UINT32，页表首地址。
 * @param  uwVirtAddr       [IN] 类型#UINT32，虚拟地址起始地址。
 * @param  uwLen            [IN] 类型#UINT32，设置内存属性长度。
 * @param  uwAttr           [IN] 类型#UINT32，内存属性，uwAttr有效bit位为(bit7-0)，bit2-0:read,write,execute; bit3:shareable; bit4:speculative; bit7-5:cache attributes;
 *
 * @retval #SRE_OK          0x00000000，内存属性设置成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */
extern UINT32 SRE_MmuStateSet(UINT32 uwTransTblAddr,UINT32 uwVirtAddr, UINT32 uwLen, UINT32 uwAttr);

/**
 * @ingroup  SRE_mmu
 * @brief Backdoor模式读取L2TLB内容
 *
 * @par 描述:
 * Backdoor模式读取L2TLB内容
 *
 * @attention
 * <ul>
 * <li>在进入backdoor模式之前，系统需要首先设置为bypass模式</li>
 * </ul>
 *
 * @param  uwSetIndex       [IN] 类型#UINT32，set index。
 * @param  uwWayIndex       [IN] 类型#UINT32，way index。
 * @param  puwMmuPbaInfo    [OUT] 类型#UINT32 *,read PBA 。
 * @param  puwMmuVbaInfo    [OUT] 类型#UINT32 *,read VBA。
 *
 * @retval #SRE_OK  成功
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */
extern UINT32 SRE_MmuBackDoorGet(UINT32 uwSetIndex, UINT32 uwWayIndex, UINT32 *puwMmuPbaInfo, UINT32 *puwMmuVbaInfo);

typedef struct tagDefMmuMemAttr
{
    UINT32      uwShared     : 1;
    UINT32      uwNonDes     : 1;
    UINT32      uwCA         : 3;
    UINT32      uwRsv0       : 27;
}DEF_MMU_MEM_ATTR_S;
/**
 * @ingroup  SRE_mmu
 * @brief Data区域默认属性配置。
 *
 * @par 描述:
 * BYPASS模式下，全数据空间有效。
 *
 * @attention
 * <ul>
 * </ul>
 *
 * @param  uwTrdID          [IN] 类型#UINT32，线程ID,0/1
 * @param  uwAttr           [IN] 类型#UINT32，默认属性，uwAttr有效bit位为(bit4-0)， bit0:shareable ; bit1:speculative; bit4-2:cache attributes;
 *
 * @retval #SRE_OK  成功
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */
extern UINT32 SRE_DataDefaultAttrSet(UINT32 uwTrdID, UINT32 uwAttr);

/**
 * @ingroup  SRE_mmu
 * @brief Text区域默认属性配置。
 *
 * @par 描述:
 * BYPASS模式下，全代码空间有效。
 *
 * @attention
 * <ul>
 * <li>系统必须处于BYPASS模式，MMU模式下，要求所有的代码段在page table中有映射</li>
 * </ul>
 *
 * @param  uwAttr           [IN] 类型#UINT32，默认属性，uwAttr有效bit位为(bit4-0)， bit0:shareable ; bit1:speculative; bit4-2:cache attributes;
 *
 * @retval #SRE_OK
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */
extern UINT32 SRE_TextDefaultAttrSet(UINT32 uwAttr);

/**
 * @ingroup  SRE_mmu
 * @brief 配置L2 TLB查找的顺序
 *
 * @par 描述:
 * 配置L2 TLB查找的顺序
 *
 * @attention
 * <ul>
 * <li></li>
 * </ul>
 *
 * @param  uwOrdValue           [IN] 类型#UINT32，优先级顺序。
 *
 * @retval #SRE_OK  成功
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */
extern UINT32 SRE_L2TlbOrdSet(UINT32 uwOrdValue);

/**
 * @ingroup  SRE_mmu
 * @brief 配置页表首地址
 *
 * @par 描述:
 * 配置super和user状态下页表基地址
 *
 * @attention
 * <ul>
 * <li></li>
 * </ul>
 *
 * @param  enMode           [IN] 类型#OS_SYSTEM_MODE，SUPER或者USER模式。
 * @param  uwBaseAddr       [IN] 类型#UINT32，页表基地址。
 *
 * @retval #SRE_OK  成功
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */
typedef enum
{
    SUPER_MODE         ,
    USER_MODE
}OS_SYSTEM_MODE;
extern UINT32 SRE_MmuBaseSet(OS_SYSTEM_MODE enMode, UINT32 uwBaseAddr);

/**
 * @ingroup  SRE_mmu
 * @brief 配置ITLB Context ID Mask
 *
 * @par 描述:
 * 配置ITLB Context ID Mask
 *
 * @attention
 * <ul>
 * <li></li>
 * </ul>
 *
 * @param  uwMsk       [IN] 类型#UINT32，Context ID Mask。
 *
 * @retval #SRE_OK  成功
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */
extern UINT32 SRE_ITlbCIdMsk(UINT32 uwCidMsk);

/**
 * @ingroup  SRE_mmu
 * @brief 配置二级页表基地址和大小
 *
 * @par 描述:
 * 配置ITLB Context ID Mask
 *
 * @attention
 * <ul>
 * <li></li>
 * </ul>
 *
 * @param  uwBaseAddr       [IN] 类型#UINT32，二级页表基地址
 * @param  uwSize           [IN] 类型#UINT32，大小
 *
 * @retval #SRE_OK  成功
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see NA
 */
extern UINT32 SRE_MmuL2ArgSet(UINT32 uwBaseAddr, UINT32 uwSize);

/**
 * @ingroup  SRE_mmu
 * @brief TLB Preload & preload lock & unlock。
 *
 * @par 描述:
 * TLB Preload & preload lock & unlock
 *
 * @attention
 * <ul>
 * </ul>
 *
 * @param  enTlbType    [IN] 类型#OS_TLB_TYPE，ITLB或者DTLB
 * @param  uwVirAddr    [IN] 类型#UINT32，虚拟地址
 *
 * @retval SRE_OK 成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see
 */
OS_SEC_ALW_INLINE INLINE  UINT32 SRE_TlbPreload(UINT32 uwVirAddr)
{
    __asm__ __volatile__("mmupf@agl %0 " :: "r"(uwVirAddr));

    return SRE_OK;
}

OS_SEC_ALW_INLINE INLINE UINT32 SRE_TlbPreloadLock(UINT32 uwVirAddr)
{
    __asm__ __volatile__("mmupflck@agl %0 "  :: "r"(uwVirAddr));

    return SRE_OK;
}

OS_SEC_ALW_INLINE INLINE UINT32 SRE_TlbUnlock(UINT32 uwVirAddr)
{
    __asm__ __volatile__("mmuulck@agl %0 "  :: "r"(uwVirAddr));

    return SRE_OK;
}
typedef  struct tagSecAttr{
    UINT32 uwSecVA;
    UINT32 uwSecPA;
    UINT32 uwSecSize;
    UINT32 uwSecAttr;
}SEC_ATTR_S;

typedef  struct tagSlaveCoreBootPara
{
    UINT32  uwCoreId;               /*  取值范围0~33*/
    UINT32  uwCIDR;                 /*  context ID 低8bit有效*/
    UINT32  uwPageTblAddr;
    UINT32  uwPageL2TblAddr;
    UINT32  uwPageL2Len;
    BOOL    bMmuMapFlag;            /* TRUE:表示需要配置后续vector heap stack secinfo;False:用户通过内存加载方式配置，此处无需配置*/
    SEC_ATTR_S  stIntVectorSec;     /*线程启动必须配置，指定vecbase，需12bit对齐*/
    SEC_ATTR_S  stSysHeapSec;
    SEC_ATTR_S  stSysStackSec;      /*线程启动必须配置，指定SP=起始地址+size*/
    UINT32 uwSecNum;                /*stSecInfo结构体个数*/
    SEC_ATTR_S  stSecInfo[0];
}SLAVE_CORE_BOOTPARA_S;

/**
 * @ingroup  SRE_mmu
 * @brief
 *
 * @par 描述:配置其他核启动参数
 *
 *
 * @attention
 * <ul>
 * </ul>
 *
 * @param  pstCoreBootPara    [IN] 类型#OS_TLB_TYPE，ITLB或者DTLB
 *
 * @retval SRE_OK 成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see
 */
extern UINT32 SRE_SlavePreBoot(SLAVE_CORE_BOOTPARA_S  *pstCoreBootPara);

/**
 * @ingroup  SRE_mmu
 * @brief
 *
 * @par 描述:触发核号为uwCoreId的核的wakeup中断
 *
 *
 * @attention
 * <ul>
 * </ul>
 *
 * @param  uwCoreId    [IN] 类型#UINT32，核号，包含线程号
 * @param  uwWakeupIntPri    [IN] 类型#UINT32，中断优先级
 *
 * @retval SRE_OK 成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see
 */
extern UINT32 SRE_CoreWakeUp(UINT32 uwCoreId,  UINT32 uwWakeupIntPri);

/**
 * @ingroup  SRE_mmu
 * @brief
 *
 * @par 描述: 触发核号为uwCoreId的核的boot中断
 *
 *
 * @attention
 * <ul>
 * </ul>
 *
 * @param  uwCoreId    [IN] 类型#UINT32，核号，包含线程号
 * @param  uwWakeupIntPri    [IN] 类型#UINT32，中断优先级
 *
 * @retval SRE_OK 成功。
 * @par 依赖:
 * <ul><li>sre_mmu.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see
 */
extern UINT32 SRE_CoreBootInt(UINT32 uwCoreId,  UINT32 uwWakeupIntPri);


#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _SRE_MMU_H */

/**
 * History:
 * 2013-02-18 h00213062: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
 */

