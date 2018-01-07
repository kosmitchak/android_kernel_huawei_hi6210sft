
#ifndef __DRV_BBP_H__
#define __DRV_BBP_H__

#include "drv_global.h"
#include "drv_dpm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*用于bbp掉电时， 查询子帧号等接口的返回值*/
#define BBP_POWER_DOWN_BACK 0xffff
#define BBP_TDS_STU_SFN_MASK 0x1fff

#define BSP_BBP_POWER_DOWN BBP_POWER_DOWN_BACK

/* 下行业务数据DMA搬移完成中断,此中断只对应Modem核*/
typedef BSP_VOID (*BSPBBPIntDlTbFunc)(BSP_VOID);
/* BBP子帧中断处理函数,此中断只对应Modem核*/
typedef BSP_VOID (*BSPBBPIntTimerFunc)(BSP_VOID);
/* BBP TDS中断处理函数,此中断只对应Modem核 */
typedef BSP_VOID (*BSPBBPIntTdsFunc)(BSP_VOID);


extern BSP_VOID bsp_bbp_timerintregcb(BSPBBPIntTimerFunc pFunc);
#define DRV_BBP_INT_TIMER_REG_CB(pFunc) bsp_bbp_timerintregcb(pFunc)
/*兼容旧接口*/
#define BSP_BBPIntTimerRegCb(pFunc) bsp_bbp_timerintregcb(pFunc)
extern BSP_VOID bsp_bbp_dltbintregcb(BSPBBPIntDlTbFunc pFunc);
#define DRV_BBP_INT_DL_TB_REG_CB(pFunc) bsp_bbp_dltbintregcb(pFunc)
/*兼容旧接口*/
#define BSP_BBPIntDlTbRegCb(pFunc) bsp_bbp_dltbintregcb(pFunc)
extern BSP_VOID bsp_bbp_timerintclear(BSP_VOID);
#define DRV_BBP_INT_TIMER_CLEAR() bsp_bbp_timerintclear()
/*兼容旧接口*/
#define BSP_BBPIntTimerClear() bsp_bbp_timerintclear()
extern BSP_S32 bsp_bbp_timerintenable(BSP_VOID);
#define DRV_BBP_INT_TIMER_ENABLE() bsp_bbp_timerintenable()
/*兼容旧接口*/
#define BSP_BBPIntTimerEnable() bsp_bbp_timerintenable()
extern BSP_VOID bsp_bbp_timerintdisable(BSP_VOID);
#define DRV_BBP_INT_TIMER_DISABLE() bsp_bbp_timerintdisable()
/*兼容旧接口*/
#define BSP_BBPIntTimerDisable() bsp_bbp_timerintdisable()
extern BSP_U32 bsp_bbp_getcurtime(BSP_U64 *pCurTime);
extern BSP_U32 BSP_BBPGetCurTime(BSP_U64 *pCurTime);
#define DRV_BBP_GET_CUR_TIME(pCurTime) BSP_BBPGetCurTime(pCurTime)


extern BSP_S32 bsp_bbp_get_time_value(BSP_U32  *pulHigh32bitValue,  BSP_U32  *pulLow32bitValue);
#define DRV_GET_BBP_TIMER_VALUE(pulHigh32bitValue, pulLow32bitValue) bsp_bbp_get_time_value(pulHigh32bitValue, pulLow32bitValue)


extern BSP_U16 bsp_bbp_getsysframe(BSP_VOID);
#define DRV_GET_SYS_FRAME() bsp_bbp_getsysframe()
/*兼容旧接口*/
#define BSP_GetSysFrame() bsp_bbp_getsysframe()
extern BSP_U16 bsp_bbp_getsyssubframe(BSP_VOID);
#define DRV_GET_SYS_SUB_FRAME() bsp_bbp_getsyssubframe()
/*兼容旧接口*/
#define BSP_GetSysSubFrame() bsp_bbp_getsyssubframe()
extern BSP_U16 bsp_bbp_get_tds_subframe(BSP_VOID);
#define DRV_GET_TDS_SUB_FRAME()   bsp_bbp_get_tds_subframe()
/*兼容旧接口*/
#define BSP_GetTdsSubFrame()   bsp_bbp_get_tds_subframe()
extern BSP_U16 bsp_bbp_set_tds_subframoffset(BSP_U16 usOffset);
#define DRV_SET_TDS_SUB_FRAME_OFFSET(usOffset)   bsp_bbp_set_tds_subframoffset(usOffset)
/*兼容旧接口*/
#define BSP_SetTdsSubFrameOffset(usOffset)   bsp_bbp_set_tds_subframoffset(usOffset)
extern BSP_U32 bsp_bbp_get_tds_sleep_time(BSP_VOID);
#define DRV_GET_TDS_SLEEP_TIME()   bsp_bbp_get_tds_sleep_time()
/*兼容旧接口*/
#define BSP_GetTdsSleepTime()   bsp_bbp_get_tds_sleep_time()
extern BSP_VOID bsp_bbp_tdstf_regcb(BSPBBPIntTdsFunc pFunc);
#define DRV_BBP_INT_TDS_TF_REG_CB(pFunc) bsp_bbp_tdstf_regcb(pFunc)
/*兼容旧接口*/
#define BSP_BBPIntTdsTFRegCb(pFunc) bsp_bbp_tdstf_regcb(pFunc)
extern BSP_S32 bsp_bbp_tdstf_enable(BSP_VOID);
#define DRV_BBP_INT_TDS_TF_ENABLE() bsp_bbp_tdstf_enable()
/*兼容旧接口*/
#define BSP_BBPIntTdsTFEnable() bsp_bbp_tdstf_enable()
extern BSP_VOID bsp_bbp_tdstf_disable(BSP_VOID);
#define DRV_BBP_INT_TDS_TF_DISABLE() bsp_bbp_tdstf_disable()
/*兼容旧接口*/
#define BSP_BBPIntTdsTFDisable() bsp_bbp_tdstf_disable()
extern void bbp_wakeup_int_enable(PWC_COMM_MODE_E mode);
#define DRV_BBPWAKE_INT_ENABLE(mode) bbp_wakeup_int_enable(mode)
/*兼容旧接口*/
#define BSP_BBPIntWakeEnable(mode) bbp_wakeup_int_enable(mode)
extern void bbp_wakeup_int_disable(PWC_COMM_MODE_E mode);
#define DRV_BBPWAKE_INT_DISABLE(mode) bbp_wakeup_int_disable(mode)
/*兼容旧接口*/
#define BSP_BBPIntWakeDisable(mode) bbp_wakeup_int_disable(mode)
extern void bbp_wakeup_int_clear(PWC_COMM_MODE_E mode);
#define DRV_BBPWAKE_INT_CLEAR(mode) bbp_wakeup_int_clear(mode)
/*兼容旧接口*/
#define BSP_BBPIntWakeClear(mode) bbp_wakeup_int_clear(mode)
extern BSP_S32 bsp_bbp_get_tdsclk_switch(BSP_VOID);
#define DRV_GET_TDS_CLK_STATUS()   bsp_bbp_get_tdsclk_switch()
/*兼容旧接口*/
#define BSP_GetTdsClkSwitchStatus()   bsp_bbp_get_tdsclk_switch()
extern BSP_S32 bsp_bbp_get_means_flag(PWC_COMM_MODE_E enCommMode);
#define DRV_PWC_GET_MEANSFLAG(enCommMode) bsp_bbp_get_means_flag(enCommMode)
/*兼容旧接口*/
#define BSP_PWC_GetMeansFlag(enCommMode) bsp_bbp_get_means_flag(enCommMode)
extern int bsp_bbp_timer_int_init(PWC_COMM_MODE_E mode);
#define DRV_BBP_TIMER_INT_INIT(mode) bsp_bbp_timer_int_init(mode)


int bsp_bbp_dma_int_init(void);
#define DRV_BBP_DMA_INT_INIT() bsp_bbp_dma_int_init()


int bsp_bbp_dma_int_enable(void);
#define DRV_BBP_DMA_INT_ENABLE() bsp_bbp_dma_int_enable()

/*****************************************************************************
* 函 数 名  : bsp_bbp_lps_get_ta
* 功能描述  : 被LPS调用，查看上行子帧头相对系统子帧头的提前量
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : BSP_U16
* 修改记录  : 2013年11月15日  porting from v7r2,just for lte
*****************************************************************************/
extern BSP_U16 bsp_bbp_lps_get_ta(void);
#define DRV_LPS_GET_TA() bsp_bbp_lps_get_ta()
/*兼容旧接口*/
#define BSP_LPS_GetTa() bsp_bbp_lps_get_ta()

/*****************************************************************************
* 函 数 名  : bsp_bbp_timerintswitch
* 功能描述  : 用于获取lte 1ms定时器中断使能/开关状态
* 输入参数  : 无
* 输出参数  : BSP_BOOL  定时器中断开关状态(1:中断使能、0:中断去时能,BBP禁止访问时默认中断去使能)
* 返 回 值  : BSP_OK    LTE-BBP处于允许访问状态
*             BSP_ERROR LTE-BBP处于禁止访问状态
* 修改记录  : 2014年1月9日  zhoushan  creat
*****************************************************************************/
BSP_S32 bsp_bbp_timerintswitch(BSP_BOOL *pbStat);
#define DRV_BBP_INT_TIMER_SWITCH_STATUS(pbStat) bsp_bbp_timerintswitch(pbStat)
/*兼容旧接口*/
#define BSP_BBPIntTimerSwitchStatus(pbStat) bsp_bbp_timerintswitch(pbStat)
BSP_U32 bsp_bbp_get_ms(BSP_VOID);
#define DRV_GET_32K_MS()    bsp_bbp_get_ms()
/*兼容旧接口*/
#define BSP_BBP_Get32kMs()    bsp_bbp_get_ms()

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif
