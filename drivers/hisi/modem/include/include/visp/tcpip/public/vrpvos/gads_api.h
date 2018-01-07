/*******************************************************************************
*                                                                              *
*        Copyright 2003-2006  Huawei Technologies India Private Limited        *
*                          All Rights Reserved                                 *
*                                                                              *
*------------------------------------------------------------------------------*
*                                                                              *
*                              gads_api.h                                      *
*                                                                              *
*    Project Code:  VRP5.0                                                     *
*    Module Name:   GADS(General Array Data Structure)                         *
*    Create Date:  2003/05/17                                                  *
*         Author:  Rainbow                                                     *
*     Description: Output API and data type definition for GADS. App module    *
*                  who use GADS must include this header file.                 *
*                                                                              *
*------------------------------------------------------------------------------*
*  Modification History                                                        *
*  DATE        NAME             DESCRIPTION                                    *
*  --------------------------------------------------------------------------  *
*  2003-05-17  Rainbow           Create                                        *
 * 2008/01/10  z62474           Modify for A82D22522
*******************************************************************************/

#ifndef _GADS_API_H
#define _GADS_API_H

typedef        ULONG MINDEX;
typedef        UINTPTR GADS_HANDLE;
#define        GADS_INVALID_HANDLE                0 
#define        GADS_INVALID_MIDX                  0xFFFFFFFF
#define        GADS_INVALID_LIST_IDX              0x7FFFFFFF   
#define        GADS_USER_INFO_DEFAULT_BIT_LEN     0
#define        GADS_USER_INFO_DEFAULT_VALUE       0


#ifdef __cplusplus
extern "C"
{
#endif

GADS_HANDLE GADS_CreateArray(ULONG ulMaxNum, ULONG  ulAvailNum, ULONG ulUnitSize, 
            CHAR *pchName , ULONG ulMID, ULONG ulSID , ULONG ulMaxUserInfoBitLen);
ULONG  GADS_DeleteArray(GADS_HANDLE hGADS);
MINDEX GADS_CreateObj(GADS_HANDLE hGADS , ULONG ulUserInfo, VOID **ppObj);
ULONG  GADS_VerifyObjByMIndex(GADS_HANDLE hGADS ,MINDEX mIdx);
VOID   *GADS_CreateObjByMIndex(GADS_HANDLE hGADS ,MINDEX mIdx);
ULONG  GADS_DeleteObj(GADS_HANDLE hGADS ,MINDEX mIdx);
VOID   *GADS_GetObj(GADS_HANDLE hGADS , MINDEX mIdx ,ULONG *pulUserInfo);
ULONG  GADS_VerifyMindex(GADS_HANDLE hGADS, MINDEX mIdx);
VOID   *GADS_GetObjBySimpleIndex(GADS_HANDLE hGADS , ULONG ulIdx , MINDEX *pmIdx);
ULONG  GADS_GetUserInfo(GADS_HANDLE hGADS , MINDEX mIdx ,ULONG *pulUserInfo);
ULONG  GADS_GetUserInfoFromMindex(MINDEX mIdx,ULONG ulUserinfobitlen, ULONG ulMaxNumObj,ULONG *pulUserInfo);
ULONG  GADS_GetRealIndexFromMIndex(MINDEX mIdx,ULONG ulUserinfobitlen, ULONG ulMaxNumObj,ULONG *pulRealIndex);
ULONG  GADS_GetMaxStaticNum(GADS_HANDLE hGADS);
ULONG  GADS_GetUsedNum(GADS_HANDLE hGADS);
ULONG  GADS_GetMaxStaticAvailNum(GADS_HANDLE hGADS);
ULONG  GADS_SetMaxStaticAvailNum(GADS_HANDLE hGADS, ULONG ulMaxAvailNum);
ULONG  GADS_GetMaxAvailNum(GADS_HANDLE hGADS);
CHAR   *GADS_GetName(GADS_HANDLE hGADS);
MINDEX  GADS_GetUsedFirst(GADS_HANDLE hGADS);
MINDEX  GADS_GetUsedNext (GADS_HANDLE hGADS, MINDEX midxCurrPos);

#ifdef __cplusplus
}
#endif

#endif

