/*******************************************************************************
*
*
*                Copyright 2006, Huawei Technologies Co. Ltd.
*                            ALL RIGHTS RESERVED
*
*-------------------------------------------------------------------------------
*
*                              bfd_sh.h
*
*  Project Code: VISPV100R006
*   Module Name: BFD SCM
*  Date Created: 2006-12-04
*        Author: Wangchengyang
*   Description: BFD Shell 函数声明以及宏定义头文件
*
*-------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME             DESCRIPTION    
*  -----------------------------------------------------------------------------
*  2006-12-07   Wangchengyang    Create
*  2009-01-20   wangchengyang    VISP1.7C03 VRF,多个函数声明增加入参
*******************************************************************************/
#ifndef _BFD_SCM_H_
#define _BFD_SCM_H_

#ifdef __cplusplus
extern "C" {
#endif

/*Modified by wangchengyang-xujun62830-57575 for VISPV1R8C01, 2009/4/9 */
/*
ULONG BFD_Core_SessionCreate(ULONG *pulDstIP, ULONG *pulSrcIP, ULONG ulOutIfIndex, ULONG * pulSessionID, ULONG ulVrfIndex,ULONG ulIpVer,ULONG ulStartId);
*/
/*End of Modified by wangchengyang-xujun62830-57575, 2009/4/9 */

typedef struct tagBFD_CORE_SESSION_CREATE_S { 
    ULONG ulIpVer;
    ULONG ulDstIP[4];
    ULONG ulSrcIP[4];
    ULONG ulOutIfIndex;
    ULONG ulVrfIndex;
    ULONG ulStartId;
    ULONG ulBfdType;
    /* Add for V2R3C05, by b0177000/y00176567, at 2011-06-03. 修改原因: V2R3C05  增加BFD创建的协议类型字段 */   
    ULONG ulBfdProtocol;
    /*Added by zhongyu206043, 增加BFD中的TOS值设置 for S.BFD.1.1, 2012/10/11 */
    UCHAR ucTos;
    UCHAR ucRes[3]; /* 保证字节对齐 */
    ULONG ulRemoteDiscr;
    /* Added by fengjing209023 for S-BFD-MultiSession.CFG.001 置为BFD_ON会生效，其他默认不生效 */
    ULONG ulMultiHopFlag;       /* 如果置上该标记，则在多跳下也是用3784，否则默认4784 */
    /* Added end */
}BFD_CORE_SESSION_CREATE_S;

ULONG BFD_Core_SessionCreate(BFD_CORE_SESSION_CREATE_S *pstSession, ULONG * pulSessionID);
ULONG BFD_Core_SessionCreateByParam(BFD_CORE_SESSION_CREATE_S *pstSession, BFD_SESSION_PARA_S *pSessPara,ULONG * pulSessionID);
/*End of Modified by qinyun62011, 2013/10/17 */

ULONG BFD_SessionCreateBySlave(ULONG ulDstIP, ULONG ulSrcIP, ULONG ulOutIfIndex, ULONG ulSessionID,ULONG ulIpVer,ULONG ulVrfIndex);

ULONG BFD_Core_SessionDelete(ULONG ulSessionID,ULONG ulVrfIndex);

ULONG BFD_Core_DeleteSingleApp(ULONG ulSessionID, ULONG ulAppID,ULONG ulVrfIndex);

ULONG BFD_Core_SessionParaUpdate(ULONG ulSessionID, BFD_SESSION_PARA_S *pSessPara,ULONG ulVrfIndex, ULONG ulIpVer);

ULONG BFD_Core_SetSingleAppToSession(ULONG ulSessionID, ULONG ulAppID,ULONG ulVrfIndex);

ULONG BFD_Core_SetAppsToSession(ULONG ulSessionID, ULONG ulAppGroup,ULONG ulVrfIndex);

ULONG BFD_Core_SetWtrTime(ULONG ulSeconds);

ULONG BFD_SCM_ProcZeroDiscr (ULONG ulSessDisc, BFD_PACKET_S *pstPkt,ULONG ulVrfIndex);

ULONG BFD_SCM_ProcPkt( BFD_SESS_ENTRY_S *pstSessEntry ,  BFD_PACKET_S *pstPkt );

ULONG BFD_SCM_ProcAsyncPkt( BFD_SESS_ENTRY_S *pstSessEntry , BFD_PACKET_S *pstPkt);

ULONG BFD_SCM_ProcDemandPkt ( BFD_SESS_ENTRY_S *pstSessEntry , BFD_PACKET_S *pstPkt );

ULONG BFD_Core_SessionAdminister(ULONG ulSessionID, BOOL_T bShutdown,ULONG ulVrfIndex);

ULONG BFD_Core_GetSessionInfo(ULONG ulSessionID, BFD_SESSION_INFO_S *pSess, ULONG ulVrfIndex);

/* wangjiaqi modify for ipv6 bfd 20131114 begin */
ULONG BFD6_Core_GetSessionInfo(ULONG ulSessionID, BFD6_SESSION_INFO_S *pSess, ULONG ulVrfIndex);

ULONG BFD_Core_GetNextSessionID(ULONG ulSessionID,ULONG ulVrfIndex, ULONG ulIpVersion);
/* wangjiaqi modify for ipv6 bfd end */

ULONG BFD_Core_SetDebugSwitch(ULONG ulSessionID, ULONG ulDebug,ULONG ulVrfIndex);

ULONG BFD_Core_GetDebugSwitch(ULONG ulSessionID, ULONG *pulDebug,ULONG ulVrfIndex);

ULONG BFD_Core_DemandQueryByApp(ULONG ulSessionID,ULONG ulVrfIndex);

/* wangjiaqi modify for ipv6 bfd 20131114 begin */
ULONG BFD_Core_SessionCountClr(UCHAR ucSpecSessFlag,ULONG ulLocalDiscr,ULONG ulVrfIndex, ULONG ulIpVersion);
/* wangjiaqi modify for ipv6 bfd end */

ULONG BFD_Core_Hook_Register(BFD_NOTIFY_HOOK_FUNC pBfdNotifyHookFunc);

ULONG BFD_Core_Hook_UnRegister(BFD_NOTIFY_HOOK_FUNC pBfdNotifyHookFunc);

ULONG BFD_Core_GetSessionIdByName(UCHAR *pszSessName,ULONG *pulSessionId,ULONG ulVrfIndex);

ULONG BFD_Core_GetNameBySessionId(ULONG ulSessionId,UCHAR *pszSessName,ULONG ulLength,ULONG ulVrfIndex);

ULONG BFD_Core_AddSessionName(ULONG ulSessionID,UCHAR *pszSessName,ULONG ulVrfIndex);

ULONG BFD_Core_AddSessionDesc(ULONG ulSessionID,UCHAR *pszSessDesc,ULONG ulVrfIndex);

ULONG BFD_Core_GetSessionConfig(ULONG ulSessionId,BFD_SESSION_CONFIG_INFO_S *pstSessConfig,ULONG ulVrfIndex);

ULONG BFD_Core_GetSessionAllInfo(ULONG ulSessionId,BFD_SESSION_ALL_INFO_S *pstSessInfo,ULONG ulVrfIndex);

ULONG BFD_Core_GetSessionCount(ULONG ulSessionId,BFD_SESSION_COUNT_S *pstCount,ULONG ulVrfIndex);

ULONG BFD_Core_GetSessionBriefInfo(ULONG ulSessionId,BFD_SESSION_BRIEF_INFO_S *pstBriefInfo,ULONG ulVrfIndex);

ULONG BFD_Core_GetNextSessionByFilter(ULONG ulEntryWaitList,ULONG *pulVrfIndex,ULONG *pulSessionId);

ULONG BFD_Core_SessionDeleteNoQue(ULONG ulSessionID,ULONG ulVrfIndex);

ULONG BFD_Core_VrfHook_Register(BFD_NOTIFY_BYVRF_HOOK_FUNC pBfdNotifyByVrfHookFunc);

ULONG BFD_Core_VrfHook_UnRegister(BFD_NOTIFY_BYVRF_HOOK_FUNC pBfdNotifyByVrfHookFunc);

/* wangjiaqi modify for ipv6 bfd 20131114 begin */
VOID BFD_Core_ShowSessionCountByVrf(ULONG ulVrfIndex, ULONG ulIpVersion);
VOID BFD_Core_ShowSessionInfoByVrf(ULONG ulVrfIndex, ULONG ulIpVersion);
/* wangjiaqi modify for ipv6 bfd end */
extern ULONG BFD_GetVrfNameByIndex(CHAR *pszVrfName,ULONG ulVrfIndex);

/*Added by wangchengyang-xujun62830-57575 for VISPV1R8C01, 2009/4/14 */
ULONG BFD6_Core_GetSessionConfig(ULONG ulSessionId,BFD6_SESSION_CONFIG_INFO_S *pstSessConfig,ULONG ulVrfIndex);

ULONG BFD6_Core_GetSessionAllInfo(ULONG ulSessionId,BFD6_SESSION_ALL_INFO_S *pstSessInfo,ULONG ulVrfIndex);

ULONG BFD6_Core_GetNextSessionByFilter(ULONG ulEntryWaitList,ULONG *pulVrfIndex,ULONG *pulSessionId);

ULONG BFD_Core_RecvPktWithInfo(BFD_CONTROL_PACKET_S *pBfdPkt,ULONG ulPaketLen,BFD_PKTINFO_S *pstPktInfo);

ULONG BFD6_RecvPacket(CHAR *pBFD6Packet, ULONG ulPktLenth, BFD_PKTINFO_S *pstRcvPktInfo);

ULONG BFD6_Core_Hook_Register(BFD6_NOTIFY_HOOK_FUNC pBfd6NotifyHookFunc);

ULONG BFD6_Core_Hook_UnRegister(BFD6_NOTIFY_HOOK_FUNC pBfd6NotifyHookFunc);

ULONG BFD_Core_SetFirstNegoTimes(ULONG ulTimes);

ULONG BFD_Core_GetIpVerBySessionId(ULONG ulSessionId,ULONG ulVrfIndex,ULONG *pulIpVer);

/* End of Added by wangchengyang-xujun62830-57575, 2009/4/14 */
VOID BFD_Core_SetRelationFlage(BOOL_T bFlag);
ULONG BFD_Core_SetSessionTosByVrf(ULONG ulSessionID, ULONG ulVrfIndex, UCHAR ucTos);

/*Add by q62011 for BC3D02224*/
ULONG BFD_Core_GetSessionInfoNoIp(ULONG ulSessionId, BFD_SESSION_PUB_INFO_S *pstSessInfo,ULONG ulVrfIndex);

VOID BFD_NotifyTimerOut(ULONG ulVrfBfdIndex,ULONG ulTimerId);
ULONG BFD_Core_SetNotifyTime(ULONG ulTimes);
ULONG BFD_Core_SetNotifyTimeFlag( ULONG ulFlag);
ULONG BFD_Core_SetDontSendAdminDownPktFlag(ULONG ulSessionID, ULONG ulVrfIndex);

/* wangjiaqi modify for ipv6 bfd 20131119 begin */
/*******************************************************************************
*    Func Name: BFD_Core_GetSessionStatics
* Date Created: 2013-11-19
*       Author: wangjiaqi
*  Description: get BFD session statics
*        Input: ULONG ulVrfIndex
                ULONG ulSessionId
*       Output: BFD_STATISTIC_S *pstStat
*       Return: 
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2013-11-19   wangjiaqi               Create    
*
*******************************************************************************/
ULONG BFD_Core_GetSessionStatics(ULONG ulVrfIndex ,ULONG ulSessionId, BFD_STATISTIC_S * pstStat);

/*******************************************************************************
*    Func Name: BFD_Core_ClrSessionStatics
* Date Created: 2013-11-19
*       Author: wangjiaqi
*  Description: show BFD session statics
*        Input: ULONG ulVrfIndex
                ULONG ulSessionId
*       Output: NA
*       Return: 
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2013-11-19   wangjiaqi               Create    
*
*******************************************************************************/
ULONG BFD_Core_ClrSessionStatics(ULONG ulVrfIndex ,ULONG ulSessionId);

/*******************************************************************************
*    Func Name: BFD_Core_GetBfdVersion
* Date Created: 2013-11-14
*       Author: wangjiaqi
*  Description: get BFD ip version
*        Input: ULONG ulVrfIndex
                ULONG ulSessionId
*       Output: ULONG *pulVersion: BFD_OVER_IPV4/BFD_OVER_IPV6
*       Return: 
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2013-11-14   wangjiaqi               Create    
*
*******************************************************************************/
ULONG BFD_Core_GetBfdSessionVersion(ULONG ulVrfIndex, ULONG ulSessionId, ULONG *pulVersion);
/* wangjiaqi modify for ipv6 bfd end */

/* wangjiaqi modify for ipv6 bfd 20131121 begin */
/*******************************************************************************
*    Func Name: BFD_Core_CheckCapability
* Date Created: 2013-11-21
*       Author: wangjiaqi
*  Description: check bfd capability
*        Input: ULONG ulVrfIndex
                ULONG ulSessionId
*       Output: 
*       Return: 
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2013-11-21   wangjiaqi               Create    
*
*******************************************************************************/
ULONG BFD_Core_CheckCapability(ULONG ulVrfIndex, ULONG ulSessionId);
/* wangjiaqi modify for ipv6 bfd end */

/*******************************************************************************
*    Func Name: BFD_Core_CheckCapability
* Date Created: 2013-11-21
*       Author: wangjiaqi
*  Description: check bfd capability
*        Input: ULONG ulVrfIndex
                ULONG ulSessionId
*       Output: 
*       Return: 
*      Caution: 
*------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME                    DESCRIPTION
*  ----------------------------------------------------------------------------
*  2013-11-21   wangjiaqi               Create    
*
*******************************************************************************/
ULONG BFD_Core_CheckCapability2(ULONG ulVrfIndex, ULONG ulSessionId);

#ifdef __cplusplus
}
#endif

#endif

