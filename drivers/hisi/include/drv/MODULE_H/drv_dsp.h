
#ifndef __DRV_DSP_H__
#define __DRV_DSP_H__
#include "drv_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


extern int bsp_bbe_load_muti(void);
#define DRV_BBE_LOAD() bsp_bbe_load_muti()
/* 兼容老接口 */
#define BSP_LoadDsp() DRV_BBE_LOAD()
extern int bsp_bbe_run(void);
#define DRV_BBE_RUN() bsp_bbe_run()
/* 兼容老接口 */
#define BSP_RunDsp() DRV_BBE_RUN()
#define DRV_PWRCTRL_DSP_RUN() DRV_BBE_RUN()
extern void bsp_bbe_load_mode_set(PHY_LOAD_MODE_E mode);
#define DRV_BBE_LOAD_MODE_SET(mode) bsp_bbe_load_mode_set(mode)
/* 兼容老接口 */
#define BSP_LoadDspMode(mode) DRV_BBE_LOAD_MODE_SET(mode)
extern int bsp_bbe_power_on(void);
#define DRV_BBE_POWER_ON()  bsp_bbe_power_on()


extern int bsp_bbe_power_off(void);
#define DRV_BBE_POWER_OFF() bsp_bbe_power_off()


extern int bsp_bbe_stop(void);
#define DRV_PWRCTRL_DSP_HOLD() bsp_bbe_stop()


extern int bsp_msp_bbe_restore(void);
#define DRV_EDMA_DSP_MemRestore() bsp_msp_bbe_restore()


extern int bsp_msp_bbe_store(void);
#define DRV_EDMA_DSP_MemStore() bsp_msp_bbe_store()


extern int bsp_msp_wait_edma_ok(void);
#define DRV_EDMA_DSP_StateGet() bsp_msp_wait_edma_ok()


extern int bsp_bbe_unreset(void);
#define DRV_PWRCTRL_DSP_UNRESET() bsp_bbe_unreset()


extern int bsp_bbe_reset(void);
#define DRV_PWRCTRL_DSP_RESET() bsp_bbe_reset()


extern int bsp_bbe_clock_enable(void);
#define DRV_PWRCTRL_DSP_CLKENABLE() bsp_bbe_clock_enable()


extern int bsp_bbe_clock_disable(void);
#define DRV_PWRCTRL_DSP_CLKDISABLE() bsp_bbe_clock_disable()

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif


