/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_mem_remap.c
  版 本 号   : 初稿
  作    者   : c61362
  生成日期   : 2012年3月2日
  最近修改   :
  功能描述   : 底软给上层软件封装的接口层
  修改历史   :
  1.日    期   : 2012年3月2日
    作    者   : c61362
    修改内容   : 新建Drvinterface.c

  2.日    期   : 2013年2月19日
    作    者   : 蔡喜 220237
    修改内容   : 由Drvinterface.c拆分所得

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "BSP.h"
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#include "sys_config.h"
#include "string.h"
#include "drv_mailbox_cfg.h"
#endif

#if (defined BSP_CORE_APP)
#include "soc_irqs.h"
#include <linux/module.h>
#include "soc_baseaddr_interface.h"
#include "soc_timer_interface.h"
#if defined (CHIP_BB_HI6210)
#include "soc_peri_sctrl_interface.h"
#elif defined (CHIP_BB_HI6620)
#include "soc_sctrl_interface.h"
#endif
#include "MemoryMap.h"
#include <linux/dma-mapping.h>
#if defined (CHIP_BB_HI6210)
#include <linux/hisi/hi6xxx-iomap.h>                   /* For IO_ADDRESS access */
#elif defined (CHIP_BB_HI6620)
#include <linux/hisi/hi6620-iomap.h>                   /* For IO_ADDRESS access */
#endif
#endif

#include <soc_ao_sctrl_interface.h>
#include "general_sram_map.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern int printf(const char *format, ...);

#define NOT_EXIST 0
/*****************************************************************************
  2 函数声明
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/* 全局的结构体，记录DDR内存段分配信息 */
BSP_DDR_SECT_INFO g_stDdrSectInfo[] = {
    {BSP_DDR_SECT_TYPE_TTF,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST,              (BSP_VOID *)NOT_EXIST,            NOT_EXIST},
    {BSP_DDR_SECT_TYPE_ARMDSP,    BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST,    (BSP_VOID *)NOT_EXIST, NOT_EXIST},
    {BSP_DDR_SECT_TYPE_UPA,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)ECS_UPA_BASE_ADDR,               (BSP_VOID *)ECS_UPA_BASE_ADDR,            ECS_UPA_SIZE},
    {BSP_DDR_SECT_TYPE_CQI,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)ECS_CQI_BASE_ADDR,               (BSP_VOID *)ECS_CQI_BASE_ADDR,            ECS_CQI_SIZE},
    {BSP_DDR_SECT_TYPE_APT,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)ECS_APT_BASE_ADDR,               (BSP_VOID *)ECS_APT_BASE_ADDR,            ECS_APT_SIZE},
    {BSP_DDR_SECT_TYPE_ET,        BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)ECS_ET_BASE_ADDR,                (BSP_VOID *)ECS_ET_BASE_ADDR,             ECS_ET_SIZE},
    {BSP_DDR_SECT_TYPE_BBPMASTER, BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)ECS_BBP_MASTER_BASE_ADDR,        (BSP_VOID *)ECS_BBP_MASTER_BASE_ADDR,     ECS_BBP_MASTER_SIZE},
    {BSP_DDR_SECT_TYPE_NV,        BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)VIRT_ADDRESS(ECS_NV_BASE_ADDR),  (BSP_VOID *)ECS_NV_BASE_ADDR,             ECS_NV_SIZE},
    {BSP_DDR_SECT_TYPE_DICC,      BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)VIRT_ADDRESS(ECS_TTF_DICC_ADDR), (BSP_VOID *)ECS_TTF_DICC_ADDR,            ECS_TTF_DICC_SIZE},
    {BSP_DDR_SECT_TYPE_WAN,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)CORESHARE_MEM_WAN_ADDR,          (BSP_VOID *)CORESHARE_MEM_WAN_ADDR,       CORESHARE_MEM_WAN_SIZE},
    {BSP_DDR_SECT_TYPE_SHARE_MEM, BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)GLOBAL_MEM_CORE_SHARE_ADDR,      (BSP_VOID *)GLOBAL_MEM_CORE_SHARE_ADDR,   GLOBAL_MEM_CORE_SHARE_SIZE},
    {BSP_DDR_SECT_TYPE_EXCP,      BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)GLOBAL_MEM_EXCP_BASE_ADDR,       (BSP_VOID *)GLOBAL_MEM_EXCP_BASE_ADDR,    GLOBAL_MEM_EXCP_SIZE},
    {BSP_DDR_SECT_TYPE_HIFI,      BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)GLOBAL_MEM_HIFI_BASE_ADDR,       (BSP_VOID *)GLOBAL_MEM_HIFI_BASE_ADDR,    GLOBAL_MEM_HIFI_SIZE},
    {BSP_DDR_SECT_TYPE_HUTAF,     BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)HUTAF_HLT_MEM_ADDR,              (BSP_VOID *)HUTAF_HLT_MEM_ADDR,           HUTAF_HLT_MEM_SIZE},
    {BSP_DDR_SECT_TYPE_ZSP_UP,    BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)ZSP_UP_ADDR,                     (BSP_VOID *)ZSP_UP_ADDR,                  ZSP_UP_SIZE},
    {BSP_DDR_SECT_TYPE_TDS_LH2,   BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)GLOBAL_MEM_TDS_TABLE_ADDR,       (BSP_VOID *)GLOBAL_MEM_TDS_TABLE_ADDR,    GLOBAL_MEM_TDS_TABLE_SIZE},
    {BSP_DDR_SECT_TYPE_BUTTOM,    0,                              NULL,                                         NULL,                                    0}
};

/* 全局的结构体，记录AXI内存段分配信息 */
BSP_AXI_SECT_INFO g_stAxiSectInfo[] = {
    {(BSP_VOID *)MEMORY_AXI_IFC_ADDR,               (BSP_VOID *)MEMORY_AXI_IFC_ADDR,          BSP_AXI_SECT_TYPE_IFC,                              MEMORY_AXI_IFC_SIZE},
    {(BSP_VOID *)MEMORY_AXI_ICC_ADDR,               (BSP_VOID *)MEMORY_AXI_ICC_ADDR,          BSP_AXI_SECT_TYPE_ICC,                              MEMORY_AXI_ICC_SIZE},
    {(BSP_VOID *)MEMORY_AXI_MEMMGR_FLAG_ADDR,       (BSP_VOID *)MEMORY_AXI_MEMMGR_FLAG_ADDR,  MEMORY_AXI_MEMMGR_FLAG_SIZE},
    {NULL,                                           NULL,                                    BSP_AXI_SECT_TYPE_DYNAMIC,                          0},
    {(BSP_VOID *)MEMORY_AXI_IPF_ADDR,               (BSP_VOID *)MEMORY_AXI_IPF_ADDR,          BSP_AXI_SECT_TYPE_IPF,                              MEMORY_AXI_IPF_SIZE},
    {(BSP_VOID *)MEMORY_AXI_TEMP_PROTECT_ADDR,      (BSP_VOID *)MEMORY_AXI_TEMP_PROTECT_ADDR, BSP_AXI_SECT_TYPE_TEMPERATURE,                      MEMORY_AXI_TEMP_PROTECT_SIZE},
    {(BSP_VOID *)MEMORY_AXI_DICC_ADDR,              (BSP_VOID *)MEMORY_AXI_DICC_ADDR,         BSP_AXI_SECT_TYPE_DICC,   MEMORY_AXI_DICC_SIZE},
    {(BSP_VOID *)MEMORY_AXI_HIFI_ADDR,              (BSP_VOID *)MEMORY_AXI_HIFI_ADDR,         BSP_AXI_SECT_TYPE_HIFI,                             MEMORY_AXI_HIFI_SIZE},
    {(BSP_VOID *)MEMORY_AXI_DDR_CAPACITY_ADDR,      (BSP_VOID *)MEMORY_AXI_DDR_CAPACITY_ADDR, BSP_AXI_SECT_TYPE_DDR_CAPACITY,                     MEMORY_AXI_DDR_CAPACITY_SIZE},
    {(BSP_VOID *)MEMORY_AXI_RESERVE_ADDR,           (BSP_VOID *)MEMORY_AXI_RESERVE_ADDR,      BSP_AXI_SECT_TYPE_RESERVE,                          MEMORY_AXI_RESERVE_SIZE},
    {NULL,                                           NULL,                                    BSP_AXI_SECT_TYPE_BUTTOM,                           0}
};

BSP_MEMADDR_INFO g_stMemInfo[] = {
                {BSP_DDR_TYPE_DDR_GU,               GLOBAL_MEM_GU_RESERVED_ADDR,            GLOBAL_MEM_GU_RESERVED_SIZE},
                {BSP_DDR_TYPE_DDR_TLPHY_IMAGE,      GLOBAL_MEM_LT_IMAGE_ADDR,               GLOBAL_MEM_LT_IMAGE_SIZE},
                {BSP_DDR_TYPE_DDR_LPHY_SDR,         GLOBAL_MEM_SDR_ADDR,                    GLOBAL_MEM_SDR_SIZE},
                {BSP_DDR_TYPE_DDR_TLPHY_LCS,        GLOBAL_MEM_LCS_ADDR,                    GLOBAL_MEM_LCS_SIZE},
                {BSP_DDR_TYPE_DDR_TLPHY_BANDNV,     CORESHARE_MEM_TENCILICA_MULT_BAND_ADDR, CORESHARE_MEM_TENCILICA_MULT_BAND_SIZE},
                {BSP_DDR_TYPE_DDR_TPHY_TABLE,       GLOBAL_MEM_TDS_TABLE_ADDR,              GLOBAL_MEM_TDS_TABLE_SIZE},
                {BSP_DDR_TYPE_DDR_HIFI,             GLOBAL_MEM_HIFI_BASE_ADDR,              GLOBAL_MEM_HIFI_SIZE},
                {BSP_DDR_TYPE_SRAM_TLPHY,           MEMORY_AXI_TDL_ADDR,                    MEMORY_AXI_TDL_SIZE},
                {BSP_DDR_TYPE_SRAM_RTT_SLEEP_FLAG,  SRAM_RTT_SLEEP_FLAG_ADDR,               SRAM_RTT_SLEEP_FLAG_SIZE},
                {BSP_DDR_TYPE_SHM_TEMPERATURE,      MCU_HKADC_DDR_ADDR,                     MCU_HKADC_DDR_SIZE},
                {BSP_DDR_TYPE_SHM_BBE16_HIFI_HEAD,  MAILBOX_HEAD_ADDR(BBE16, HIFI, MSG),    MAILBOX_HEAD_LEN},
                {BSP_DDR_TYPE_SHM_HIFI_BBE16_HEAD,  MAILBOX_HEAD_ADDR(HIFI, BBE16, MSG),    MAILBOX_HEAD_LEN},
                {BSP_DDR_TYPE_SHM_BBE16_HIFI_QUEUE, MAILBOX_QUEUE_ADDR(BBE16, HIFI, MSG),   MAILBOX_QUEUE_SIZE(BBE16, HIFI, MSG)},
                {BSP_DDR_TYPE_SHM_HIFI_BBE16_QUEUE, MAILBOX_QUEUE_ADDR(HIFI, BBE16, MSG),   MAILBOX_QUEUE_SIZE(HIFI, BBE16, MSG)},
                {BSP_DDR_TYPE_SHM_TIMESTAMP,      SHM_TIMESTAMP_ADDR,                                SHM_TIMESTAMP_SIZE},
                {BSP_DDR_TYPE_SHM_AFC_TEMPCOMP,      SHM_MEM_AFC_TEMPCOMP_ADDR,      SHM_MEM_AFC_TEMPCOMP_SIZE},
                {BSP_DDR_TYPE_SRAM_MAILBOX_PROTECT_FLG,      MEMORY_AXI_LDSP_AWAKE_ADDR,      MEMORY_AXI_LDSP_AWAKE_SIZE}
};

#endif

#if (defined BSP_CORE_APP)
/* 全局的结构体，记录DDR内存段分配信息 */
BSP_DDR_SECT_INFO g_stDdrSectInfo[] = {
    {BSP_DDR_SECT_TYPE_TTF,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)NOT_EXIST,            NOT_EXIST},
    {BSP_DDR_SECT_TYPE_ARMDSP,    BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)NOT_EXIST, NOT_EXIST},
    {BSP_DDR_SECT_TYPE_UPA,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_UPA_BASE_ADDR,            ECS_UPA_SIZE},
    {BSP_DDR_SECT_TYPE_CQI,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_CQI_BASE_ADDR,            ECS_CQI_SIZE},
    {BSP_DDR_SECT_TYPE_APT,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_APT_BASE_ADDR,            ECS_APT_SIZE},
    {BSP_DDR_SECT_TYPE_ET,        BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_ET_BASE_ADDR,             ECS_ET_SIZE},
    {BSP_DDR_SECT_TYPE_BBPMASTER, BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_BBP_MASTER_BASE_ADDR,     ECS_BBP_MASTER_SIZE},
    {BSP_DDR_SECT_TYPE_NV,        BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_NV_BASE_ADDR,             ECS_NV_SIZE},
    {BSP_DDR_SECT_TYPE_DICC,      BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)ECS_TTF_DICC_ADDR,            ECS_TTF_DICC_SIZE},
    {BSP_DDR_SECT_TYPE_WAN,       BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)CORESHARE_MEM_WAN_ADDR,       CORESHARE_MEM_WAN_SIZE},
    {BSP_DDR_SECT_TYPE_SHARE_MEM, BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)GLOBAL_MEM_CORE_SHARE_ADDR,   GLOBAL_MEM_CORE_SHARE_SIZE},
    {BSP_DDR_SECT_TYPE_EXCP,      BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)GLOBAL_MEM_EXCP_BASE_ADDR,    GLOBAL_MEM_EXCP_SIZE},
    {BSP_DDR_SECT_TYPE_HIFI,      BSP_DDR_SECT_ATTR_NONCACHEABLE, (BSP_VOID *)NOT_EXIST, (BSP_VOID *)GLOBAL_MEM_HIFI_BASE_ADDR,    GLOBAL_MEM_HIFI_SIZE},
    {BSP_DDR_SECT_TYPE_TDS_LH2,   BSP_DDR_SECT_ATTR_NONCACHEABLE, NULL,                   NULL,                            0},
    {BSP_DDR_SECT_TYPE_BUTTOM,    0,                              NULL,                   NULL,                         0}
};

/* 全局的结构体，记录AXI内存段分配信息 */
BSP_AXI_SECT_INFO g_stAxiSectInfo[] = {
    {NULL,(BSP_VOID *)MEMORY_AXI_IFC_ADDR,              BSP_AXI_SECT_TYPE_IFC,                          MEMORY_AXI_IFC_SIZE},
    {NULL,(BSP_VOID *)MEMORY_AXI_ICC_ADDR,              BSP_AXI_SECT_TYPE_ICC,                          MEMORY_AXI_ICC_SIZE},
    {NULL,(BSP_VOID *)MEMORY_AXI_MEMMGR_FLAG_ADDR,      BSP_AXI_SECT_TYPE_MEMMGR_FLAG,                  MEMORY_AXI_MEMMGR_FLAG_SIZE},
    {NULL,            NULL,                             BSP_AXI_SECT_TYPE_DYNAMIC,                      0},
    {NULL,(BSP_VOID *)MEMORY_AXI_IPF_ADDR,              BSP_AXI_SECT_TYPE_IPF,                          MEMORY_AXI_IPF_SIZE},
    {NULL,(BSP_VOID *)MEMORY_AXI_TEMP_PROTECT_ADDR,     BSP_AXI_SECT_TYPE_TEMPERATURE,                  MEMORY_AXI_TEMP_PROTECT_SIZE},
    {NULL,(BSP_VOID *)MEMORY_AXI_DICC_ADDR,             BSP_AXI_SECT_TYPE_DICC,                         MEMORY_AXI_DICC_SIZE},
    {NULL,(BSP_VOID *)MEMORY_AXI_HIFI_ADDR,             BSP_AXI_SECT_TYPE_HIFI,                         MEMORY_AXI_HIFI_SIZE},
    {NULL,(BSP_VOID *)MEMORY_AXI_DDR_CAPACITY_ADDR,     BSP_AXI_SECT_TYPE_DDR_CAPACITY,                 MEMORY_AXI_DDR_CAPACITY_SIZE},
    {NULL,(BSP_VOID *)MEMORY_AXI_RESERVE_ADDR,          BSP_AXI_SECT_TYPE_RESERVE,                      MEMORY_AXI_RESERVE_SIZE},
    {NULL,            NULL,                             BSP_AXI_SECT_TYPE_BUTTOM,                                                     0}
};
#endif


/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : BSP_DDR_GetSectInfo
 功能描述  : DDR内存段查询接口
 输入参数  : pstSectQuery: 需要查询的内存段类型、属性
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_DDR_GetSectInfo(BSP_DDR_SECT_QUERY *pstSectQuery, BSP_DDR_SECT_INFO *pstSectInfo)
{
    int i = 0;
    int NUM_TMP = sizeof(g_stDdrSectInfo)/sizeof(g_stDdrSectInfo[0]);
    if((BSP_NULL == pstSectQuery) || (BSP_NULL == pstSectInfo))
    {
        return BSP_ERROR;
    }

    for (i = 0;i < NUM_TMP;i++)
    {

        if (pstSectQuery->enSectType == g_stDdrSectInfo[i].enSectType)
        {
            memcpy((void *)pstSectInfo, (const void *)(&g_stDdrSectInfo[i]), sizeof(BSP_DDR_SECT_INFO));
#if (defined BSP_CORE_APP)
            if (pstSectInfo->pSectPhysAddr != 0)
            {
                pstSectInfo->pSectVirtAddr = (BSP_VOID*)HISI_VA_ADDRESS(pstSectInfo->pSectPhysAddr);
            }
#endif
            return BSP_OK;
        }
    }

    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : BSP_DDR_ShowSectInfo
 功能描述  : 打印DDR内存段信息
 输入参数  : 无
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
BSP_VOID BSP_DDR_ShowSectInfo(BSP_VOID)
{
    int enSectTypeIndex = 0;

    DRV_PRINTF("\ntype       paddr      vaddr      size       attr\n");
    for(; enSectTypeIndex < (sizeof(g_stDdrSectInfo) / sizeof(BSP_DDR_SECT_INFO)); enSectTypeIndex++)
    {
        DRV_PRINTF("0x%-8.8x 0x%p 0x%p 0x%-8.8x 0x%-8.8x\n\n", \
          g_stDdrSectInfo[enSectTypeIndex].enSectType, \
          g_stDdrSectInfo[enSectTypeIndex].pSectPhysAddr, \
          g_stDdrSectInfo[enSectTypeIndex].pSectVirtAddr, \
          g_stDdrSectInfo[enSectTypeIndex].ulSectSize, \
          g_stDdrSectInfo[enSectTypeIndex].enSectAttr);
    }

#if 0
    BSP_DDR_SECT_TYPE_E     enSectTypeIndex = (BSP_DDR_SECT_TYPE_E)0;

    DRV_PRINTF("\ntype       paddr      vaddr      size       attr\n");
    for(; enSectTypeIndex < BSP_DDR_SECT_TYPE_BUTTOM; enSectTypeIndex++)
    {
        DRV_PRINTF("0x%-8.8x 0x%-8.8x 0x%-8.8x 0x%-8.8x 0x%-8.8x\n\n", \
          g_stDdrSectInfo[enSectTypeIndex].enSectType, \
          g_stDdrSectInfo[enSectTypeIndex].ulSectPhysAddr, \
          g_stDdrSectInfo[enSectTypeIndex].ulSectVirtAddr, \
          g_stDdrSectInfo[enSectTypeIndex].ulSectSize, \
          g_stDdrSectInfo[enSectTypeIndex].enSectAttr);
    }
#endif
}

/*****************************************************************************
 函 数 名  : BSP_AXI_GetSectInfo
 功能描述  : AXI内存段查询接口
 输入参数  : enSectType: 需要查询的内存段类型
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_AXI_GetSectInfo(BSP_AXI_SECT_TYPE_E enSectType, BSP_AXI_SECT_INFO *pstSectInfo)
{
    int i = 0;
    int NUM_TMP = sizeof(g_stAxiSectInfo)/sizeof(g_stAxiSectInfo[0]);

    if(BSP_NULL == pstSectInfo)
    {
        return BSP_ERROR;
    }

    for (i = 0;i < NUM_TMP;i++)
    {
        if (enSectType == g_stAxiSectInfo[i].enSectType)
        {
            memcpy((void *)pstSectInfo, (const void *)(&g_stAxiSectInfo[i]), sizeof(BSP_AXI_SECT_INFO));
#if (defined BSP_CORE_APP)
            if (pstSectInfo->pSectPhysAddr != 0)
            {
                pstSectInfo->pSectVirtAddr = (BSP_VOID*)HISI_VA_ADDRESS(pstSectInfo->pSectPhysAddr);
            }
#endif
            return BSP_OK;
        }
    }

    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : BSP_AXI_ShowSectInfo
 功能描述  : 打印AXI内存段信息
 输入参数  : 无
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
BSP_VOID BSP_AXI_ShowSectInfo(BSP_VOID)
{
    int enSectTypeIndex = 0;

    DRV_PRINTF("\ntype       paddr      vaddr      sizer\n");
    for(; enSectTypeIndex < (sizeof(g_stAxiSectInfo) / sizeof(BSP_AXI_SECT_INFO)); enSectTypeIndex++)
    {
        DRV_PRINTF("0x%-8.8x 0x%p 0x%p 0x%-8.8x\n\n", \
          g_stAxiSectInfo[enSectTypeIndex].enSectType, \
          g_stAxiSectInfo[enSectTypeIndex].pSectPhysAddr, \
          g_stAxiSectInfo[enSectTypeIndex].pSectVirtAddr, \
          g_stAxiSectInfo[enSectTypeIndex].ulSectSize);
    }

#if 0
    BSP_AXI_SECT_TYPE_E     enSectTypeIndex = 0;

    DRV_PRINTF("\ntype       paddr      vaddr      sizer\n");
    for(; enSectTypeIndex < BSP_AXI_SECT_TYPE_BUTTOM; enSectTypeIndex++)
    {
        DRV_PRINTF("0x%-8.8x 0x%-8.8x 0x%-8.8x 0x%-8.8x\n\n", \
          g_stAxiSectInfo[enSectTypeIndex].enSectType, \
          g_stAxiSectInfo[enSectTypeIndex].ulSectPhysAddr, \
          g_stAxiSectInfo[enSectTypeIndex].ulSectVirtAddr, \
          g_stAxiSectInfo[enSectTypeIndex].ulSectSize);
    }
#endif
}

#if (defined BSP_CORE_APP)

/*************************内存虚实转换 start*********************************/
/*****************************************************************************
 函 数 名  : DRV_DDR_VIRT_TO_PHY
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
void * DRV_DDR_VIRT_TO_PHY(void* ulVAddr)
{

    return (void*)HISI_PA_ADDRESS((unsigned long)ulVAddr);
}

/*****************************************************************************
 函 数 名  : DRV_DDR_PHY_TO_VIRT
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
void* DRV_DDR_PHY_TO_VIRT(void* ulPAddr)
{
    return (void*)HISI_VA_ADDRESS((unsigned long)ulPAddr);
}

#endif

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/*****************************************************************************
 函 数 名  : BSP_GetMemAddrSize
 功能描述  : 获取mem地址和大小信息
 输入参数  : enMemType: 需要查询的mem类型
 输出参数  : addr 地址
             size 大小
 返回值    ：BSP_OK    查询成功
             BSP_ERROR 查询失败
*****************************************************************************/
BSP_U32 BSP_GetMemAddrSize(BSP_DDR_TYPE_E enDdrType, BSP_U32 *addr, BSP_U32 *size)
{
    BSP_U32 i = 0;
    BSP_U32 ulMemNum = sizeof(g_stMemInfo)/sizeof(g_stMemInfo[0]);

    /*入参检查*/
    if((!addr) || (!size) || (enDdrType >= BSP_DDR_TYPE_BUTTOM))
    {
        return BSP_ERROR;
    }

    for (i = 0;i < ulMemNum;i++)
    {
        if (enDdrType == g_stMemInfo[i].mem_type)
        {
            *addr = g_stMemInfo[i].mem_base;
            *size = g_stMemInfo[i].mem_size;
            return BSP_OK;
        }
    }

    return BSP_ERROR;
}

/*************************内存虚实转换 start*********************************/
/*****************************************************************************
 函 数 名  : DRV_DDR_VIRT_TO_PHY
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : pVirtAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
BSP_VOID * DRV_DDR_VIRT_TO_PHY(BSP_VOID * pVirtAddr)
{
    if((pVirtAddr < (BSP_VOID *)ECS_MDDR_RESERVE_BASE_ADDR_VIRT)
        || (pVirtAddr > (BSP_VOID *)(ECS_MDDR_RESERVE_BASE_ADDR_VIRT + GLOBAL_MEM_GU_RESERVED_SIZE
        + GLOBAL_MEM_CORE_SHARE_SIZE + GLOBAL_MEM_EXCP_SIZE)))
    {
        DRV_PRINTF("DRV_DDR_VIRT_TO_PHY: pVirtAddr is invalid!\n");
        return 0;
    }

    return (BSP_VOID *)((char *)pVirtAddr - ECS_MDDR_RESERVE_BASE_ADDR_VIRT + ECS_MDDR_RESERVE_BASE_ADDR);
}

/*****************************************************************************
 函 数 名  : DRV_DDR_PHY_TO_VIRT
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : pPhyAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
BSP_VOID *DRV_DDR_PHY_TO_VIRT(BSP_VOID *pPhyAddr)
{
    if((pPhyAddr < (BSP_VOID *)ECS_MDDR_RESERVE_BASE_ADDR)
        || (pPhyAddr > (BSP_VOID *)(ECS_MDDR_RESERVE_BASE_ADDR + GLOBAL_MEM_GU_RESERVED_SIZE
        + GLOBAL_MEM_CORE_SHARE_SIZE + GLOBAL_MEM_EXCP_SIZE + LPS_RESERVED_MEM_SIZE)))
    {
        DRV_PRINTF("DRV_DDR_PHY_TO_VIRT: pPhyAddr(0x%p) is invalid!\n",pPhyAddr);
        return 0;
    }

    return (BSP_VOID *)((char *)pPhyAddr - ECS_MDDR_RESERVE_BASE_ADDR + ECS_MDDR_RESERVE_BASE_ADDR_VIRT);
}

#define DATA_ABORT_ADDR 0xF7240000
/*****************************************************************************
 函 数 名  : TTF_VIRT_TO_PHY
 功能描述  : TTF内存虚地址往实地址转换
 输入参数  : pVirtAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
BSP_VOID *TTF_VIRT_TO_PHY(BSP_VOID *pVirtAddr)
{
/*
    查看代码，在v8形态，nas和ttf都不会调用该函数(v9也不会调，只有V3会调用)；
    考虑到在pc工程会调用该函数，故选择用非法地址来打桩；
    后续如果在v8分支编译v3形态的产品，此处还需要调试；
    返回一个空洞地址(PERI_AP的Reserved区), 期望访问该地址时产生data abort或者panic，以便调试时迅速定位
*/
    return (BSP_VOID *)DATA_ABORT_ADDR;
}

/*****************************************************************************
 函 数 名  : TTF_PHY_TO_VIRT
 功能描述  : TTF内存实地址往虚地址转换
 输入参数  : pPhyAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
BSP_VOID *TTF_PHY_TO_VIRT(BSP_VOID *pPhyAddr)
{
/*
    查看代码，在v8形态，nas和ttf都不会调用该函数(v9也不会调，只有V3会调用)；
    考虑到在pc工程会调用该函数，故选择用非法地址来打桩；
    后续如果在v8分支编译v3形态的产品，此处还需要调试；
    返回一个空洞地址(PERI_AP的Reserved区), 期望访问该地址时产生data abort或者panic，以便调试时迅速定位
*/
    return (BSP_VOID *)DATA_ABORT_ADDR;
}

/*****************************************************************************
 函 数 名  : IPF_VIRT_TO_PHY
 功能描述  : IPF寄存器虚地址往实地址转换
 输入参数  : pVirtAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
BSP_VOID *IPF_VIRT_TO_PHY(BSP_VOID *pVirtAddr)
{
    return (BSP_VOID *)((char *)pVirtAddr - IPF_BASE_ADDR_VIRT + IPF_PHY_BASE_ADDR);
}

/*****************************************************************************
 函 数 名  : IPF_PHY_TO_VIRT
 功能描述  : IPF寄存器实地址往虚地址转换
 输入参数  : pPhyAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
BSP_VOID *IPF_PHY_TO_VIRT(BSP_VOID *pPhyAddr)
{
    return (BSP_VOID *)((char *)pPhyAddr - IPF_PHY_BASE_ADDR + IPF_BASE_ADDR_VIRT);
}

/*****************************************************************************
 函 数 名  : DRV_AXI_VIRT_TO_PHY
 功能描述  : AXI内虚地址往实地址转换
 输入参数  : pVirtAddr；虚地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
BSP_VOID *DRV_AXI_VIRT_TO_PHY(BSP_VOID *pVirtAddr)
{
    return (BSP_VOID *)((char *)pVirtAddr - AXI_BASE_ADDR_VIRT + AXI_PHY_BASE_ADDR);
}

/*****************************************************************************
 函 数 名  : DRV_AXI_PHY_TO_VIRT
 功能描述  : AXI内实地址往虚地址转换
 输入参数  : pPhyAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
BSP_VOID * DRV_AXI_PHY_TO_VIRT(BSP_VOID *pPhyAddr)
{
    return (BSP_VOID *)((char *)pPhyAddr - AXI_PHY_BASE_ADDR + AXI_BASE_ADDR_VIRT);
}

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

