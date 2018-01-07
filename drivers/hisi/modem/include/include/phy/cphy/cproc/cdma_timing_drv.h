

#ifndef __CDMA_TIMING_DRV_H__
#define __CDMA_TIMING_DRV_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2  macro definition
*****************************************************************************/
#define CDMA_MODE_1X                                    (0)
#define CDMA_MODE_HRPD                                  (1)

#define CDMA_TIMING_CLIENT_BUTT				(8)     /* max clients of every period */

/* step of high reg: 5.12s / 80ms = 64 */
#define CDMA_SYSTEM_TIME_HIGHSEG_STEP_LENGTH    (64)

/* system time in low reg is bit 24~29 */
#define CDMA_SYSTEM_TIME_BBPREG_START_BIT       (24)
#define CDMA_SYSTEM_TIME_BBPREG_END_BIT         (29)

/* 31bit is maximum bit */
#define CDMA_SYSTEM_TIME_BBPREG_MAX_BIT         (31)

/*****************************************************************************
  3  enum definition
*****************************************************************************/
enum CDMA_TIMING_RTT_ENUM
{
    CDMA_TIMING_RTT_1X = CDMA_MODE_1X,
    CDMA_TIMING_RTT_EVDO = CDMA_MODE_HRPD,
    CDMA_TIMING_RTT_BUTT
};
typedef VOS_UINT16 CDMA_TIMING_RTT_ENUM_UINT16;


enum CDMA_TIMING_PERIOD_ENUM
{
    /* 1x Period is 10 ms */
    CDMA_TIMING_PERIOD_10_00_MS_1X    = 0,

    /* evdo Period is 2slot + 4slot */
    CDMA_TIMING_PERIOD_4096_CHIP_EVDO = 1,
    CDMA_TIMING_PERIOD_8192_CHIP_EVDO = 2,
    CDMA_TIMING_PERIOD_BUTT
};
typedef VOS_UINT16 CDMA_TIMING_PERIOD_ENUM_UINT16;



/*****************************************************************************
 枚举名    : BBP_REG_NUM_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 用来区分BBP寄存器
*****************************************************************************/
enum BBP_REG_NUM_ENUM
{
    BBP_1x_HIGH_SEG_DATA_REG       = 0,
    BBP_1x_LOW_SEG_DATA_REG        = 1,
    BBP_HRPD_HIGH_SEG_DATA_REG     = 2,
    BBP_HRPD_LOW_SEG_DATA_REG      = 3,
    BBP_REG_BUTT
};
typedef VOS_INT32 BBP_REG_NUM_ENUM_INT32;


/*****************************************************************************
 枚举名    : CDMA_SYSTEM_TIME_SEG_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : 用来区分系统时间的高位段和低位段
*****************************************************************************/
enum CDMA_SYSTEM_TIME_SEG_ENUM
{
    CDMA_SYSTEM_TIME_LOW_SEG           = 0,
    CDMA_SYSTEM_TIME_HIGH_SEG          = 1,
    CDMA_SYSTEM_TIME_BUTT
};
typedef VOS_UINT16 CDMA_SYSTEM_TIME_SEG_ENUM_UINT16;
/*****************************************************************************
  6 STRUCT definition
*****************************************************************************/
#pragma pack(4)
typedef VOS_VOID (* CDMA_TIME_CALL_BACK_T)(const VOS_UINT64 *pstCdmaSysTime, VOS_VOID *pvParam);


typedef struct
{
    /** callback */
    CDMA_TIME_CALL_BACK_T pfCallback;
    /** self defined para pointer */
    VOS_VOID *pvParam;
    /** clientid , one of CDMA_TIMING_CLIENT_ENUM */
    VOS_UINT32 uwClientid;
    /** uhwPeriod , one of CDMA_TIMING_PERIOD_ENUM */
    VOS_UINT16 uhwPeriod;
    /** uhwPeriod , one of CDMA_TIMING_RTT_ENUM */
    VOS_UINT16 uhwRttMode;
}CDMA_TIME_CALL_BACK_CLIENT_STRU;


typedef CDMA_TIME_CALL_BACK_CLIENT_STRU *CDMA_TIME_HANDLE_STRU;


/*****************************************************************************
  10 function definition
*****************************************************************************/
extern CDMA_TIME_HANDLE_STRU CDMA_Timing_Drv_RegisterIntEvent(CDMA_TIMING_RTT_ENUM_UINT16 enRtt, CDMA_TIMING_PERIOD_ENUM_UINT16 enPeriod,
                                                 CDMA_TIME_CALL_BACK_T pfCallback,
                                                 VOS_VOID *pvParam);

extern VOS_UINT32 CDMA_Timing_Drv_DeRegisterIntEvent(CDMA_TIME_HANDLE_STRU pstEventHandle);
	

extern VOS_UINT32 CDMA_Timing_Drv_GenSystemTime(VOS_UINT32  aulSystemTimeSeg[],
                                        VOS_UINT32 ulArrayLength,
                                        VOS_UINT64  *pullSystemTimeValue);


void CDMA_Timing_Drv_Initialize( void );


#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif

