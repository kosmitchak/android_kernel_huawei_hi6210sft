/******************************************************************************

                  版权所有 (C), 2011-2031, 华为技术有限公司

 ******************************************************************************
  文 件 名   : RcmPhyInterface.h
  版 本 号   : 初稿
  作    者   : 吴翔 138719
  生成日期   : 2014年1月20日
  最近修改   :
  功能描述   : RCM和PHY的接口头文件,接口中的时间都是以32K的clock为单位
  函数列表   :
  修改历史   :
  1.日    期   : 2014年1月20日
    作    者   : 吴翔 138719
    修改内容   : 创建文件

******************************************************************************/

#ifndef __RCMPHYINTERFACE_H__
#define __RCMPHYINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "product_config.h"
#include "Hi_Config.h"
#include "vos.h"


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*最多支持的RF数目*/
#define RCM_RF_SUPPORT_NUM                      ( RCM_RF_ID_BUTT )

/*最多支持的PHY mode数目*/
#define RCM_MODE_SUPPORT_NUM                    ( RCM_MODE_ID_BUTT )

/*最多支持的MODEM数目*/
#define RCM_MODEM_SUPPORT_NUM                   ( RCM_MODEM_ID_BUTT )

/* 分配队列任务size */
#define RCM_PHY_ASSIGN_BUF_SIZE                 ( 32 )

/* 公共定时计数周期，以32K时钟为单位，必须是2的指数 */
#define RCM_CLOCK_PERIOD                        ( ((VOS_UINT32)0x1)<<31 )

/* 公共定时频率 */
#define RCM_CLOCK_FREQUENCY                     ( 32768 )

/* 无效定时 同时作为最大时长 */
#define RCM_CLOCK_INVALID_VALUE                 ( 0xffffffff )

/* 单次和累积被抢占门限,抢占统计周期无效值(非PS业务统一填此值) */
#define RCM_PRMPT_TH_INVALID_VALUE              ( RCM_CLOCK_INVALID_VALUE )

/* 时间ms转换为32K的计数 输入必须小于0x20000 ms*/
#define RCM_MsToClock( M_uhwTime )              ( (VOS_INT32)( ( ((VOS_UINT32)M_uhwTime) *RCM_CLOCK_FREQUENCY)/1000 ))

/* GPHY TASK ID 对应RCM TASK ID 起始枚举值 */
#define RCM_GPHY_TASK_ID_BEGIN_NUM              ( 0 )

/* WPHY TASK ID 对应RCM TASK ID 起始枚举值 */
#define RCM_WPHY_TASK_ID_BEGIN_NUM              ( 100 )

/* TPHY TASK ID 对应RCM TASK ID 起始枚举值 */
#define RCM_TPHY_TASK_ID_BEGIN_NUM              ( 200 )

/* LPHY TASK ID 对应RCM TASK ID 起始枚举值 */
#define RCM_LPHY_TASK_ID_BEGIN_NUM              ( 300 )

/* 一次删除的任务ID最大个数 */
#define RCM_REL_TASK_ID_MAX_NUM                 ( 32 )

/* 为了提高周期任务的精度，PHY传给RCM的周期值扩大的倍数 */
#define EXTEND_PERIOD_TASK_TIMES                ( ((VOS_UINT32)0x1)<<7 )

/* 获得周期任务的整数位 */
#define RCM_GetTaskPeriod( M_uwPeriod )         ( (((VOS_UINT32)M_uwPeriod) & (~( EXTEND_PERIOD_TASK_TIMES - 1 )))>>7 )

/* 获得周期任务的余数位 */
#define RCM_GetTaskRemainder( M_uwPeriod )      ( ((VOS_UINT32)M_uwPeriod) & ( EXTEND_PERIOD_TASK_TIMES - 1 )  )

/* 定义各任务属性，如优先级、是否独占等: GPHY */
#define RCM_GPHY_TASK_PROPERTY_TABLE  \
    { RCM_GPHY_TASK_START,                  RCM_TASK_PRIORITY_BUTT, RCM_PREEMPT_BUTT },\
    { RCM_GPHY_TASK_SEARCHING_MEA,          RCM_TASK_PRIORITY_3,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_SEARCHING_FBSB,         RCM_TASK_PRIORITY_3,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_SEARCHING_BCCH_PART,    RCM_TASK_PRIORITY_3,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_SEARCHING_BCCH_WHOLE,   RCM_TASK_PRIORITY_3,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_PAGE_NORMAL,       RCM_TASK_PRIORITY_2,    RCM_PREEMPT_NO   },\
    { RCM_GPHY_TASK_IDLE_PAGE_EXT,          RCM_TASK_PRIORITY_2,    RCM_PREEMPT_NO   },\
    { RCM_GPHY_TASK_IDLE_PAGE_REORG,        RCM_TASK_PRIORITY_4,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_SCELL_BCCH_PERIOD, RCM_TASK_PRIORITY_7,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_NCELL_BCCH_DECODE, RCM_TASK_PRIORITY_3_7,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_BASIC_CBCH,        RCM_TASK_PRIORITY_4,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_EXT_CBCH,          RCM_TASK_PRIORITY_4,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_NCELL_FB,          RCM_TASK_PRIORITY_4,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_NCELL_SB,          RCM_TASK_PRIORITY_4,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_CELL_RESELECT,     RCM_TASK_PRIORITY_4,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_BG_MEA,            RCM_TASK_PRIORITY_6,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_BG_FBSB,           RCM_TASK_PRIORITY_6,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_BG_BCCH,           RCM_TASK_PRIORITY_6,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_INTERRAT_WMEA,     RCM_TASK_PRIORITY_4,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_INTERRAT_TMEA,     RCM_TASK_PRIORITY_4,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_INTERRAT_LMEA,     RCM_TASK_PRIORITY_4,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_BG_WMEA,           RCM_TASK_PRIORITY_6,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_BG_TMEA,           RCM_TASK_PRIORITY_6,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_BG_LMEA,           RCM_TASK_PRIORITY_6,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_FULLLIST_MEA,      RCM_TASK_PRIORITY_5,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_TUNING_RACH,            RCM_TASK_PRIORITY_1,    RCM_PREEMPT_NO  },\
    { RCM_GPHY_TASK_TUNING_PRACH,           RCM_TASK_PRIORITY_1,    RCM_PREEMPT_NO   },\
    { RCM_GPHY_TASK_PBCCH,                  RCM_TASK_PRIORITY_5,    RCM_PREEMPT_NO   },\
    { RCM_GPHY_TASK_PPCH,                   RCM_TASK_PRIORITY_5,    RCM_PREEMPT_NO   },\
    { RCM_GPHY_TASK_TUNING_SINGLE_BLOCK,    RCM_TASK_PRIORITY_1,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_DCH_CS,                 RCM_TASK_PRIORITY_1,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_DCH_PS,                 RCM_TASK_PRIORITY_5,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_LOST_NET,               RCM_TASK_PRIORITY_1,    RCM_PREEMPT_NO   },\
    { RCM_GPHY_TASK_INACTIVE,               RCM_TASK_PRIORITY_1,    RCM_PREEMPT_NO   },\
    { RCM_GPHY_TASK_IDLE_SCELL_BCCH_RESEL,  RCM_TASK_PRIORITY_1_6,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_SCELL_BCCH_ENH,    RCM_TASK_PRIORITY_3_5,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_NCELL_BCCH_TYPE_RESEL, RCM_TASK_PRIORITY_2_5,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_IDLE_NCELL_BCCH_TYPE_BG,    RCM_TASK_PRIORITY_8,    RCM_PREEMPT_YES  },\
    { RCM_GPHY_TASK_END,                    RCM_TASK_PRIORITY_BUTT, RCM_PREEMPT_BUTT }


/* 定义各任务属性，如优先级、是否独占等: WPHY */
#define RCM_WPHY_TASK_PROPERTY_TABLE  \
    { RCM_WPHY_TASK_LIST,                  RCM_TASK_PRIORITY_BUTT, RCM_PREEMPT_BUTT },\
    { RCM_WPHY_TASK_FREE,                  RCM_TASK_PRIORITY_3,    RCM_PREEMPT_YES  },\
    { RCM_WPHY_TASK_CPICH_PI,              RCM_TASK_PRIORITY_2,    RCM_PREEMPT_NO   },\
    { RCM_WPHY_TASK_CPICH_MEAS,            RCM_TASK_PRIORITY_4,    RCM_PREEMPT_YES  },\
    { RCM_WPHY_TASK_DPCH,                  RCM_TASK_PRIORITY_5,    RCM_PREEMPT_YES  },\
    { RCM_WPHY_TASK_END,                   RCM_TASK_PRIORITY_BUTT, RCM_PREEMPT_BUTT }


/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 枚举名    : RCM_PHY_MSG_ID_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : RCM <-> PHY 接口消息ID
*****************************************************************************/
enum RCM_PHY_MSG_ID_ENUM
{
    ID_PHY_RCM_TASK_APPLY_REQ               = 0x1801,                           /* _H2ASN_MsgChoice PHY_RCM_TASK_APPLY_REQ_STRU */
    ID_PHY_RCM_TASK_REL_REQ                 = 0x1802,                           /* _H2ASN_MsgChoice PHY_RCM_TASK_REL_REQ_STRU */
    ID_PHY_RCM_TASK_RELALL_REQ              = 0x1803,                           /* _H2ASN_MsgChoice PHY_RCM_TASK_RELALL_REQ_STRU */
    ID_PHY_RCM_TASK_PREEMPT_ACK             = 0x1804,                           /* _H2ASN_MsgChoice PHY_RCM_TASK_PREEMPT_ACK_STRU */
    ID_PHY_RCM_TASK_RESUME_ACK              = 0x1805,                           /* _H2ASN_MsgChoice PHY_RCM_TASK_RESUME_ACK_STRU */
    ID_PHY_RCM_AWAKE_REQ                    = 0x1807,                           /* _H2ASN_MsgChoice PHY_RCM_AWAKE_REQ_STRU */
    ID_PHY_RCM_SLEEP_REQ                    = 0x1808,                           /* _H2ASN_MsgChoice PHY_RCM_SLEEP_REQ_STRU */
    ID_PHY_RCM_ASSIGN_ACK                   = 0x1809,                           /* _H2ASN_MsgChoice PHY_RCM_ASSIGN_ACK_STRU */
    ID_PHY_RCM_CLOCK_CAL_ACK                = 0x180a,                           /* _H2ASN_MsgChoice PHY_RCM_CLOCK_CAL_ACK_STRU */
    ID_PHY_RCM_GET_MSGLISTADDR_REQ          = 0x180c,                           /* _H2ASN_MsgChoice PHY_RCM_GET_MSGLISTADDR_REQ_STRU */
    ID_PHY_RCM_PERIOD_TASK_COMPLETE_REQ     = 0x180d,                           /* _H2ASN_MsgChoice PHY_RCM_TASK_COMPLETE_REQ_STRU */

    ID_RCM_PHY_TASK_APPLY_CNF               = 0x1881,                           /* _H2ASN_MsgChoice RCM_PHY_TASK_APPLY_CNF_STRU */
    ID_RCM_PHY_TASK_REL_CNF                 = 0x1882,                           /* _H2ASN_MsgChoice RCM_PHY_TASK_REL_CNF_STRU */
    ID_RCM_PHY_TASK_RELALL_CNF              = 0x1883,                           /* _H2ASN_MsgChoice RCM_PHY_TASK_RELALL_CNF_STRU */
    ID_RCM_PHY_TASK_PREEMPT_IND             = 0x1884,                           /* _H2ASN_MsgChoice RCM_PHY_TASK_PREEMPT_IND_STRU */
    ID_RCM_PHY_TASK_RESUME_IND              = 0x1885,                           /* _H2ASN_MsgChoice RCM_PHY_TASK_RESUME_IND_STRU */
    ID_RCM_PHY_ASSIGN_IND                   = 0x1889,                           /* _H2ASN_MsgChoice RCM_PHY_ASSIGN_IND_STRU */
    ID_RCM_PHY_CLOCK_CAL_IND                = 0x188a,                           /* _H2ASN_MsgChoice RCM_PHY_CLOCK_CAL_IND_STRU */
    ID_RCM_PHY_OVERTIME_IND                 = 0x188b,                           /* _H2ASN_MsgChoice RCM_PHY_OVERTIME_IND_STRU */
    ID_RCM_PHY_GET_MSGLISTADDR_CNF          = 0x188c,                           /* _H2ASN_MsgChoice RCM_PHY_GET_MSGLISTADDR_CNF_STRU */
    ID_RCM_PHY_PERIOD_TASK_COMPLETE_CNF     = 0x188d,                           /* _H2ASN_MsgChoice RCM_PHY_TASK_COMPLETE_CNF_STRU */

    ID_RCM_PHY_MSG_BUTT

};
typedef VOS_UINT16 RCM_PHY_MSG_ID_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_PREEMPT_FLAG_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : RCM中任务是否可被抢占标志
*****************************************************************************/
enum RCM_PREEMPT_FLAG_ENUM
{
    RCM_PREEMPT_YES                     = 0,                                    /* 允许被抢占 */
    RCM_PREEMPT_NO                         ,                                    /* 不允许被抢占 */
    RCM_PREEMPT_BUTT
};
typedef VOS_UINT16 RCM_PREEMPT_FLAG_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TASK_PRIORITY_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : RCM中任务优先级 任务可以申请的值为优先级1~8(200~900)其余优先级由RCM内部使用
*****************************************************************************/
enum RCM_TASK_PRIORITY_ENUM
{
    RCM_TASK_PRIORITY_HIGHEST           = 1 ,                                   /* 任务最高优先级 1~99用于独占 */
    RCM_TASK_PRIORITY_CALA_HIGHEST      = 100,                                  /* 优先级100 任务优先级能够提升到的最大值 RCM计算得到 */
    RCM_TASK_PRIORITY_1                 = 200,                                  /* 优先级200 任务优先级申请值 */
    RCM_TASK_PRIORITY_1_5               = 250,                                  /* 优先级250 任务优先级申请值 */
    RCM_TASK_PRIORITY_1_6               = 260,                                  /* 优先级260 任务优先级申请值 */
    RCM_TASK_PRIORITY_2                 = 300,                                  /* 优先级300 */
    RCM_TASK_PRIORITY_2_5               = 350,                                  /* 优先级350 */
    RCM_TASK_PRIORITY_3                 = 400,                                  /* 优先级400 */
    RCM_TASK_PRIORITY_3_5               = 450,                                  /* 优先级450 */
    RCM_TASK_PRIORITY_3_7               = 470,                                  /* 优先级470 */
    RCM_TASK_PRIORITY_4                 = 500,                                  /* 优先级500 */
    RCM_TASK_PRIORITY_4_5               = 550,                                  /* 优先级550 */
    RCM_TASK_PRIORITY_5                 = 600,                                  /* 优先级600 */
    RCM_TASK_PRIORITY_5_5               = 650,                                  /* 优先级650 */
    RCM_TASK_PRIORITY_6                 = 700,                                  /* 优先级700 */
    RCM_TASK_PRIORITY_7                 = 800,                                  /* 优先级800 */
    RCM_TASK_PRIORITY_8                 = 900,                                  /* 优先级900 */
    RCM_TASK_PRIORITY_LOWEST            = 1000,                                 /* 任务最低优先级 RCM使用 */
    RCM_TASK_PRIORITY_BUTT
};
typedef VOS_UINT16 RCM_TASK_PRIORITY_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_PHY_TASK_ID_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : PHY 在RCM中注册的任务ID
*****************************************************************************/
enum RCM_PHY_TASK_ID_ENUM
{
    RCM_GPHY_TASK_START                     = RCM_GPHY_TASK_ID_BEGIN_NUM,       /* 虚拟的启动任务 */
    RCM_GPHY_TASK_SEARCHING_MEA             ,                                   /* 1开机测量 */
    RCM_GPHY_TASK_SEARCHING_FBSB            ,                                   /* 2开机同步 */
    RCM_GPHY_TASK_SEARCHING_BCCH_PART       ,                                   /* 3开机解广播，部分读 */
    RCM_GPHY_TASK_SEARCHING_BCCH_WHOLE      ,                                   /* 4开机解广播，全读 */
    RCM_GPHY_TASK_IDLE_PAGE_NORMAL          ,                                   /* 5解寻呼任务 */
    RCM_GPHY_TASK_IDLE_PAGE_EXT             ,                                   /* 6解扩展寻呼任务 */
    RCM_GPHY_TASK_IDLE_PAGE_REORG           ,                                   /* 7Reorg态解寻呼 */
    RCM_GPHY_TASK_IDLE_SCELL_BCCH_PERIOD    ,                                   /* 8读服务小区系统消息 */
    RCM_GPHY_TASK_IDLE_NCELL_BCCH_DECODE    ,                                   /* 9读邻区系统消息 */
    RCM_GPHY_TASK_IDLE_BASIC_CBCH           ,                                   /* 10BASIC类型CBS业务 */
    RCM_GPHY_TASK_IDLE_EXT_CBCH             ,                                   /* 11EXT类型CBS业务 */
    RCM_GPHY_TASK_IDLE_NCELL_FB             ,                                   /* 12邻区FB */
    RCM_GPHY_TASK_IDLE_NCELL_SB             ,                                   /* 13邻区SB */
    RCM_GPHY_TASK_IDLE_CELL_RESELECT        ,                                   /* 14小区重选 */
    RCM_GPHY_TASK_IDLE_BG_MEA               ,                                   /* 15背景搜测量 */
    RCM_GPHY_TASK_IDLE_BG_FBSB              ,                                   /* 16背景搜同步 */
    RCM_GPHY_TASK_IDLE_BG_BCCH              ,                                   /* 17背景搜解广播 */
    RCM_GPHY_TASK_IDLE_INTERRAT_WMEA        ,                                   /* 18异系统W测量 */
    RCM_GPHY_TASK_IDLE_INTERRAT_TMEA        ,                                   /* 19异系统T测量 */
    RCM_GPHY_TASK_IDLE_INTERRAT_LMEA        ,                                   /* 20异系统L测量 */
    RCM_GPHY_TASK_IDLE_BG_WMEA              ,                                   /* 21背景搜W测量 */
    RCM_GPHY_TASK_IDLE_BG_TMEA              ,                                   /* 22背景搜T测量 */
    RCM_GPHY_TASK_IDLE_BG_LMEA              ,                                   /* 23背景搜L测量 */
    RCM_GPHY_TASK_IDLE_FULLLIST_MEA         ,                                   /* 24FULLIS测量 */
    RCM_GPHY_TASK_TUNING_RACH               ,                                   /* 25随机接入 */
    RCM_GPHY_TASK_TUNING_PRACH              ,                                   /* 26PACKET随机接入 */
    RCM_GPHY_TASK_PBCCH                     ,                                   /* 27PACKET系统消息 */
    RCM_GPHY_TASK_PPCH                      ,                                   /* 28PACKET寻呼 */
    RCM_GPHY_TASK_TUNING_SINGLE_BLOCK       ,                                   /* 29发送单块和多块 */
    RCM_GPHY_TASK_DCH_CS                    ,                                   /* 30CS专用态 */
    RCM_GPHY_TASK_DCH_PS                    ,                                   /* 31PS专用态 */
    RCM_GPHY_TASK_LOST_NET                  ,                                   /* 32丢网用 */
    RCM_GPHY_TASK_INACTIVE                  ,                                   /* 33去激活 */
    RCM_GPHY_TASK_IDLE_SCELL_BCCH_RESEL     ,                                   /* 34重选过程中服务小区系统消息读取 */
    RCM_GPHY_TASK_IDLE_SCELL_BCCH_ENH       ,                                   /* 35增强接收过程中系统消息读取 */
    RCM_GPHY_TASK_IDLE_NCELL_BCCH_TYPE_RESEL,                                   /* 36重选过程中系统消息读取 */
    RCM_GPHY_TASK_IDLE_NCELL_BCCH_TYPE_BG   ,                                   /* 37背景搜过程中系统消息读取 */
    RCM_GPHY_TASK_END                       ,                                   /* 虚拟的启动结束 */
    RCM_WPHY_TASK_LIST                      = RCM_WPHY_TASK_ID_BEGIN_NUM,
    RCM_WPHY_TASK_FREE                      = 101,
    RCM_WPHY_TASK_CPICH_PI                  = 102,
    RCM_WPHY_TASK_CPICH_MEAS                = 103,
    RCM_WPHY_TASK_DPCH                      = 104,
    RCM_WPHY_TASK_END                       ,
    RCM_TPHY_TASK_LIST                      = RCM_TPHY_TASK_ID_BEGIN_NUM,
    RCM_LPHY_TASK_LIST                      = RCM_LPHY_TASK_ID_BEGIN_NUM,
    RCM_PHY_TASK_BUTT
};
typedef VOS_UINT16 RCM_PHY_TASK_ID_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_MODE_ID_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 物理层模式标示 排列次序和校准次序一致
*****************************************************************************/
enum RCM_MODE_ID_ENUM
{
    RCM_LTE                             = 0,                                    /* LTE */
    RCM_WCDMA                           = 1,                                    /* WCDMA */
    RCM_1X                              = 2,                                    /* 1X */
    RCM_HRPD                            = 3,                                    /* HRPD */
    RCM_TD                              = 4,                                    /* TD */
    RCM_G0                              = 5,                                    /* G0 */
    RCM_G1                              = 6,                                    /* G1 */
    RCM_G2                              = 7,                                    /* G2 */
    RCM_MODE_ID_BUTT
};
typedef VOS_UINT16  RCM_MODE_ID_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_MODEM_ID_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : MODEM ID
*****************************************************************************/
enum RCM_MODEM_ID_ENUM
{
    RCM_MODEM_1                         = 0,                                    /* MODEM0 */
    RCM_MODEM_2                         = 1,                                    /* MODEM1 */
    RCM_MODEM_3                         = 2,                                    /* MODEM2 */
    RCM_MODEM_ID_BUTT
};
typedef VOS_UINT16  RCM_MODEM_ID_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_RF_ID_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : RF ID
*****************************************************************************/
enum RCM_RF_ID_ENUM
{
    RCM_RF_0                            = 0,                                    /* RF0 */
    RCM_RF_1                            = 1,                                    /* RF1 */
    RCM_RF_ID_BUTT
};
typedef VOS_UINT16  RCM_RF_ID_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_RTT_STA_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : RTT状态,用于时钟校准选择

*****************************************************************************/
enum RCM_RTT_STA_ENUM
{
    RCM_RTT_INITIAL                     = 0,                                    /* 初始态 ,没有驻留 */
    RCM_RTT_DRX                         = 1,                                    /* 非连续态，空闲态 */
    RCM_RTT_CONTINUE                    = 2,                                    /* 连续态 ，打电话数传*/
    RCM_RTT_STA_BUTT
};
typedef VOS_UINT16  RCM_RTT_STA_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_RSLT_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 接口中的有效性标识
*****************************************************************************/
enum RCM_RSLT_ENUM
{
    RCM_SUCC                            = 0,                                    /* 成功 */
    RCM_FAIL_PARA                       = 1,                                    /* 参数错误 */
    RCM_FAIL_OVERTIME                   = 2,                                    /* 申请提前量不够 */
    RCM_FAIL_OTHER                      = 3,                                    /* 操作节点为空 */
    RCM_RSLT_BUTT
};
typedef VOS_UINT16  RCM_RSLT_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_DIRECTION_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 接口中的调整方向标识
*****************************************************************************/
enum RCM_DIRECTION_ENUM
{
    RCM_DIRECTION_SLOWDOWN              = 0,                                    /* 减速1个周期 */
    RCM_DIRECTION_SPEEDUP               = 1,                                    /* 加速1个周期 */
    RCM_DIRECTION_KEEP                  = 2,                                    /* 不调整 */
    RCM_DIRECTION_BUTT
};
typedef VOS_UINT16 RCM_DIRECTION_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TASK_PERIOD_FLAG_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 周期/非周期任务标识
*****************************************************************************/
enum RCM_TASK_PERIOD_FLAG_ENUM
{
    RCM_TASK_PERIOD_FLAG_INVALID        = 0,                                    /* 非周期任务 */
    RCM_TASK_PERIOD_FLAG_VALID          = 1,                                    /* 周期任务 */
    RCM_TASK_PERIOD_FLAG_BUTT
};
typedef VOS_UINT16  RCM_TASK_PERIOD_FLAG_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TASK_FIXPOS_FLAG_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 固定/非固定启动时刻标识
*****************************************************************************/
enum RCM_TASK_FIXPOS_FLAG_ENUM
{
    RCM_TASK_FIXPOS_FLAG_INVALID        = 0,                                    /* 无固定启动时间 */
    RCM_TASK_FIXPOS_FLAG_VALID          = 1,                                    /* 有固定启动时间 */
    RCM_TASK_FIXPOS_FLAG_BUTT
};
typedef VOS_UINT16  RCM_TASK_FIXPOS_FLAG_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TASK_LIMITLEN_FLAG_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 固定/非固定长度标识
*****************************************************************************/
enum RCM_TASK_LIMITLEN_FLAG_ENUM
{
    RCM_TASK_LIMITLEN_FLAG_INVALID      = 0,                                    /* 无限长任务 */
    RCM_TASK_LIMITLEN_FLAG_VALID        = 1,                                    /* 有限长任务 */
    RCM_TASK_LIMITLEN_FLAG_BUTT
};
typedef VOS_UINT16  RCM_TASK_LIMITLEN_FLAG_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TASK_IMMEDIATE_ASSIGN_FLAG_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 立即/非立即分配任务标识
*****************************************************************************/
enum RCM_TASK_IMMEDIATE_ASSIGN_FLAG_ENUM
{
    RCM_TASK_IMMEDIATE_ASSIGN_FLAG_INVALID  = 0,                                /* 不需要立即分配任务 */
    RCM_TASK_IMMEDIATE_ASSIGN_FLAG_VALID    = 1,                                /* 需要立即分配任务 */
    RCM_TASK_IMMEDIATE_ASSIGN_FLAG_BUTT
};
typedef VOS_UINT16  RCM_TASK_IMMEDIATE_ASSIGN_FLAG_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TASK_PREEMPTIND_FLAG_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 可打断/不可打断任务标识
*****************************************************************************/
enum RCM_TASK_PREEMPTIND_FLAG_ENUM
{
    RCM_TASK_PREEMPTIND_FLAG_INVALID        = 0,                                /* 不可打断任务 */
    RCM_TASK_PREEMPTIND_FLAG_VALID          = 1,                                /* 可打断任务 */
    RCM_TASK_PREEMPTIND_FLAG_BUTT
};
typedef VOS_UINT16  RCM_TASK_PREEMPTIND_FLAG_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_TASK_RELEASE_PRINCIPAL_FLAG_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 物理层/RCM 任务释放标识
*****************************************************************************/
enum RCM_TASK_RELEASE_PRINCIPAL_ENUM
{
    RCM_TASK_RELEASE_PRINCIPAL_PHY          = 0,                                /* 由物理层释放 */
    RCM_TASK_RELEASE_PRINCIPAL_RCM          = 1,                                /* 由RCM释放 */
    RCM_TASK_RELEASE_PRINCIPAL_FLAG_BUTT
};
typedef VOS_UINT16  RCM_TASK_RELEASE_PRINCIPAL_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_CMD_ENUM_UINT16
 协议表格  :
 ASN.1描述 :
 枚举说明  : 消息队列中消息类型
*****************************************************************************/
enum RCM_CMD_ENUM
{
    RCM_CMD_TASK_ASSIGN_SUCC                = 0,                                /* 任务分配成功 */
    RCM_CMD_TASK_ASSIGN_FAIL_OTHER          = 1,                                /* 任务分配其他失败 */
    RCM_CMD_TASK_RECYCLE                    = 2,                                /* 已分配任务还没启动进行回收 */
    RCM_CMD_MSG_BUTT
};
typedef VOS_UINT16  RCM_CMD_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_CMODE_TYPE_ENUM_UINT16
 枚举说明  : C模的系统时间类型,00：1x定时，01：evdo定时，10： base_cnt ，11：无效。
*****************************************************************************/
enum RCM_CMODE_TYPE_ENUM
{
    RCM_CMODE_1X                        = 0,
    RCM_CMODE_EVDO                      = 1,
    RCM_CMODE_BASECNT                   = 2,
    RCM_CMODE_BUTT
};
typedef VOS_UINT16 RCM_CMODE_TYPE_ENUM_UINT16;

/*****************************************************************************
 枚举名    : RCM_WMODE_TYPE_ENUM_UINT16
 枚举说明  : W模的系统时间类型
             00：sys_cnt定时        01：base_cnt定时
             10：无效               11：无效
*****************************************************************************/
enum RCM_WMODE_TYPE_ENUM
{
    RCM_WMODE_SYSCNT                    = 0,
    RCM_WMODE_BASECNT                   = 1,
    RCM_WMODE_BUTT
};
typedef VOS_UINT16 RCM_WMODE_TYPE_ENUM_UINT16;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 结构名    : PHY_RCM_TASK_REQ_PARA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务请求消息数据结构
*****************************************************************************/
typedef struct
{
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_RF_ID_ENUM_UINT16                       enRFID;                         /* RF ID */
    VOS_UINT16                                  usPriority;                     /* 任务优先级 */
    VOS_UINT16                                  usTaskID;                       /* 任务ID */
    RCM_RTT_STA_ENUM_UINT16                     enRttSta;                       /* RTT状态 */
    RCM_TASK_FIXPOS_FLAG_ENUM_UINT16            enFixPosFlag;                   /* 0-无固定启动时刻 1-有固定启动时刻 */
    RCM_TASK_LIMITLEN_FLAG_ENUM_UINT16          enLimitedLenFlag;               /* 0-无限长 1-有限长 */
    RCM_TASK_PERIOD_FLAG_ENUM_UINT16            enPeriodFlag;                   /* 0-无周期任务 1-周期任务 */
    RCM_TASK_PREEMPTIND_FLAG_ENUM_UINT16        enPreemptInd;                   /* 0-不可抢断 1-可抢断 */
    RCM_TASK_RELEASE_PRINCIPAL_ENUM_UINT16      enReleasePrincipal;             /* 0-PHY释放  1-RCM释放 */
    RCM_TASK_IMMEDIATE_ASSIGN_FLAG_ENUM_UINT16  enImmediateAssignFlag;          /* 0-不需立即分配 1-需立即分配 */
    VOS_UINT16                                  usFailCnt;                      /* 连续失败次数 */
    VOS_UINT16                                  usFailThd;                      /* 最大容忍失败次数门限,如果填0表示不对当前任务进行优先级调整 */
    VOS_UINT32                                  ulStartPos;                     /* 任务启动时刻 必须填写有效值，如果无固定启动时刻则为最迟启动时刻，*/
    VOS_UINT32                                  ulLen;                          /* 任务长度.有限长任务必须填写有效值,不能超过(RCM_CLOCK_PERIOD/4), */
    VOS_UINT32                                  ulMinLen;                       /* 任务单次分配的最小长度,所有必须填写有效值,不能超过(RCM_CLOCK_PERIOD/4),如果有限长任务可以等于ulLen */
    VOS_UINT32                                  ulPeriod;                       /* 周期任务的周期，如果是周期任务必须填写有效值 */
    VOS_UINT32                                  ulPrmptPeriod;                  /* 无限长任务的抢占统计周期 非PS任务写无效值 0xffffffff PS业务如果使用此功能，要填成小于(RCM_CLOCK_PERIOD/4)的值 */
    VOS_UINT32                                  ulPrmptTimeThd;                 /* 无限长任务单次被抢占时间门限 非PS任务写无效值 0xffffffff PS业务如果使用此功能，要填成小于(RCM_CLOCK_PERIOD/4)的值 */
    VOS_UINT32                                  ulTotalPrmptTimeThd;            /* 无限长任务周期内累计被抢占时间门限 非PS任务写无效值 0xffffffff PS业务如果使用此功能，要填成小于(RCM_CLOCK_PERIOD/4)的值 */
}PHY_RCM_TASK_REQ_PARA_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_TASK_APPLY_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务请求消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    PHY_RCM_TASK_REQ_PARA_STRU                  stTaskReqPara;                  /* 任务申请参数 */
}PHY_RCM_TASK_APPLY_REQ_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_TASK_APPLY_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务请求响应消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_RSLT_ENUM_UINT16                        enResult;                       /* 确认结果 */
    VOS_UINT16                                  usTaskID;                       /* 任务ID */
    VOS_UINT32                                  uw32KSysTime;                   /* 当前的32K 定时 */
}RCM_PHY_TASK_APPLY_CNF_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_TASK_REL_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务释放请求消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                          /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT32                                  uwRelTaskIdNum;                 /* 待删除任务个数，对应发ausRelTaskId的下标 */
    VOS_UINT16                                  ausRelTaskId[RCM_REL_TASK_ID_MAX_NUM]; /* 待删除任务数组 */
}PHY_RCM_TASK_REL_REQ_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_TASK_REL_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 单个任务释放请求响应消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT32                                  uw32KSysTime;                   /* 当前的32K 定时 */
    RCM_RSLT_ENUM_UINT16                        enResult;                       /* 确认结果,RCM只检测MODEM和MODEID的正确性  */
    VOS_UINT16                                  usRsv2;
}RCM_PHY_TASK_REL_CNF_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_TASK_RELALL_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务全部释放请求消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                          /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
}PHY_RCM_TASK_RELALL_REQ_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_TASK_RELALL_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 全部任务释放请求响应消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                          /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT32                                  uw32KSysTime;                   /* 当前的32K 定时 */
    RCM_RSLT_ENUM_UINT16                        enResult;                       /* 确认结果 */
    VOS_UINT16                                  usRsv2;                         /* 保留字 */
}RCM_PHY_TASK_RELALL_CNF_STRU;


/*****************************************************************************
 结构名    : PHY_RCM_AWAKE_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : wake请求消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
}PHY_RCM_AWAKE_REQ_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_SLEEP_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : sleep请求消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
}PHY_RCM_SLEEP_REQ_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_TASK_PREEMPT_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务抢占指示消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT16                                  usTaskID;                       /* 任务ID */
    RCM_RF_ID_ENUM_UINT16                       enRfID;                         /* 被抢占的RFID */
    VOS_UINT32                                  ulPreemptPos;                   /* 任务抢占时刻*/
    VOS_UINT32                                  ul32KSysTime;                   /* 当前的32K 定时 */
}RCM_PHY_TASK_PREEMPT_IND_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_TASK_PREEMPT_ACK_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务抢占确认消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_RF_ID_ENUM_UINT16                       enRfID;                         /* 被抢占的RFID */
    RCM_RSLT_ENUM_UINT16                        enResult;                       /* 确认结果 */
    VOS_UINT16                                  usTaskID;                       /* 任务ID */
    VOS_UINT16                                  usReserved;                     /* 保留字 */
}PHY_RCM_TASK_PREEMPT_ACK_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_TASK_RESUME_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务恢复指示消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT32                                  ulResumPos;                     /* 任务恢复时刻*/
    VOS_UINT16                                  usTaskID;                       /* 任务ID */
    VOS_UINT16                                  usRsv2;                         /* 保留字 */
    VOS_UINT32                                  ul32KSysTime;                   /* 当前的32K 定时 */
}RCM_PHY_TASK_RESUME_IND_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_TASK_RESUME_ACK_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务恢复确认消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字段 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_RSLT_ENUM_UINT16                        enResult;                       /* 确认结果 */
    VOS_UINT16                                  usTaskID;                       /* 任务ID */
}PHY_RCM_TASK_RESUME_ACK_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_MSG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 消息队列数据结构
*****************************************************************************/

typedef struct
{
    RCM_CMD_ENUM_UINT16                         enCmdID;                        /* 消息CMD类型 */
    VOS_UINT16                                  usTaskID;                       /* 任务ID */
    VOS_UINT32                                  ulStartPos;                     /* 任务分配的启动时刻 */
    VOS_UINT32                                  ulStopPos;                      /* 任务分配的结束时刻 */
}RCM_PHY_MSG_STRU;


/*****************************************************************************
 结构名    : RCM_PHY_RPT_MSG_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 消息队列数据结构
*****************************************************************************/

typedef struct
{
    VOS_UINT16                                  usIndex;                        /* 任务ID */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_CMD_ENUM_UINT16                         enCmdID;                        /* 消息CMD类型 */
    VOS_UINT16                                  usTaskID;                       /* 任务ID */
    VOS_UINT32                                  ulStartPos;                     /* 任务分配的启动时刻 */
    VOS_UINT32                                  ulStopPos;                      /* 任务分配的结束时刻 */
}RCM_PHY_RPT_MSG_STRU;


/*****************************************************************************
 结构名    : RCM_PHY_MSG_LIST_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 各个MODE的消息缓存
*****************************************************************************/
typedef struct
{
    VOS_UINT16                                  usRdIdx;                        /* 读指针 */
    VOS_UINT16                                  usWrIdx;                        /* 写指针 */
    RCM_PHY_MSG_STRU                            astMsgList[RCM_PHY_ASSIGN_BUF_SIZE];    /* 消息队列 */
}RCM_PHY_MSG_LIST_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_ASSIGN_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务分配指示消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT32                                  uw32KSysTime;                   /* 当前的32K 定时 */
}RCM_PHY_ASSIGN_IND_STRU;


/*****************************************************************************
 结构名    : RCM_PHY_ASSIGN_IND_FORTL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务分配指示消息数据结构发给TL
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT32                                  uw32KSysTime;                   /* 当前的32K 定时 */
    RCM_PHY_MSG_STRU                            stMsg;
}RCM_PHY_ASSIGN_IND_FORTL_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_ASSIGN_ACK_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务分配确认消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字段 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_RSLT_ENUM_UINT16                        enResult;                       /* 确认结果 */
    VOS_UINT16                                  usRsv2;                         /* 保留字段 */
}PHY_RCM_ASSIGN_ACK_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_CLOCK_CAL_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 时钟校准指示消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
}RCM_PHY_CLOCK_CAL_IND_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_CLOCK_CAL_ACK_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 时钟校准上报消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_RSLT_ENUM_UINT16                        enResult;                       /* 确认结果 */
    RCM_DIRECTION_ENUM_UINT16                   enDirection;                    /* 调整方向 */
    VOS_UINT32                                  ulAdjustValue;                  /* 调整周期 */
}PHY_RCM_CLOCK_CAL_ACK_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_OVERTIME_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务超时上报消息数据结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT16                                  usTaskID;                       /* 任务ID */
    VOS_UINT16                                  usRsv2;                         /* 保留字 */
    VOS_UINT32                                  ulStartPos;                     /* 任务分配的启动时刻 */
    VOS_UINT32                                  ulStopPos;                      /* 任务分配的结束时刻 */
}RCM_PHY_OVERTIME_IND_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_GET_MSGLISTADDR_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 任务获取消息队列地址结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /*_H2ASN_Skip*/ /* 原语类型 */
    VOS_UINT16                                  usRsv;                          /* 保留字 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
}PHY_RCM_GET_MSGLISTADDR_REQ_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_GET_MSGLISTADDR_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 返回消息队列地址结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /*_H2ASN_Skip*/ /* 原语类型 */
    VOS_UINT16                                  usRsv;                          /* 保留字 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    RCM_RSLT_ENUM_UINT16                        enResult;                       /* 确认结果 */
    VOS_UINT16                                  usRsv2;                         /* 保留字 */
    VOS_UINT32                                  uw32KSysTime;                   /* 当前的32K 定时 */
    RCM_PHY_MSG_LIST_STRU                      *pstMsgList;                     /* 消息队列首地址 */
}RCM_PHY_GET_MSGLISTADDR_CNF_STRU;

/*****************************************************************************
 结构名    : PHY_RCM_TASK_COMPLETE_REQ_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知RCM任务执行完成
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT16                                  usTaskID;                       /* 任务ID */
    RCM_RF_ID_ENUM_UINT16                       enRfID;                         /* 任务所在的RFID */
    VOS_UINT32                                  ulStartPos;                     /* 提前完成的周期任务的启动时刻 */
}PHY_RCM_TASK_COMPLETE_REQ_STRU;

/*****************************************************************************
 结构名    : RCM_PHY_TASK_COMPLETE_CNF_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 通知PHY收到任务执行完成指示
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /*_H2ASN_Skip*/
    RCM_PHY_MSG_ID_ENUM_UINT16                  enMsgID;                        /* 原语类型 */ /*_H2ASN_Skip*/
    VOS_UINT16                                  usRsv;                          /* 保留字 */
    VOS_UINT16                                  usOPID;                         /* 操作标识 */
    VOS_UINT16                                  usRsv1;                         /* 保留字 */
    RCM_MODE_ID_ENUM_UINT16                     enModeID;                       /* 模式ID */
    RCM_MODEM_ID_ENUM_UINT16                    enModemID;                      /* modem ID */
    VOS_UINT16                                  usTaskID;                       /* 任务ID */
    RCM_RF_ID_ENUM_UINT16                       enRfID;                         /* 任务所在的RFID */
    RCM_RSLT_ENUM_UINT16                        enResult;                       /* 确认结果 */
    VOS_UINT16                                  usRsv2;                         /* 保留字 */
    VOS_UINT32                                  uw32KSysTime;                   /* 当前32k时间 */
}RCM_PHY_TASK_COMPLETE_CNF_STRU;

/*****************************************************************************
 结构名    : RCM_WMODE_TIMING_STRU
 协议表格  :
 结构说明  : W模系统时间
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwSfnCnt;                              /* SFN计数，范围0~4095 */
    VOS_UINT16                          uhwSlotCnt;                             /* 时隙计数，范围0~14 */
    VOS_UINT16                          uhwChipCnt;                             /* chip计数，范围0~2559 */
    VOS_UINT16                          uhwClkCnt;                              /* chip内时钟计数，范围0~15 */
    RCM_WMODE_TYPE_ENUM_UINT16          enWMode;                                /* W模的时间类型 */
    VOS_UINT16                          uhwReserved;                            /* 保留位 */
}RCM_WMODE_TIMING_STRU;

/*****************************************************************************
 结构名    : RCM_GMODE_TIMING_STRU
 协议表格  :
 结构说明  : G模系统时间
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwFnCnt;                                /* FN计数 */
    VOS_UINT16                          uhwQbCnt;                               /* 系统定时帧内Qb计数，范围0~4999 */
    VOS_UINT16                          uhwReserved;                            /* 保留位 */
}RCM_GMODE_TIMING_STRU;

/*****************************************************************************
 结构名    : RCM_LTEMODE_TIMING_STRU
 协议表格  :
 结构说明  : LTE模系统时间
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uwSfnCnt;                               /* SFN计数，范围0~1023 */
    VOS_UINT16                          uhwsubfrm_cnt;                          /* 子帧计数 */
    VOS_UINT16                          uhwTsCnt;                               /* 子帧内计数 */
    VOS_UINT16                          uhwReserved;                            /* 保留位 */
}RCM_LTEMODE_TIMING_STRU;

/*****************************************************************************
 结构名    : RCM_TDMODE_TIMING_STRU
 协议表格  :
 结构说明  : TD模系统时间
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwSfnCnt;                               /* tcfsr系统子帧号0~65535循环计数，TDS实际有效子帧范围为0~8191 */
    VOS_UINT32                          uwSubFrmcnt;                            /* framc帧计数 */
}RCM_TDMODE_TIMING_STRU;

/*****************************************************************************
 结构名    : RCM_TIMING_STRU
 协议表格  :
 结构说明  : 各模系统时间和32K定时
*****************************************************************************/
typedef struct
{
    RCM_WMODE_TIMING_STRU               stWTime;                                /* W模系统时间 */
    VOS_UINT32                          uwCTime;                                /* C模系统时间 */
    RCM_GMODE_TIMING_STRU               stGmTime;                               /* Gm模系统时间 */
    RCM_GMODE_TIMING_STRU               stGsTime;                               /* Gs模系统时间 */
    RCM_LTEMODE_TIMING_STRU             stLTETime;                              /* LTE模系统时间 */
    RCM_TDMODE_TIMING_STRU              stTDSTime;                              /* TD模系统时间 */
    VOS_UINT32                          uw32KSysTime;                           /* 32K系统定时 */
}RCM_TIMING_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID RCM_DRV_SetWTimeMode( RCM_WMODE_TYPE_ENUM_UINT16 enWModeType );
extern VOS_UINT32 RCM_DRV_TimingInquire( RCM_WMODE_TYPE_ENUM_UINT16 enCModemType,
                                                 RCM_TIMING_STRU* pstTiming );
extern VOS_INT32 RCM_SCHDL_CalcDistance( VOS_UINT32 uwTimeX,
                                                 VOS_UINT32 uwTimeY );

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of RcmPhyInterface.h */



