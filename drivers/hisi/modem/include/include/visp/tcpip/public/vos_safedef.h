/**
 * @file vos_safedef.h
 *
 * Copyright(C), 2008-2050, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * 描述：VISP安全函数转定义 \n
 */

/**@defgroup vos_safedef VISP安全函数转定义
 *@ingroup OSAL
*/

#ifndef __VOS_SAFEDEF_H__
#define __VOS_SAFEDEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#ifndef TCPIP_SAFE_DEBUG
#define TCPIP_Mem_Copy(pDstPtr, dst_size, pSrcPtr, copy_size) VOS_Mem_Copy_Safe(pDstPtr, dst_size, pSrcPtr, copy_size)
#else
#define TCPIP_Mem_Copy(pDstPtr, dst_size, pSrcPtr, copy_size) TCPIP_Safe_Mem_Copy(pDstPtr, dst_size, pSrcPtr, copy_size)
#endif

#define TCPIP_Mem_Set(pDstPtr, uChar, ulSetLen )              VOS_Mem_Set(pDstPtr, uChar, ulSetLen )

#ifndef TCPIP_SAFE_DEBUG
#define TCPIP_strncpy(pDstPtr, dst_size, pSrcPtr, copy_size) VOS_strncpy_Safe(pDstPtr, dst_size, pSrcPtr, copy_size)
#else
#define TCPIP_strncpy(pDstPtr, dst_size, pSrcPtr, copy_size) TCPIP_Safe_Strncpy(pDstPtr, dst_size, pSrcPtr, copy_size)
#endif

#ifndef TCPIP_SAFE_DEBUG
#define TCPIP_StrNCat(pDstPtr, dst_size, pSrcPtr, copy_size) VOS_StrNCat_Safe(pDstPtr, dst_size, pSrcPtr, copy_size)
#else
#define TCPIP_StrNCat(pDstPtr, dst_size, pSrcPtr, copy_size) TCPIP_Safe_StrNCat(pDstPtr, dst_size, pSrcPtr, copy_size)
#endif

#define TCPIP_nsprintf   VOS_nsprintf
#define TCPIP_nvsprintf  VOS_nvsprintf

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif //__VOS_SAFEDEF_H__


