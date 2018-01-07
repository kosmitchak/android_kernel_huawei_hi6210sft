/*--------------------------------------------------------------------------------------------------------------------------*/
/*!!Warning: This is a key information asset of Huawei Tech Co.,Ltd                                                         */
/*CODEMARK:kOyQZYzjDpyGdBAEC2GaWrXNs8MZYk9cpFfgXfKX0g1VVtQNxHnHCKsK8caFMiW8LvCCkwIW
Tc6iJlHGFi3nUeJwZnaqeEUBwq/bzU5cp8JMIZQR60CPwhsVGkig7D46L1mTcjaBXLwtoHt/
6v3DannTIsMO1Rlb7E6huEjiBi2GwEf1TMAEhigMyHIF2mQ7wv/k3Rb1nNyQxx1DXFvLYWHv
YFZ8jRdZOLWTX3jEAWYbD6MdnV+O6Dea6Flg16w6cBTNK0q/ghq5fjpNQMnBzQ==*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/**
* @file SRE_nsm.h
*
* Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
*
* 描述：有名空间模块对外头文件。 \n
*/

/**
 * @defgroup SRE_nsm  有名空间
 * @ingroup SRE_kernel
 */

#ifndef _SRE_NSM_H
#define _SRE_NSM_H

#include "sre_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/**
 * @ingroup SRE_nsm
 * 有名空间错误码：入参非法，传入指针为NULL。
 *
 * 值: 0x02001b01
 *
 * 解决方案：确保指针非空。
 */
#define OS_ERRNO_NSM_PTR_NULL                               SRE_ERRNO_OS_ERROR(OS_MID_NSM,0x01)

/**
 * @ingroup SRE_nsm
 * 有名空间错误码：传入的有名空间大小非法。
 *
 * 值: 0x02001b02
 *
 * 解决方案：有名空间配置大小需大于最小的有名空间大小。
 */

#define OS_ERRNO_NSM_SPACE_SIZE_ILLEG                       SRE_ERRNO_OS_ERROR(OS_MID_NSM,0x02)

/**
 * @ingroup SRE_nsm
 * 有名空间错误码：传入有名空间首地址未4字节对齐。
 *
 * 值: 0x02001b03
 *
 * 解决方案：有名空间首地址需4字节对齐。
 */
#define OS_ERRNO_NSM_ADDR_NOT_ALIGN                         SRE_ERRNO_OS_ERROR(OS_MID_NSM,0x03)

/**
 * @ingroup SRE_nsm
 * 有名空间错误码：传入有名空间大小未4字节对齐。
 *
 * 值: 0x02001b04
 *
 * 解决方案：有名空间大小需4字节对齐。
 */
#define OS_ERRNO_NSM_SIZE_NOT_ALIGN                         SRE_ERRNO_OS_ERROR(OS_MID_NSM,0x04)

/**
 * @ingroup SRE_nsm
 * 有名空间错误码：有名空间已满。
 *
 * 值: 0x02001b05
 *
 * 解决方案：添加记录时，确保有名空间有足够的空间存储。
 */
#define OS_ERRNO_NSM_MEM_FULL                               SRE_ERRNO_OS_ERROR(OS_MID_NSM,0x05)

/**
 * @ingroup SRE_nsm
 * 有名空间错误码：有名空间中记录重命名。
 *
 * 值: 0x02001b06
 *
 * 解决方案：往有名空间中添加记录时，确保记录名唯一。
 */
#define OS_ERRNO_NSM_OBJ_RENAME                             SRE_ERRNO_OS_ERROR(OS_MID_NSM,0x06)

/**
 * @ingroup SRE_nsm
 * 有名空间错误码：删除有名记录时，对应记录不存在。
 *
 * 值: 0x02001b07
 *
 * 解决方案：删除有名记录时，请确认相应记录存在。
 */
#define OS_ERRNO_NSM_DEL_NOT_FIND                           SRE_ERRNO_OS_ERROR(OS_MID_NSM,0x07)

/**
 * @ingroup SRE_nsm
 * 有名空间错误码：有名空间中对应记录不存在。
 *
 * 值: 0x02001b08
 *
 * 解决方案：确保该空间已有相应记录。
 */
#define OS_ERRNO_NSM_FIND_FAILURE                           SRE_ERRNO_OS_ERROR(OS_MID_NSM,0x08)

/**
 * 有名空间创建结构体定义。
 *
 * 有名空间创建结构体定义。
 */
typedef struct tagNsmModInfo
{
    VOID  *pStart;               /**< 有名空间起始地址                              */
    UINT32 uwSize;               /**< 有名空间内存空间大小                          */
}NSM_MOD_INFO_S;

/**
 * @ingroup  SRE_nsm
 * @brief 查询有名共享记录。
 *
 * @par 描述:
 * 通过传入空间号uwSpaceID，对象名uwNameId，在有名空间中查找对应的记录，并通过ppAddr和puwSize指针返回其地址和大小。
 *
 * @attention
 * <ul>
 * <li>无。</li>
 * </ul>
 *
 * @param  uwSpaceID     [IN]  类型#UINT32  ，有名空间号，暂为无效参数，留以后扩展使用。
 * @param  uwNameId      [IN]  类型#UINT32  ，有名共享对象名。
 * @param  ppAddr        [OUT] 类型#VOID ** ，返回有名对象首地址。
 * @param  puwSize       [OUT] 类型#UINT32 *，返回有名共享对象内存空间大小。
 *
 * @retval #OS_ERRNO_NSM_FIND_FAILURE           0x02001b08，有名空间中对应记录不存在。
 * @retval #OS_ERRNO_NSM_PTR_NULL               0x02001b01，入参非法，传入指针为NULL。
 * @retval #SRE_OK                              0x00000000，记录查找成功。
 * @par 依赖:
 * <ul><li>sre_nsm.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see SRE_NSReg | SRE_NSUnReg
 */
extern UINT32 SRE_NSFind (UINT32 uwSpaceID,  UINT32 uwNameId, VOID **ppAddr, UINT32 *puwSize);

/**
 * @ingroup  SRE_nsm
 * @brief 添加有名共享记录。
 *
 * @par 描述:
 * 通过传入对象名uwNameId，有名对象首地址pAddr，有名对象大小uwSize，在有名空间uwSpaceID中添加一条记录。
 *
 * @attention
 * <ul>
 * <li>无。</li>
 * </ul>
 *
 * @param  uwSpaceID     [IN]  类型#UINT32  ，有名空间号，暂为无效参数，留以后扩展使用。
 * @param  uwNameId      [IN]  类型#UINT32  ，有名共享对象名。
 * @param  pAddr         [IN]  类型#VOID *  ，有名对象首地址。
 * @param  uwSize        [IN]  类型#UINT32  ，有名共享对象内存空间大小。
 *
 * @retval #OS_ERRNO_NSM_MEM_FULL               0x02001b05，有名空间已满。
 * @retval #OS_ERRNO_NSM_PTR_NULL               0x02001b01，入参非法，传入指针为NULL。
 * @retval #OS_ERRNO_NSM_OBJ_RENAME             0x02001b06，有名空间中记录重命名。
 * @retval #SRE_OK                              0x00000000，记录添加成功。
 * @par 依赖:
 * <ul><li>sre_nsm.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see SRE_NSUnReg
 */
extern UINT32 SRE_NSReg (UINT32 uwSpaceID, UINT32 uwNameId, VOID *pAddr, UINT32 uwSize);

/**
 * @ingroup  SRE_nsm
 * @brief 删除有名共享记录。
 *
 * @par 描述:
 * 通过传入对象名uwNameId，在有名空间uwSpaceID中删除对应的记录。
 *
 * @attention
 * <ul>
 * <li>无。</li>
 * </ul>
 *
 * @param  uwSpaceID     [IN]  类型#UINT32  ，有名空间号，暂为无效参数，留以后扩展使用。
 * @param  uwNameId      [IN]  类型#UINT32  ，有名共享对象名。
 *
 * @retval #OS_ERRNO_NSM_DEL_NOT_FIND           0x02001b07，删除有名记录时，对应记录不存在。
 * @retval #SRE_OK                              0x00000000，记录删除成功。
 * @par 依赖:
 * <ul><li>sre_nsm.h：该接口声明所在的头文件。</li></ul>
 * @since RTOSck V100R002C00
 * @see SRE_NSReg
 */

extern UINT32 SRE_NSUnReg (UINT32 uwSpaceID, UINT32 uwNameId);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _SRE_NSM_H */

/**
 * History:
 * 2013-03-22 l00211012: Create this file.
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
*/

