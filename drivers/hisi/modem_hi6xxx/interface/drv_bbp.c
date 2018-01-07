

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BSP.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

BSP_U32 BSP_BBPGetCurTime(BSP_U64 *pCurTime)
{
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return bsp_bbp_getcurtime(pCurTime);
#endif

#if (defined BSP_CORE_APP)
	return 0;
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

