/*******************************************************************************
*
*
*                Copyright 2006, Huawei Technologies Co. Ltd.
*                            ALL RIGHTS RESERVED
*
*-------------------------------------------------------------------------------
*
*                              bfd_ext_com.h
*
*  Project Code: VISPV100R007
*   Module Name: BFD EXTERN
*  Date Created: 
*        Author: 
*   Description: 
*
*-------------------------------------------------------------------------------
*  Modification History
*  DATE         NAME         DESCRIPTION    
*  -----------------------------------------------------------------------------
*  2008-07-24   qinyun       Create
*  2008-11-03   Q62011       Add for BC3D00658, BFD_OSPF_RULE_TYPE_INTF,
*                            BFD_OSPF_RULE_TYPE_PROC转移到api头文件中供用户使用
*******************************************************************************/
#ifndef _BFD_EXT_COM_H_
#define _BFD_EXT_COM_H_

#ifdef __cplusplus
extern "C" {
#endif
/*************************************/
/*              宏定义               */
/*************************************/
#define SID_EXT_BFD_OSPF_RULE_S 1
#define SID_EXT_BFD_RTM_NODE_S  2

/*#define BFD_OSPF_MAX_BLOCK_NUM      32   */
/*#define BFD_OSPF_MAX_RULE_NUM       32   */
/*#define BFD_STATIC_MAX_RELATION_NUM 32   */
/*#define BFD_STATIC_ONE_SESS_MAX_NUM 1    */
#define BFD_OSPF_MAX_BLOCK_NUM      64
#define BFD_OSPF_MAX_RULE_NUM       64
#define BFD_STATIC_MAX_RELATION_NUM  5*1024
#define BFD_STATIC_ONE_SESS_MAX_NUM  5*1024

#define BFD_STATIC_DETECT_TIME_DEFAULT  10*1000
#define BFD_STATIC_DETECT_TIME_MAX      50*1000
#define BFD_STATIC_DETECT_MAX_SECS      50
#define BFD_STATIC_DETECT_MIN_SECS      1

/*与BFD_MAX_SESSION_ID保持一致，若BFD模块的会话个数处理有修改则需要相应修改本模块的处理*/
#define BFD_EXT_MIN_SESSION_ID             1          /* BFD最小session id*/
#define BFD_EXT_MAX_SESSION_ID          2000          /* BFD最大session id*/

#define BFD_EXT_TRUE                       1
#define BFD_EXT_FALSE                      0

/*标志位*/
#define BFD_EXT_YES     1
#define BFD_EXT_NO      0

typedef struct tagBFD_ADDR6_S
{
    ULONG     u6_uladdr[4];
}BFD_ADDR6_S;

#define HASH_BFD1(ulAddr) \
(((USHORT)((ULONG)(ulAddr)>>16)) ^ ((USHORT)(ulAddr)))

#define HASH_BFD2(pAddr6) \
     (HASH_BFD1(((BFD_ADDR6_S *)(pAddr6))->u6_uladdr[0]) ^ HASH_BFD1(((BFD_ADDR6_S *)(pAddr6))->u6_uladdr[1]) ^ \
      HASH_BFD1(((BFD_ADDR6_S *)(pAddr6))->u6_uladdr[2]) ^ HASH_BFD1(((BFD_ADDR6_S *)(pAddr6))->u6_uladdr[3]))

/*modify by q62011 for ospfv3 */
typedef enum tagBFD_EXT_TABLE_NUM_E
{
    BFD_EXT_TABLE_IPV4 = 0,
    BFD_EXT_TABLE_IPV6,
    BFD_EXT_TABLE_MAX_NUM
}BFD_EXT_TABLE_NUM;

/* End of Added by qinyun62011, 2011/2/9   问题单号:V2R3C03-ROTUE-MERGE */

typedef struct tagBFD_ENTRY_S
{
    struct tagBFD_ENTRY_S   *pstNextEntry;        
} BFD_ENTRY_S;

typedef struct tagBFD_STATIC_NODE_S
{
    struct tagBFD_STATIC_NODE_S   *pstNextNode;  
    struct tagBFD_STATIC_NODE_S   *pstPreNode;
    struct tagBFD_STATIC_NODE_S   *pstLeft; /* 左子树 */
    struct tagBFD_STATIC_NODE_S   *pstRight; /* 右子树 */    
    VOID   *pTable;        
    /* wangjiaqi modify for ipv6 bfd 20131125 begin */
    ULONG   ulDstIp[4]; 
    ULONG   ulPreLen; 
    ULONG   ulNextHop[4]; 
    /* wangjiaqi modify for ipv6 bfd end */
    ULONG   ulIfIndex; 
    ULONG   ulTimeId;  
    ULONG   ulVrfIndex; /* Vrf Index V4,V6共用 */
    ULONG   ulIpVersion;/* 区分是V4还是V6的HASH节点 */
} BFD_STATIC_NODE_S ;

/*modify by q62011 for ospfv3 */
typedef struct tagBFD_OSPF_NODE_S
{
    struct tagBFD_OSPF_NODE_S   *pstNextNode;        
    VOID   *pTable;        
    ULONG   ulInstanceId;     /*OSPFV3有效*/      
    ULONG   ulRouterId;        /*邻居的Route ID(网络序)*/      
    ULONG   ulIfIndex;            
    ULONG   ulProcId;           /*OSPF/OSPFV3实例号*/
} BFD_OSPF_NODE_S ;
/* End of Added by qinyun62011, 2011/2/9   问题单号:V2R3C03-ROTUE-MERGE */

typedef union unBFD_RELATION_PARA_U
{
    struct tagOSPFTbl
    {
        ULONG   ulNumber;       
        BFD_OSPF_NODE_S stNodeHd;
    }Bfd_For_Ospf;
    struct tagStaticTbl
    {
        ULONG   ulNumber;       
        ULONG   ulProcId;           /*RTM实例号*/
        BFD_STATIC_NODE_S stNodeHd;
    }Bfd_For_Static;
}BFD_RELATION_PARA_U;

typedef struct tagBFD_RELATION_TABLE_S
{
    ULONG   ulProcType;         /*路由协议类型，支持OSPF、RTM */
    ULONG   ulBfdSessId;        /*对应的BFD会话索引，初始化后任何情况都不应该清零*/
    ULONG   ulNetworkType;       /*类型，0-IPv4联动,1-IPv6联动 */
    BFD_RELATION_PARA_U  unRelationPara;
} BFD_RELATION_TABLE_S;


typedef struct tagBFD_RELATION_HASHTABLE_S
{
    struct tagBFD_STATIC_NODE_S   *pstLeft; /* 左子树 */
    struct tagBFD_STATIC_NODE_S   *pstRight; /* 右子树 */
}BFD_RELATION_HASHTABLE_S;

/* Begin:VISP1.7C03 VRF wangchengyang,2009-02-12 */
typedef struct tagBFD_VRFRELATION_TABLE_S
{
    ULONG ulVrfIndex;                              /* 对应VRF索引 */
    /* 由于最大BFD会话ID通过预配置设置,即最大值是可变的,为了减少对使用会话ID不是预配置最大值的SGSN产品的内存影响
       所以采用动态内存申请方式,即这里使用指针而不是数组 */
    BFD_RELATION_TABLE_S **ppstBfdRelationTable;   /* 会话关联表 */
}BFD_VRFRELATION_TABLE_S;
/* End:VISP1.7C03 VRF wangchengyang,2009-02-12 */

typedef struct tagOspfMsg
{
    /*modify by q62011 for ospfv3 */
    /*ULONG ulRouterId;           OSPF路由表索引ID
    ULONG ulIfIndex;            OSPF路由出接口
    ULONG ulLocalIp;            本端地址
    ULONG ulPeerIp;             对端地址*/
    ULONG ulNbrKey;                /*OSPF邻居查找关键字，V2是Route ID，V3是Instace ID*/
    ULONG ulIfIndex;               /*OSPF路由出接口*/
    ULONG ulLocalIp[LEN_4];        /*本端地址*/
    ULONG ulPeerIp[LEN_4];         /*对端地址*/
    ULONG ulIpType;                /*地址类型，BFD_OVER_IPV4(0)-IPv4地址,BFD_OVER_IPV6(1)-IPv6地址*/
    /* End of Added by qinyun62011, 2011/2/9   问题单号:V2R3C03-ROTUE-MERGE */
}BFD_TO_OSPF_MSG_S;/*与BFD_NTY_OSPF保持一致*/
typedef struct tagRtmMsg
{
    ULONG ulDstIp[LEN_4];       /*静态路由目的地址*/
    ULONG ulPreLen;             /*静态路由掩码长度*/
    ULONG ulNextHop[LEN_4];     /*静态路由下一跳IP地址*/
    ULONG ulOutIndex;           /*静态路由出接口索引*/
    ULONG ulIpType;             /*地址类型，BFD_OVER_IPV4(0)-IPv4地址,BFD_OVER_IPV6(1)-IPv6地址*/
}BFD_TO_RTM_MSG_S;/*与BFD_TO_RTM_DATA保持一致*/

typedef ULONG(*BFD_IFNET_GET_INDEX_BY_NAME)( CHAR *pszIfName, ULONG *pulIfIndex);
typedef ULONG(*BFD_IFNET_GET_NAME_BY_INDEX)( ULONG ulIfIndex, CHAR *pszIfName);

extern ULONG Extern_BFD_Init(VOID);
extern ULONG Extern_BFD_RegStatNotifyHook( BFD_RM_NOTIFY_HOOK_FUNC pfStatNotifyFunc );
extern ULONG Extern_BFD_RegOspfAndRtmType( ULONG ulOspf, ULONG ulRtm );
extern ULONG Extern_BFD_NotifyHook(ULONG ulSessionID, ULONG ulState,ULONG ulDstIP, ULONG ulSrcIP, 
                                         ULONG ulInIfIndex, ULONG ulAppGroup);
extern ULONG Extern_BFD_NotifyVrfHook(BFD_NOTIFY_BYVRF_S *pstBfdNotifyByVrf);
extern ULONG Extern_BFD6_NotifyHook(BFD6_NOTIFY_S *pstBfdNotify);

extern ULONG Extern_BFD_SetDbgSwitch(ULONG ulNetworkType, ULONG ulSwitch);
extern ULONG Extern_BFD_CheckDbgSwitch(ULONG ulNetworkType, ULONG ulRelaType);
extern VOID Extern_BFD_RemoveEntry(BFD_ENTRY_S *pstPrevEntry,BFD_ENTRY_S *pstCurEntry);
extern VOID Extern_BFD_InsertEntry(BFD_ENTRY_S *pstCurEntry,BFD_ENTRY_S *pstNewEntry);
extern VOID Extern_BFD_StateNotify(ULONG ulNotifyType, ULONG ulSessionID, ULONG ulVrfIndex, 
                                                        ULONG ulDstIP, ULONG ulSrcIP, ULONG ulInIfIndex, ULONG ulNewSessionID);
extern CHAR* VOS_IpAddrToStr(ULONG ulAddr, CHAR * szStr);
extern LONG VOS_strcmp(const CHAR *,const CHAR *);
extern VOID* TCPIP_Malloc(ULONG ulInfo, ULONG ulSize);
extern ULONG TCPIP_Free(VOID* pBuf);
extern VOID BFD_Begin_PT();
extern VOID BFD_End_PT();

extern ULONG Extern_BFD6_HashFunc(ULONG *ulDestIp,ULONG ulPrefixLen,ULONG *ulNextHop, ULONG ulVrfIndex);
extern ULONG Extern_BFD_HashFunc(ULONG *ulDestIp,ULONG ulPrefixLen,ULONG *ulNextHop, ULONG ulVrfIndex);
extern VOID Extern_BFD_AddToHashRelation(BFD_RELATION_HASHTABLE_S * pstHashNode, BFD_STATIC_NODE_S *pstBfdStNode);
extern VOID Extern_BFD_DelFromHashRelation(BFD_RELATION_HASHTABLE_S * pstHashNode, BFD_STATIC_NODE_S *pstBfdStNode);
extern VOID Extern_BFD_DelFromRelationTable(BFD_STATIC_NODE_S *pstBfdStNode);
#ifdef __cplusplus
}
#endif

#endif


