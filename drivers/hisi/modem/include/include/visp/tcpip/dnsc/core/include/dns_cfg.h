/*******************************************************************************
*
*
*                Copyright 2006, Huawei Technologies Co. Ltd.
*                            ALL RIGHTS RESERVED
*
*-------------------------------------------------------------------------------
*
*                              dns_cfg.h
*
*  Project Code: VISP1.5
*   Module Name: DNSC Module  
*  Date Created: 2002-11-25
*        Author: Sun Yanfeng(32084)
*   Description: 此文件主要负责DNS的数据配置
*
*-------------------------------------------------------------------------------
*  Modification History
*  DATE        NAME                 DESCRIPTION    
*  -----------------------------------------------------------------------------
*  2002-11-25  Sun Yanfeng(32084)         Creat the first version.
*
*******************************************************************************/

#ifndef _DNS_CFG_H_
#define _DNS_CFG_H_

/*In case of the C++.*/
#ifdef  __cplusplus
extern "C" {
#endif
extern ULONG  DNS_QueryEnable(ULONG ulNoFlag);
extern ULONG  DNS_Core_SetServer(SetServer_S *pstSetServer);
extern ULONG  DNS_SetServer(SetServer_S *pstSetServer);
extern ULONG  DNS_GetServerList(DomainInfo_S *pstDomainInfo);
extern ULONG  DNS_Core_SetDomainSuffix(ULONG  ulNoFlag , CHAR  szDomainName[DNS_DOMAIN_LIST_NAME_LENGTH + 1] ,ULONG  ulDomainNameFlag);
extern ULONG  DNS_SetDomainSuffix(ULONG  ulNoFlag , CHAR  szDomainName[DNS_DOMAIN_LIST_NAME_LENGTH + 1] ,ULONG  ulDomainNameFlag);
extern ULONG  DNS_GetDomainList(DomainInfo_S *pstDomainInfo);
extern ULONG  DNS_ClearDynamicHost(ULONG ulProtocolFlag);
extern ULONG  DNS_OpenDynamicHostTbl(ULONG *ulDnsCacheHandle,WAITLIST_GETNEXTFUNC pfGetNextFunc);
extern ULONG  DNS_CloseDynamicHostTbl(ULONG ulDnsCacheHandle);
extern DNS_CACHE_S*  DNS_GetDynamicHostFirst(ULONG ulDnsCacheHandle);
extern ULONG   DNS_GetDynamicHostNext(ULONG ulDnsCacheHandle,DNS_CACHE_S **ppstDnsCache);
extern ULONG  DNS_Core_SetServerAddr(ULONG ulNoFlag, ULONG ulServerIp);
extern ULONG  DNS_SetServerAddr(ULONG ulNoFlag, ULONG ulServerIp);

extern ULONG DNS_SetDebugSwitch(ULONG ulswitch);
extern ULONG DNS_GetDebugSwitch(ULONG *pulswitch);

extern struct hostent *DNSC_GetHost_PT(CHAR *pName,LONG laddrtype, ULONG ulPolicy); /*Modified by x36317 for D00030, 2005-12-05*/
extern ULONG   DNS_ClearDynamicHost_PT(ULONG ulProtocolFlag);
extern DNS_CACHE_S* DNS_GetDynamicHostFirst_PT(ULONG ulDnsCacheHandle);
extern ULONG DNS_GetDynamicHostNext_PT(ULONG ulDnsCacheHandle,DNS_CACHE_S **ppstDnsCache);

extern ULONG DNS_SetTos(UCHAR ucTos);
extern ULONG DNS_GetTos(UCHAR *pucTos);

/*In case of the C++.*/
#ifdef  __cplusplus
}
#endif


#endif 

