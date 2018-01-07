

#ifndef __IMM_MEM_H__
#define __IMM_MEM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "MemoryMap.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif




#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define FEATURE_IMM_MEM_DEBUG             1

/* IMM 内存池可能的最大长度 */
#define IMM_MEM_BLK_MAX_BYTE_LEN        (3800)

/* IMM 控制内存池预留块数 */
#define IMM_MEM_CTRL_MEM_CNT           (418)
/* IMM 内存池可能的最多档位 */
#define IMM_MEM_BLK_MAX_CLUSTER_NUM     (8)

/* IMM 控制头添加内存指针 */
#define IMM_ADD_CTL_HEAD_MEM(pstImmHead, pMem)          ((pstImmHead)->p_Imm_Ttf_Mem = (pMem))

#define IMM_GET_4BYTE_ALIGN_VALUE(ulSize)   ( ((ulSize) + 0x03) & (~0x03) )

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
 枚举名    : IMM_BOOL_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : IMM统一布尔类型枚举定义
*****************************************************************************/
enum IMM_BOOL_ENUM
{
    IMM_FALSE                            = 0,
    IMM_TRUE                             = 1,
    IMM_BOOL_BUTT
};
typedef unsigned char   IMM_BOOL_ENUM_UINT8;

/*****************************************************************************
 枚举名    : IMM_RSLT_CODE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : IMM 统一返回值枚举定义
*****************************************************************************/
enum IMM_RSLT_CODE_ENUM
{
    IMM_SUCC                             = 0,
    IMM_FAIL                             = 1,
};
typedef unsigned long   IMM_RSLT_CODE_ENUM_UINT32;


enum IMM_MEM_USED_INFO_TRIG_TYPE_ENUM
{
    IMM_MEM_TRIG_TYPE_ALERT                      = 0x0,     /*内存超门限告警*/
    IMM_MEM_TRIG_TYPE_ALLOC_FAIL,                           /*内存申请失败*/
    IMM_MEM_TRIG_TYPE_LEAK,                                 /*检查到有内存泄露*/
    IMM_MEM_TRIG_TYPE_BUTT
};
typedef unsigned char IMM_MEM_USED_INFO_TRIG_TYPE_ENUM_UINT8;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/




enum MEM_BLK_STATE_ENUM
{
    MEM_BLK_STATE_FREE,       /*该内存未申请过或已释放*/
    MEM_BLK_STATE_ALLOC,      /*该内存已申请*/
    MEM_BLK_STATE_BUTT
};
typedef unsigned long MEM_BLK_STATE_ENUM_UINT32;



typedef struct
{
    MEM_BLK_STATE_ENUM_UINT32           enMemStateFlag;
    unsigned long                       ulAllocTick;        /* CPU tick when alloc or free */
    unsigned short                      usAllocFileID;      /* File ID when alloc or free */
    unsigned short                      usAllocLineNum;     /* File Line when alloc or free */
    unsigned short                      usTraceFileID;      /* File ID when traced */
    unsigned short                      usTraceLineNum;     /* File Line when traced */
    unsigned long                       ulTraceTick;        /* CPU tick when traced */
} IMM_BLK_MEM_DEBUG_STRU;



enum IMM_MEM_POOL_ID_ENUM
{
    IMM_MEM_POOL_ID_SHARE = 0,              /* A核共享内存池ID */
    IMM_MEM_POOL_ID_EXT,                    /* A核外部内存池ID */

    IMM_MEM_POOL_ID_BUTT
};
typedef unsigned char IMM_MEM_POOL_ID_ENUM_UINT8;



enum IMM_MEM_TYPE_ID_ENUM
{
    IMM_MEM_TYPE_ID_SHARE = 0,              /* A核共享内存池 */
    IMM_MEM_TYPE_ID_EXT,                    /* 来自C核的跨核内存 */

    IMM_MEM_TYPE_ID_BUTT
};
typedef unsigned char IMM_MEM_TYPE_ID_ENUM_UINT8;



typedef struct
{
    IMM_MEM_TYPE_ID_ENUM_UINT8          enMemType;          /* 内存类型 */
    IMM_MEM_POOL_ID_ENUM_UINT8          enPoolId;           /* 本内存块是是属于哪个内存池 */
    unsigned char                       ucClusterId;        /* 本内存是属于哪一个级别 */
    unsigned char                       ucMagicNum;         /* 内存控制块魔术字 */
    IMM_BLK_MEM_DEBUG_STRU             *pstDbgInfo;         /* 指向内存控制块的DEBUG信息存储块 */

    unsigned char                      *pstMemBlk;          /* 存放数据的指针，物理上指向结构体的连续内存 */
}IMM_MEM_STRU;



typedef struct
{
    /*本级数的IMM_MEM_STRU总个数*/
    unsigned short                      usTotalCnt;

    /*本级数的IMM_MEM_STRU空闲个数*/
    unsigned short                      usFreeCnt;

    /*本级内存的内容长度*/
    unsigned short                      usLen;

    unsigned char                       aucRsv[2];

    /*记录IMM_MEM_ST可用内存索引表的栈顶元素，其内容为TTF_MEM_ST指针 */
    IMM_MEM_STRU                      **apstFreeStack;
}IMM_MEM_CLUSTER_STRU;



typedef void (*IMM_MEM_EVENT_CALLBACK)(unsigned long ulMaxClusterFreeCnt);
typedef void (*IMM_MEM_EXT_FREE_CALLBACK)(const void *pAddr);
typedef void (*IMM_MEM_USED_INFO_EVENT_RPT_CALLBACK)( unsigned char  ucPoolId, IMM_MEM_USED_INFO_TRIG_TYPE_ENUM_UINT8 enTrigType );
typedef unsigned char (*IMM_SAVE_MEM_FREE_INFO_CALLBACK)(IMM_MEM_STRU *pstImmMem);
typedef void (*IMM_SAVE_MEM_ALLOC_INFO_CALLBACK)(IMM_MEM_STRU *pstImmMem,unsigned short usFileId,unsigned short usLineNum);

typedef unsigned long (*IMM_OM_GET_SLICE_CALLBACK)(void);


typedef struct
{
    /* 标识本内存池是否使用 */
    unsigned char                       ucUsedFlag;

    /* 本内存池一共有多少级*/
    unsigned char                       ucClusterCnt;

    /* 本内存池中使用的最大用户字节 */
    unsigned short                      usMaxByteLen;

    /* 指向长度索引表的指针 */
    unsigned char                       aucLenIndexTable[IMM_MEM_BLK_MAX_BYTE_LEN];

    /* 指向级数索引表的指针 */
    IMM_MEM_CLUSTER_STRU                astClusterTable[IMM_MEM_BLK_MAX_CLUSTER_NUM];


    unsigned short                      usImmMemTotalCnt;       /* 本内存池一共有多少IMM_MEM_ST */
    unsigned short                      usImmMemUsedCnt;        /* 使用了多少IMM_MEM_ST */
    unsigned long                       ulImmMemTotalByteSize;  /* 一共有多少字节 */
    unsigned long                       ulImmMemUsedByteSize;   /* 已使用内存总大小，以字节为单位 */

    unsigned long                       ulImmMemMaxBlkCcpuUsedCnt; /* 最高档A核传给C核的库存量 */

    IMM_MEM_STRU                       *pstImmMemStStartAddr;   /* 指向ImmMemST首地址的指针 */
    IMM_MEM_EVENT_CALLBACK              pMemAllocEvent;
    IMM_MEM_EVENT_CALLBACK              pMemFreeEvent;
    IMM_MEM_EXT_FREE_CALLBACK           pMemExtFreeFunc;        /* 指向外部注册的释放函数*/
} IMM_MEM_POOL_STRU;



typedef struct
{
    unsigned short                          usSize;    /* 本级数的大小 */
    unsigned short                          usCnt;     /* 本级个数 */
}IMM_MEM_CLUSTER_CFG_INFO_STRU;



typedef struct
{
    IMM_MEM_POOL_ID_ENUM_UINT8          enPoolId;           /* 本池 ID */
    unsigned char                       ucClusterCnt;       /* 本内存池有多少级 */
    unsigned char                       aucResv[2];

    IMM_MEM_CLUSTER_CFG_INFO_STRU      *pstClusterCfgInfo;  /* 每级的具体信息 */
} IMM_MEM_POOL_CFG_INFO_STRU;
typedef struct
{
    IMM_SAVE_MEM_ALLOC_INFO_CALLBACK             pImmSaveMemAllocDebugInfoFunc;
    IMM_SAVE_MEM_FREE_INFO_CALLBACK              pImmSaveMemFreeDebugInfoFunc;
    IMM_MEM_USED_INFO_EVENT_RPT_CALLBACK         pImmMemEventRptFunc;
}IMM_MEM_REG_MNTN_FUNC_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
extern IMM_MEM_POOL_STRU                    g_astImmMemPool[IMM_MEM_POOL_ID_BUTT];
extern const unsigned char                  IMM_MEM_CTRL_POOL_ID;
extern IMM_MEM_POOL_STRU * const            g_pstImmCtrlMemPool;
extern const IMM_MEM_CLUSTER_CFG_INFO_STRU  g_astImmMemCtrlPoolClusterTableInfo[];
extern const unsigned long * const          g_pulImmMemCtrlMemSuffix;
extern const unsigned long * const          g_paulImmMemCtrlMemSpace;

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern IMM_MEM_STRU *IMM_MemCtrlHeadAlloc_Debug(unsigned short usFileID, unsigned short usLineNum);

extern IMM_MEM_STRU *IMM_MemBlkAlloc_Debug(unsigned short usFileID, unsigned short usLineNum,
                    unsigned char ucPoolId, unsigned short usLen);

extern void IMM_MemFree_Debug(IMM_MEM_STRU **pstMem);

extern void IMM_MemRegMntnFuncCallBack
(
    IMM_SAVE_MEM_ALLOC_INFO_CALLBACK             pImmSaveMemAllocDebugInfoFunc,
    IMM_SAVE_MEM_FREE_INFO_CALLBACK              pImmSaveMemFreeDebugInfoFunc,
    IMM_MEM_USED_INFO_EVENT_RPT_CALLBACK         pImmMemEventRptFunc
);

extern  unsigned long IMM_MemRegExtFreeCallBack
(
    IMM_MEM_EXT_FREE_CALLBACK           pMemExtFreeFunc
);

extern  unsigned long IMM_MemBlkInit(void);
extern  IMM_MEM_POOL_STRU *IMM_MemPoolGet(unsigned char ucPoolId);
extern  IMM_MEM_POOL_STRU *IMM_MemEventRptPoolGet(unsigned char ucPoolId);

#define IMM_MemFree(pstMem)             IMM_MemFree_Debug(&pstMem)

/* IMM MEM 获取内存池指针 */
#define IMM_MEM_GET_POOL(ucPoolId)      (&g_astImmMemPool[(ucPoolId)])

#pragma pack(0)




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of IMM.h */

