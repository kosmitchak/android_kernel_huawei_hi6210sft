

#ifndef __CSDR_DDR_RAM_EXT_H__
#define __CSDR_DDR_RAM_EXT_H__


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
/* 临时打桩 */
#define CSDR_DDR_RAM_BASE                   ( GLOBAL_MEM_TDS_TABLE_ADDR )       /* ( 0x5C600000 ) */


/****************************************************************************
 *                     ZSP使用DDR中地址的定义                               *
 *                                                                          *
*****************************************************************************/
#define CSDR_CTTF_1X_REQUIRE_LEN            ( 0x4 )
#define CSDR_CTTF_1X_F_DATA_BUFFER_LEN      ( 0xA44 )
#define CSDR_DDR_RAM_FCH_PING_LEN           ( 0x300 )
#define CSDR_DDR_RAM_FCH_PANG_LEN           ( 0x300 )
#define CSDR_DDR_RAM_SCH_PING_LEN           ( 0x3000 )
#define CSDR_DDR_RAM_SCH_PANG_LEN           ( 0x3000 )
#define CSDR_DDR_RAM_SCCH_PING_LEN          ( 0x180 )
#define CSDR_DDR_RAM_SCCH_PANG_LEN          ( 0x180 )
#define CSDR_DDR_RAM_DCCH_PING_LEN          ( 0x300 )
#define CSDR_DDR_RAM_DCCH_PANG_LEN          ( 0x300 )
#define CSDR_DDR_RAM_BCCH_PING_LEN          ( 0xC00 )
#define CSDR_DDR_RAM_BCCH_PANG_LEN          ( 0xC00 )
#define CSDR_DDR_RAM_CCCH_PING_LEN          ( 0xC00 )
#define CSDR_DDR_RAM_CCCH_PANG_LEN          ( 0xC00 )
#define CSDR_DDR_RAM_PCH_PING_LEN           ( 0x180 )
#define CSDR_DDR_RAM_PCH_PANG_LEN           ( 0x180 )
#define CSDR_DDR_RAM_SYNCH_PING_LEN         ( 0x180 )
#define CSDR_DDR_RAM_SYNCH_PANG_LEN         ( 0x180 )

/* CTTF与SDR 下行decode和上行encode头部地址信息,对应结构CTTF_CSDR_1X_ADDR_INFO_STRU */
#define CSDR_CTTF_1X_REQUIRE_ADDR           ( CSDR_DDR_RAM_BASE + 0x0 )

/* CTTF与SDR 下行decode,译码数据对应 buffer 对应结构CSDR_CTTF_1X_F_DATA_BUFFER_STRU */
#define CSDR_CTTF_1X_F_DATA_BUFFER_ADDR     (  CSDR_CTTF_1X_REQUIRE_ADDR \
                                             + CSDR_CTTF_1X_REQUIRE_LEN )

/* DPP输出FCH信道LLR结果ping存放空间, 总大小768个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_FCH_PING_ADDR          (  CSDR_CTTF_1X_F_DATA_BUFFER_ADDR \
                                             + CSDR_CTTF_1X_F_DATA_BUFFER_LEN )

/* DPP输出FCH信道LLR结果pang存放空间, 总大小768个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_FCH_PANG_ADDR          (  CSDR_DDR_RAM_FCH_PING_ADDR \
                                             + CSDR_DDR_RAM_FCH_PING_LEN )

/* DPP输出SCH信道LLR结果ping存放空间, 总大小12288个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_SCH_PING_ADDR          (  CSDR_DDR_RAM_FCH_PANG_ADDR \
                                             + CSDR_DDR_RAM_FCH_PANG_LEN )

/* DPP输出SCH信道LLR结果pang存放空间, 总大小12288个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_SCH_PANG_ADDR          (  CSDR_DDR_RAM_SCH_PING_ADDR \
                                             + CSDR_DDR_RAM_SCH_PING_LEN )

/* DPP输出SCCH信道LLR结果ping存放空间, 总大小384个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_SCCH_PING_ADDR         (  CSDR_DDR_RAM_SCH_PANG_ADDR \
                                             + CSDR_DDR_RAM_SCH_PANG_LEN )

/* DPP输出SCCH信道LLR结果pang存放空间, 总大小384个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_SCCH_PANG_ADDR         (  CSDR_DDR_RAM_SCCH_PING_ADDR \
                                             + CSDR_DDR_RAM_SCCH_PING_LEN )

/* DPP输出DCCH信道LLR结果ping存放空间, 总大小768个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_DCCH_PING_ADDR         (  CSDR_DDR_RAM_SCCH_PANG_ADDR \
                                             + CSDR_DDR_RAM_SCCH_PANG_LEN )

/* DPP输出DCCH信道LLR结果pang存放空间, 总大小768个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_DCCH_PANG_ADDR         (  CSDR_DDR_RAM_DCCH_PING_ADDR \
                                             + CSDR_DDR_RAM_DCCH_PING_LEN )

/* DPP输出BCCH信道LLR结果ping存放空间, 总大小3072个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_BCCH_PING_ADDR         (  CSDR_DDR_RAM_DCCH_PANG_ADDR \
                                             + CSDR_DDR_RAM_DCCH_PANG_LEN )

/* DPP输出BCCH信道LLR结果pang存放空间, 总大小3072个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_BCCH_PANG_ADDR         (  CSDR_DDR_RAM_BCCH_PING_ADDR \
                                             + CSDR_DDR_RAM_BCCH_PING_LEN )

/* DPP输出CCCH信道LLR结果ping存放空间, 总大小3072个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_CCCH_PING_ADDR         (  CSDR_DDR_RAM_BCCH_PANG_ADDR \
                                             + CSDR_DDR_RAM_BCCH_PANG_LEN )

/* DPP输出CCCH信道LLR结果pang存放空间, 总大小3072个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_CCCH_PANG_ADDR         (  CSDR_DDR_RAM_CCCH_PING_ADDR \
                                             + CSDR_DDR_RAM_CCCH_PING_LEN )

/* DPP输出PCH信道LLR结果ping存放空间, 总大小384个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_PCH_PING_ADDR          (  CSDR_DDR_RAM_CCCH_PANG_ADDR \
                                             + CSDR_DDR_RAM_CCCH_PANG_LEN )

/* DPP输出PCH信道LLR结果pang存放空间, 总大小384个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_PCH_PANG_ADDR          (  CSDR_DDR_RAM_PCH_PING_ADDR \
                                             + CSDR_DDR_RAM_PCH_PING_LEN )

/* DPP输出SYNCH信道LLR结果ping存放空间, 总大小384个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_SYNCH_PING_ADDR        (  CSDR_DDR_RAM_PCH_PANG_ADDR \
                                             + CSDR_DDR_RAM_PCH_PANG_LEN )

/* DPP输出SYNCH信道LLR结果pang存放空间, 总大小384个symbol，每个symbol占8bit */
#define CSDR_DDR_RAM_SYNCH_PANG_ADDR        (  CSDR_DDR_RAM_SYNCH_PING_ADDR \
                                             + CSDR_DDR_RAM_SYNCH_PING_LEN )

/* CTTF与ENC编码前数据存放空间 */
#define CTTF_CSDR_1X_R_DATA_BUFFER_ADDR     ( CSDR_DDR_RAM_SYNCH_PANG_ADDR \
                                             + CSDR_DDR_RAM_SYNCH_PANG_LEN )

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


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

#endif /* end of csdr_ddr_ram_ext.h */

