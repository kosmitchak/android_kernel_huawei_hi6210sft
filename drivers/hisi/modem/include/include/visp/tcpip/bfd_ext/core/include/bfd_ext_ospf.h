/*******************************************************************************
*
*
*                Copyright 2006, Huawei Technologies Co. Ltd.
*                            ALL RIGHTS RESERVED
*
*-------------------------------------------------------------------------------
*
*                              bfd_ext_ospf.h
*
*  Project Code: VISPV100R007
*   Module Name: BFD EXTERN
*  Date Created: 2008-07-24
*        Author: qinyun
*   Description: 
*
*-------------------------------------------------------------------------------
*  Modification History
*  DATE            NAME              DESCRIPTION    
*  -----------------------------------------------------------------------------
*  2008-07-24      qinyun            Create
*
*******************************************************************************/
#ifndef _BFD_EXT_OSPF_H_
#define _BFD_EXT_OSPF_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagBfdOspfRuleEntry
{
    struct tagBfdOspfRuleEntry   *pstNextRule;        
    ULONG   ulRuleType; 
    ULONG   ulIfIdOrProcId; 
    UCHAR   szIfFullName[BFD_EXT_MAX_INTF_NAME_LEN + 1]; 
    ULONG   ulMinTxInterval;              /* BFD期望的发送间隔 */ 
    ULONG   ulMinRxInterval;              /* BFD的本地接收能力 */ 
    ULONG   ulDetectMult ;                /* BFD的本地检测倍数 */ 
    USHORT  usEnable ;                    /* 使能OSPF联动标志:1-Enable,0-Disable */ 
    USHORT  usBlock ;                     /* 使能接口Block功能标志:1-Block,0-Unblock */ 
}BFD_OSPF_RULE_ENTRY_S;

typedef struct tagBfdOspfRuleTable
{
    /*Modified by liangjicheng 00103192, NA, 2011/10/27   问题单号: REQ-201108278323-BFD-SPEC */
    /* ULONG   ulBlockNumber;    */        
    /* ULONG   ulRuleNumber;     */  
    ULONG   ulProcNumber;
    ULONG   ulIntfNumber;
    /*End of Modified by liangjicheng 00103192, 2011/10/27   问题单号: REQ-201108278323-BFD-SPEC */
    struct tagBfdOspfRuleEntry   stRelatingRule;        
}BFD_OSPF_RULE_TABLE_S;

/*************************************/
/*          过滤器结构体             */
/*************************************/
typedef struct tagBfdForOspfFilter
{ 
    ULONG ulNetworkType;
    ULONG ulProcIndex;
    ULONG ulIfIndex;
    ULONG ulFilterEnableFlag;/*进程号，VRF ID，接口索引有效无效标志*/
}BFD_FOR_OSPF_FILTER_S;

extern VOID *Extern_BFD_GetNextRuleByFilter(VOID *pFilter,VOID *pCurEntry);
extern ULONG Extern_BFD_AddOspfRule(ULONG ulNetworkType,ULONG ulProcIndex,UCHAR *pszIfName,ULONG ulMinTxInterval,
                                             ULONG ulMinRxInterval,ULONG ulDetectMult,USHORT usEnable,USHORT usBlock);
extern ULONG Extern_BFD_DelOspfRule(ULONG ulNetworkType,ULONG ulProcIndex,UCHAR *pszIfName);
extern ULONG Extern_BFD_OSPF_NodeAddFun(ULONG ulIfIndex,ULONG ulProcIndex,ULONG ulInstanceId,
                                            ULONG ulRouterId,ULONG *pSrcIp,ULONG *pDstIp,ULONG ulNetworkType);
extern ULONG Extern_BFD_OSPF_NodeDelFun(ULONG ulIfIndex,ULONG ulProcIndex,ULONG ulInstanceId,
                                            ULONG ulRouterId,ULONG ulNetworkType);
extern ULONG Extern_BFD_OSPF_ProcShutDownFun(ULONG ulProcIndex,ULONG ulNetworkType);
extern ULONG Extern_BFD_FindOspfRelation(ULONG *pulSessionId,ULONG ulIfIndex,ULONG ulProcId,
                                            ULONG ulInstanceId,ULONG ulRouterId,ULONG ulNeworkType);
extern VOID Extern_BFD_OSPF_DelDataOnly( BFD_RELATION_TABLE_S *pstBfdRelationTable);
extern VOID * Extern_BFD_GetNextOspfRelationByFilter(VOID *pFilter,VOID *pCurEntry);

/*Begin BC3D02903 liangjicheng 2010-04-19 */
extern VOID Extern_BFD_ProcOSPFRuleUpdate();
/*End   BC3D02903 liangjicheng 2010-04-19 */

#ifdef __cplusplus
}
#endif

#endif


