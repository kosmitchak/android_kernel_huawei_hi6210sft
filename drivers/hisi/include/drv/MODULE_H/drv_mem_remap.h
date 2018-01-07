

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "drv_global.h"


#ifndef __DRV_MEM_REMAP_H__
#define __DRV_MEM_REMAP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/* DDR中各个内存段的类型 */
typedef enum tagBSP_DDR_SECT_TYPE_E
{
    BSP_DDR_SECT_TYPE_TTF = 0x0,
    BSP_DDR_SECT_TYPE_ARMDSP,
    BSP_DDR_SECT_TYPE_UPA,
    BSP_DDR_SECT_TYPE_CQI,
    BSP_DDR_SECT_TYPE_APT,
    BSP_DDR_SECT_TYPE_ET,
    BSP_DDR_SECT_TYPE_BBPMASTER,
    BSP_DDR_SECT_TYPE_NV,
    BSP_DDR_SECT_TYPE_DICC,
    BSP_DDR_SECT_TYPE_WAN,
    BSP_DDR_SECT_TYPE_SHARE_MEM,
    BSP_DDR_SECT_TYPE_EXCP,
    BSP_DDR_SECT_TYPE_HIFI,
    BSP_DDR_SECT_TYPE_HARQ,
    BSP_DDR_SECT_TYPE_ZSP_UP,
    BSP_DDR_SECT_TYPE_HUTAF,
    BSP_DDR_SECT_TYPE_BBPSAMPLE,
    BSP_DDR_SECT_TYPE_TDS_LH2,
    BSP_DDR_SECT_TYPE_BUTTOM
}BSP_DDR_SECT_TYPE_E;


/* AXI中各个内存段的类型 */
typedef enum tagBSP_AXI_SECT_TYPE_E
{
    BSP_AXI_SECT_TYPE_ACORE_DEEPSLEEP = 0x0,
    BSP_AXI_SECT_TYPE_FLASH_SEM,
    BSP_AXI_SECT_TYPE_IFC,
    BSP_AXI_SECT_TYPE_ICC,
    BSP_AXI_SECT_TYPE_MEMMGR_FLAG,
    BSP_AXI_SECT_TYPE_DYNAMIC,
    BSP_AXI_SECT_TYPE_SOFT_FLAG,
    BSP_AXI_SECT_TYPE_IPF,
    BSP_AXI_SECT_TYPE_TEMPERATURE,
    BSP_AXI_SECT_TYPE_ONOFF,
    BSP_AXI_SECT_TYPE_DICC,
    BSP_AXI_SECT_TYPE_HIFI,
    BSP_AXI_SECT_TYPE_DDR_CAPACITY,
    BSP_AXI_SECT_TYPE_PTABLE,
    BSP_AXI_SECT_TYPE_RESERVE,
    BSP_AXI_SECT_TYPE_DLOAD_AUTOINSTALL,
    BSP_AXI_SECT_TYPE_DLOAD,
    BSP_AXI_SECT_TYPE_CCORE_DEEPSLEEP ,
    BSP_AXI_SECT_TYPE_TTF_BBP,
    BSP_AXI_SECT_TYPE_BUTTOM
}BSP_AXI_SECT_TYPE_E;


/* 内存段属性 */
typedef enum tagBSP_DDR_SECT_ATTR_E
{
    BSP_DDR_SECT_ATTR_CACHEABLE = 0x0,
    BSP_DDR_SECT_ATTR_NONCACHEABLE,
    BSP_DDR_SECT_ATTR_BUTTOM
}BSP_DDR_SECT_ATTR_E;


/* 定义虚实地址是否相同的枚举 */
typedef enum tagBSP_DDR_SECT_PVADDR_E
{
    BSP_DDR_SECT_PVADDR_EQU = 0x0,
    BSP_DDR_SECT_PVADDR_NOT_EQU,
    BSP_DDR_SECT_PVADDR_BUTTOM
}BSP_DDR_SECT_PVADDR_E;

/* 定义所有需要查询的MEM类型 */
typedef enum tagBSP_DDR_TYPE_E
{
    BSP_DDR_TYPE_DDR_GU = 0x0,          /*DDR:GU预留*/
    BSP_DDR_TYPE_DDR_TLPHY_IMAGE,       /*DDR:TL DSP 镜像*/
    BSP_DDR_TYPE_DDR_LPHY_SDR,          /*DDR:SDR小区搜索*/
    BSP_DDR_TYPE_DDR_TLPHY_LCS,         /*DDR:TL DSP LCS*/
    BSP_DDR_TYPE_DDR_TLPHY_BANDNV,      /*DDR:TL DSP BAND*/
    BSP_DDR_TYPE_DDR_TPHY_TABLE,        /*DDR:TL TDS TABLE*/
    BSP_DDR_TYPE_DDR_HIFI,              /*DDR:HIFI*/
    BSP_DDR_TYPE_SRAM_TLPHY,            /*SRAM:TL DSP*/
    BSP_DDR_TYPE_SRAM_RTT_SLEEP_FLAG,   /*SRAM:TL DSP睡眠唤醒标记*/
    BSP_DDR_TYPE_SHM_TEMPERATURE,       /*共享内存:温饱*/
    BSP_DDR_TYPE_SHM_BBE16_HIFI_HEAD,   /*共享内存:BBE16->HIFI邮箱头*/
    BSP_DDR_TYPE_SHM_HIFI_BBE16_HEAD,   /*共享内存:HIFI->BBE16邮箱头*/
    BSP_DDR_TYPE_SHM_BBE16_HIFI_QUEUE,  /*共享内存:BBE16->HIFI邮箱队列*/
    BSP_DDR_TYPE_SHM_HIFI_BBE16_QUEUE,  /*共享内存:HIFI->BBE16邮箱队列*/
    BSP_DDR_TYPE_SHM_TIMESTAMP,         /*DDR:LDSP LP*/
    BSP_DDR_TYPE_SHM_AFC_TEMPCOMP,      /*LPHY温保与频偏*/
    BSP_DDR_TYPE_SRAM_MAILBOX_PROTECT_FLG,   /*LPHY邮箱保护标志*/

    BSP_DDR_TYPE_BUTTOM
}BSP_DDR_TYPE_E;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/* DDR内存段的查询结构 */
typedef struct tagBSP_DDR_SECT_QUERY
{
    BSP_DDR_SECT_TYPE_E     enSectType;
    BSP_DDR_SECT_ATTR_E     enSectAttr;
    BSP_DDR_SECT_PVADDR_E   enPVAddr;
    BSP_U32                 ulSectSize;
}BSP_DDR_SECT_QUERY;


/* DDR内存段的详细信息 */
typedef struct tagBSP_DDR_SECT_INFO
{
    BSP_DDR_SECT_TYPE_E    enSectType;
    BSP_DDR_SECT_ATTR_E    enSectAttr;
    BSP_VOID *             pSectVirtAddr;
    BSP_VOID *             pSectPhysAddr;
    BSP_U32                ulSectSize;
}BSP_DDR_SECT_INFO;


/* AXI内存段的详细信息 */
typedef struct tagBSP_AXI_SECT_INFO
{
    BSP_VOID *             pSectVirtAddr;
    BSP_VOID *             pSectPhysAddr;
    BSP_AXI_SECT_TYPE_E    enSectType;
    BSP_U32                ulSectSize;
}BSP_AXI_SECT_INFO;

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/* MEMORY ADDR INFO */
typedef struct BSP_MEMADDR_INFO_S
{
    BSP_DDR_TYPE_E         mem_type;
    BSP_U32                mem_base;
    BSP_U32                mem_size;
}BSP_MEMADDR_INFO;
#endif

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
/*****************************************************************************
 函 数 名  : BSP_DDR_GetSectInfo
 功能描述  : DDR内存段查询接口
 输入参数  : pstSectQuery: 需要查询的内存段类型、属性
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_DDR_GetSectInfo(BSP_DDR_SECT_QUERY *pstSectQuery, BSP_DDR_SECT_INFO *pstSectInfo);
#define DRV_GET_FIX_DDR_ADDR(pstSectQuery, pstSectInfo) BSP_DDR_GetSectInfo(pstSectQuery, pstSectInfo)

/*****************************************************************************
 函 数 名  : BSP_AXI_GetSectInfo
 功能描述  : AXI内存段查询接口
 输入参数  : enSectType: 需要查询的内存段类型
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_AXI_GetSectInfo(BSP_AXI_SECT_TYPE_E enSectType, BSP_AXI_SECT_INFO *pstSectInfo);
#define DRV_GET_FIX_AXI_ADDR(enSectType, pstSectInfo) BSP_AXI_GetSectInfo(enSectType, pstSectInfo)

/*****************************************************************************
 函 数 名  : BSP_GetMemAddrSize
 功能描述  : 获取mem地址和大小信息
 输入参数  : enMemType: 需要查询的mem类型
 输出参数  : addr 地址
             size 大小
 返回值    ：BSP_OK    查询成功
             BSP_ERROR 查询失败
*****************************************************************************/
BSP_S32 BSP_GetMemAddrSize(BSP_DDR_TYPE_E enDdrType, BSP_U32 *addr, BSP_U32 *size);
#define DRV_GET_MEM_ADDR_SIZE(enDdrType, pAddr, pSize)  BSP_GetMemAddrSize(enDdrType, pAddr, pSize)

/*****************************************************************************
 函 数 名  : DRV_DDR_VIRT_TO_PHY
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : pVirtAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
BSP_VOID * DRV_DDR_VIRT_TO_PHY(BSP_VOID * pVirtAddr);

/*****************************************************************************
 函 数 名  : DRV_DDR_PHY_TO_VIRT
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : pPhyAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
BSP_VOID * DRV_DDR_PHY_TO_VIRT(BSP_VOID * pPhyAddr);


/*****************************************************************************
 函 数 名  : TTF_VIRT_TO_PHY
 功能描述  : TTF内存虚地址往实地址转换
 输入参数  : pVirtAddr虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
extern BSP_VOID * TTF_VIRT_TO_PHY(BSP_VOID * pVirtAddr);

/*****************************************************************************
 函 数 名  : TTF_PHY_TO_VIRT
 功能描述  : TTF内存虚地址往实地址转换
 输入参数  : pPhyAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
extern BSP_VOID * TTF_PHY_TO_VIRT(BSP_VOID * pPhyAddr);

/*****************************************************************************
 函 数 名  : IPF_VIRT_TO_PHY
 功能描述  : IPF寄存器虚地址往实地址转换
 输入参数  : pVirtAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
extern BSP_VOID *IPF_VIRT_TO_PHY(BSP_VOID * pVirtAddr);

/*****************************************************************************
 函 数 名  : IPF_PHY_TO_VIRT
 功能描述  : IPF寄存器虚地址往实地址转换
 输入参数  : pPhyAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
extern BSP_VOID * IPF_PHY_TO_VIRT(BSP_VOID * pPhyAddr);

/*****************************************************************************
 函 数 名  : DRV_AXI_VIRT_TO_PHY
 功能描述  : AXI内虚地址往实地址转换
 输入参数  : pVirtAddr；虚地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
extern BSP_VOID * DRV_AXI_VIRT_TO_PHY(BSP_VOID * pVirtAddr);

/*****************************************************************************
 函 数 名  : DRV_AXI_PHY_TO_VIRT
 功能描述  : AXI内实地址往虚地址转换
 输入参数  : pPhyAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
extern BSP_VOID * DRV_AXI_PHY_TO_VIRT(BSP_VOID * pPhyAddr);



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of drv_mem_remap.h */

