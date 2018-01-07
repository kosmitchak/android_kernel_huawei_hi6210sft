/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_edma_enum.h
  版 本 号   : 初稿
  作    者   : 荆耀辉00179452
  生成日期   : 2014年4月10日
  最近修改   :
  功能描述   : drv_edma.h 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年4月10日
    作    者   : 荆耀辉00179452
    修改内容   : 创建文件

******************************************************************************/
#ifndef __DRV_EDMA_ENUM_H__
#define __DRV_EDMA_ENUM_H__


typedef enum _BALONG_DMA_REQ
{
    EDMA_SPI0_RX,
    EDMA_SPI0_TX,
    EDMA_SPI1_RX,
    EDMA_SPI1_TX,
    EDMA_UART1_RX,
    EDMA_UART1_TX,
    EDMA_UART2_RX,
    EDMA_UART2_TX,
    EDMA_UART3_RX,
    EDMA_UART3_TX,
    EDMA_UART4_RX,
    EDMA_UART4_TX,
    EDMA_UART5_RX,
    EDMA_UART5_TX,
    EDMA_SCI0_RX,
    EDMA_SCI0_TX,
    EDMA_SCI1_RX,
    EDMA_SCI1_TX,
    EDMA_SIO0_RX,
    EDMA_SIO0_TX,
    EDMA_SIO1_RX,
    EDMA_SIO1_TX,
    EDMA_SIO2_RX,
    EDMA_SIO2_TX,
    EDMA_SLIMBUS4,
    EDMA_SLIMBUS5,
    EDMA_SLIMBUS6,
    EDMA_SLIMBUS7,
    EDMA_SLIMBUS8,
    EDMA_SLIMBUS9,
    EDMA_SLIMBUS10,
    EDMA_UICC,
    EDMA_MEMORY_1,
    EDMA_MEMORY_2,
    EDMA_MEMORY_3,
    EDMA_MEMORY_4,
    EDMA_MEMORY_DSP_1,
    EDMA_MEMORY_DSP_2,
    EDMA_MEMORY_DSP_3,
    EDMA_MEMORY_DSP_4,
    EDMA_MEMORY_DSP_5,
    EDMA_MEMORY_DSP_6,
    EDMA_MEMORY_DSP_7,
    EDMA_MEMORY_DSP_8,
    EDMA_BBP_GRIF,
    EDMA_BBP_DBG,
    EDMA_MMC0,
    EDMA_MMC1,
    EDMA_I2C_RX,
    EDMA_I2C_TX,
    EDMA_DSP_GRIF,
    EDMA_LOG_CHAN_RESERVED
} BALONG_DMA_REQ;

#define     EDMA_LDSP_NV_LOADING         (2)           /* edma chanl 2 , for LDSP*/
#define     EDMA_LDSP_API_USED_0         (3)           /* edma chanl 3 , for LDSP*/
#define     EDMA_LDSP_API_USED_1         (4)           /* edma chanl 4 , for LDSP*/
#define     EDMA_LDSP_CSU_SDR            (5)           /* edma chanl 5 , for LDSP*/
#define     EDMA_LDSP_EMU_SDR            (6)           /* edma chanl 6, for LDSP*/
#define     EDMA_LDSP_LCS_SDR            (0xffff)      /* 没有实际用途 , stub for LDSP*/

#endif

