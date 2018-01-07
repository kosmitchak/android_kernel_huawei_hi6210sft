/**

                   Huawei Technologies Sweden AB (C), 2001-2011,

 ******************************************************************************
 * @file
 * The user interface to the CPROC_1X system time service. CPROC_1X receives
 * priodic notificaiton of the CPROC 1X system time. This time is saved and
 * is provided to user when @ref CPROC_STIME_GetSystemTime. A number of
 * macros for reading parts and validating the system time.
 * @brief
 * Public interface to the CPROC system time service.
 * @author  JGW13952
 * @version
 * @date    2013/11/18 13:57(+0100)
 * @copyright Huawei Technologies Sweden AB
******************************************************************************/

/*****************************************************************************
  1 Other files included
*****************************************************************************/
#ifndef INCLUSION_GUARD_CPROC1XSTIME_H
#define INCLUSION_GUARD_CPROC1XSTIME_H

#include "vos.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/*****************************************************************************
  2 Macro definitions
*****************************************************************************/
/**
 * Value used to indicate an invalid system time.
 */
#define CPROC_STIME_INVALID_VALUE (0xffffffffffffffffULL)

/**
 * @def CPROC_STIME_GET_LOW_BITS(uSystemTime)
 * @a   uSystemTime System time returned by CPROC_STIME_GetSystemTime
 * Returns the 32 least significant bits of the system time.
 */
#define CPROC_STIME_GET_LOW_BITS(uSystemTime)  \
    ((uSystemTime).stSystemTime.ulLowBits)

/**
 * @def CPROC_STIME_GET_HIGH_BITS(uSystemTime)
 * @a   uSystemTime System time returned by CPROC_STIME_GetSystemTime
 * Returns the 32 most significant bits of the system time.
 */
#define CPROC_STIME_GET_HIGH_BITS(uSystemTime) \
    ((uSystemTime).stSystemTime.ulHighBits)

/**
 * @def CPROC_STIME_GET_FULL_VALUE(uSystemTime)
 * @a   uSystemTime System time returned by CPROC_STIME_GetSystemTime
 * Returns the full 64 bit value of the system time. The user must
 * first read the system time with CPROC_STIME_GetSystemTime
 */
#define CPROC_STIME_GET_FULL_VALUE(uSystemTime)     \
    ((uSystemTime).ulwSystemTime)

/**
 * @def CPROC_STIME_VALID(usSystemTime)
 * @a   uSystemTime System time returned by CPROC_STIME_GetSystemTime
 * Returns VOS_FALSE if the system time in uSystemTime is invalid, else
 * VOS_TRUE is returned.
 */
#define CPROC_STIME_VALID(uSystemTime)                       \
    ( ( (CPROC_STIME_GET_HIGH_BITS(uSystemTime)              \
         == CPROC_STIME_INVALID_VALUE )                      \
     && (CPROC_STIME_GET_LOW_BITS(uSystemTime)               \
         == CPROC_STIME_INVALID_VALUE ) )                    \
      ? VOS_FALSE                                            \
      : VOS_TRUE )




/*****************************************************************************
  3 Enumeration declaration
*****************************************************************************/
/**
 * Used to indicate what system time is requested
 */
enum CPROC_STIME_SYSTEM_ENUM
{
    CPROC_STIME_SYSTEM_1X = 0,
    CPROC_STIME_SYSTEM_HRPD,
    CPROC_STIME_SYSTEM_BUTT
};
typedef VOS_UINT16 CPROC_STIME_SYSTEM_ENUM_UINT16;

/*****************************************************************************
  4 Message Header declaration
*****************************************************************************/

/*****************************************************************************
  5 Message declarations
*****************************************************************************/

/*****************************************************************************
  6 STRUCT declarations
*****************************************************************************/
/**
 * System time structure
 */
typedef struct
{
#ifdef VOS_BIG_ENDIAN
    VOS_UINT32 ulLowBits;  /** 32 least significant bits of the 1X system time */
    VOS_UINT32 ulHighBits; /** 32 most significant bits of the 1X system time  */
#else
    VOS_UINT32 ulHighBits; /** 32 most significant bits of the 1X system time  */
    VOS_UINT32 ulLowBits;  /** 32 least significant bits of the 1X system time */
#endif
} CPROC_STIME_SYSTEM_TIME_STRU;

/*****************************************************************************
  7 UNION declarations
*****************************************************************************/
/**
 * System time union,
 */
typedef union
{
    CPROC_STIME_SYSTEM_TIME_STRU stSystemTime;  /** system time structure      */
    VOS_UINT64                   ulwSystemTime; /** full 64 bit 1x system time */
} CPROC_STIME_SYSTEM_TIME_UNION;


/*****************************************************************************
  8 OTHER declarations
*****************************************************************************/

/*****************************************************************************
  9 Global variables
*****************************************************************************/

/*****************************************************************************
  10 Function declarations
*****************************************************************************/
/** **************************************************************************
  @fn        CPROC_STIME_SYSTEM_TIME_UNION CPROC_STIME_GetSystemTime(
                 CPROC_STIME_SYSTEM_ENUM_UINT16 enSystem );
  @details   Returns the last system time received. If no valid 1X system
             time is available, all '1' is returned. If the system time
             returned is valid, it is guaranteed to be no older than 10 ms.
             The granularity of the reported value is in 80 ms.
  @param[in] enSystem indicates what system (1X/HRPD) time is requested
  @return    system time no older than 10 ms or
             CPROC_SYSTEM_TIME_INVALID_VALUE
*/
/*
  History        :
  1.Date         : 2013/11/18
    Author       : jg213952
    Modification : Created function

*****************************************************************************/
CPROC_STIME_SYSTEM_TIME_UNION CPROC_STIME_GetSystemTime(
    CPROC_STIME_SYSTEM_ENUM_UINT16 enSystem );

/** **************************************************************************
  @fn        CPROC_STIME_SYSTEM_TIME_UNION CPROC_STIME_GetSystemTimeRaw(
                 CPROC_STIME_SYSTEM_ENUM_UINT16 enSystem );
  @details   Returns the last system time received. If no valid 1X system
             time is available, all '1' is returned. If the system time
             returned is valid, it is guaranteed to be no older than 10 ms.
             The function returns all bits of the system time value.
  @param[in] enSystem indicates what system (1X/HRPD) time is requested
  @return    system time no older than 10 ms or
             CPROC_SYSTEM_TIME_INVALID_VALUE
*/
/*
  History        :
  1.Date         : 2014/01/07
    Author       : tgw17581
    Modification : Created function

*****************************************************************************/
CPROC_STIME_SYSTEM_TIME_UNION CPROC_STIME_GetSystemTimeRaw(
    CPROC_STIME_SYSTEM_ENUM_UINT16 enSystem );


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INCLUSION_GUARD_CPROC1XSTIME_H */
