/*
 * drivers/hisi/load.c
 *
 * Copyright (C) 2011 Hisilicon, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <soc_peri_sctrl_interface.h>
#include <soc_baseaddr_interface.h>
#include <MemoryMap.h>
#include <global_ddr_map.h>
#include "k3v3_mmc_raw.h"
#include <reset_secure.h>
#include <hi6xxx_loadmodem.h>

typedef unsigned char DSP_IMAGE_SEC_TYPE_ENUM_UINT8;
/*****************************************************************************
 实 体 名  : DSP_IMAGE_SEC_LOAD_ENUM
 功能描述  : 镜像段加载属性
*****************************************************************************/
enum DSP_IMAGE_SEC_LOAD_ENUM
{
    DSP_IMAGE_SEC_LOAD_STATIC           = 0,    /* 静态加载, 仅加载到运行地址 */
    DSP_IMAGE_SEC_LOAD_DYNAMIC          = 1,    /* 动态加载, 同时加载到缓存buffer和运行地址 */
    DSP_IMAGE_SEC_LOAD_BUFFER           = 2,    /* 动态加载, 仅加载到缓存buffer */
    DSP_IMAGE_SEC_LOAD_MODEM_ENTRY      = 3,    /* 静态加载，且为MODEM运行起始地址 */

    DSP_IMAGE_SEC_LOAD_BUTT
};
typedef unsigned char DSP_IMAGE_SEC_LOAD_ENUM_UINT8;

/*****************************************************************************
 实 体 名  : DSP_IMAGE_SEC_STRU
 功能描述  : 镜像段定义
*****************************************************************************/
typedef struct _DSP_IMAGE_SEC_STRU_TAG
{
    unsigned short                      uhwSn;              /* 编号 */
    DSP_IMAGE_SEC_TYPE_ENUM_UINT8       enType;             /* 类型 */
    DSP_IMAGE_SEC_LOAD_ENUM_UINT8       enLoadAttrib;       /* 加载属性 */
    unsigned int                        uwSrcOffset;        /* 在文件中的偏移, bytes */
    unsigned int                        uwDesOffset;        /* 加载目的地址, bytes */
    unsigned int                        uwSize;             /* 段长度, bytes */

}DSP_IMAGE_SEC_STRU;


typedef struct _MODEM_IMAGE_BUFFER_FLAG_STRU_TAG
{
    unsigned int                        ulProtectWord1;     /* 保护字 */
    unsigned int                        ulSecNum;           /* 段个数 */
    unsigned int                        ulHeadAddr;         /* 段头起始地址 */
    unsigned int                        ulProtectWord2;     /* 保护字 */

}MODEM_IMAGE_BUFFER_FLAG_STRU;

#define MCU_SECTION_NUMBER_MAX 30
#define DSP_SECTION_NUMBER_MAX MCU_SECTION_NUMBER_MAX

/*****************************************************************************
 实 体 名  : DSP_IMAGE_HEAD_STRU
 功能描述  : 镜像文件头定义
*****************************************************************************/
typedef struct _DSP_IMAGE_HEAD_STRU_TAG
{
    char                                szTimeStamp[24];                        /* 镜像编译时间 */
    unsigned int                        uwImageSize;                            /* 镜像文件大小, bytes */
    unsigned int                        uwSecNum;                               /* 文件包含段数目 */
    DSP_IMAGE_SEC_STRU                  astSections[DSP_SECTION_NUMBER_MAX];    /* 段信息, 共uwSecNum个 */

}DSP_IMAGE_HEAD_STRU;

#define SOC_SC_OFF_BASE_ADDR SOC_PERI_SCTRL_BASE_ADDR

#define GUDSP_IMAGE_SECTION_MAX (3)
#define GUDSP_IMAGE_BUFFER_ADDR (ECS_GUDSP_LOAD_ADDR)
#define GUDSP_IMAGE_BUFFER_SIZE (ECS_GUDSP_LOAD_SIZE)
#define GUDSP_IMAGE_FLAG_ADDR (GUDSP_IMAGE_BUFFER_ADDR)
#define GUDSP_IMAGE_FLAG_SIZE (16)
#define GUDSP_IMAGE_HEAD_ADDR (GUDSP_IMAGE_FLAG_ADDR + GUDSP_IMAGE_FLAG_SIZE)
#define GUDSP_IMAGE_HEAD_SIZE (GUDSP_IMAGE_SECTION_MAX * sizeof(DSP_IMAGE_SEC_STRU))
#define GUDSP_IMAGE_BIN_ADDR (GUDSP_IMAGE_HEAD_ADDR + GUDSP_IMAGE_HEAD_SIZE)


static int __initdata async_loadmodem_flag = 0;
static int __init enable_async_loadmodem(char *__unused)
{
    async_loadmodem_flag = 1;
    return 0;
}

static int is_async_loadmodem(void)
{
    return async_loadmodem_flag;
}

early_param("async_loadmodem", enable_async_loadmodem);

static void start_modem(void)
{
    char __iomem *sc_off_baseaddr  = (void *)ioremap_nocache(SOC_SC_OFF_BASE_ADDR, 0x2000);
    unsigned int reg_value;

    /* enable MCPU core/localbus clk*/
    writel(0xf, SOC_PERI_SCTRL_SC_PERIPH_CLKEN9_ADDR(sc_off_baseaddr));

    writel(1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_lbus_START,
           (void __iomem *)SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_ADDR(sc_off_baseaddr));

    writel(((1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_dap_START)
            | (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_atb_START)
            | (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_por_START)
            | (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_core_START)
            | (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_dbg_START)
            | (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_l2_START)
            | (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_socdbg_START)
            | (1 << SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_periph_rsdist9_carm_etm_START)),
            SOC_PERI_SCTRL_SC_PERIPH_RSTDIS9_ADDR(sc_off_baseaddr));

    /* carm debug上电 */
    reg_value = readl(SOC_PERI_SCTRL_SC_CARM_CTRL0_ADDR(sc_off_baseaddr));
    reg_value |= (1 << SOC_PERI_SCTRL_SC_CARM_CTRL0_carm_dbgpwrup_START);
    writel(reg_value, SOC_PERI_SCTRL_SC_CARM_CTRL0_ADDR(sc_off_baseaddr));

    iounmap(sc_off_baseaddr);
}

extern int platform_ddr_protect_init(int flag);

int __init mmc_read_done(void)
{
    if (!is_async_loadmodem())
        return 0;

    if (raw_mmc_read_async_done()){
        printk(KERN_ERR "%s:raw_mmc_read_async_done error. \n",__FUNCTION__);
        return -1;
    }
    /* loading finished */
    return 0;
}

int __init loadmodem_init(void)
{
    #define MODEM_IMAGE_ADDR (0x3E00)
    unsigned int buff_phy = MODEM_IMAGE_ADDR;
    DSP_IMAGE_HEAD_STRU *header = NULL;
    MODEM_IMAGE_BUFFER_FLAG_STRU *buffer_flag = NULL;
    char *modem_loadaddr = NULL;
    int ret, i;
    char *tmp_addr = NULL;
    char *modem_baseaddr = NULL;
    char *gudsp_load_addr = NULL;
    char *gudsp_flag_addr = NULL;
    char *gudsp_head_addr = NULL;
    char *gudsp_bin_addr = NULL;
    char *modem_sec_imageaddr = NULL;
    unsigned int head_len = 0;
    unsigned int bin_len = 0;

    if (!is_async_loadmodem())
        return 0;

    /* Dont protect modem memory */
    /*platform_ddr_protect_init(0);*/

    modem_baseaddr = (char *)ioremap_nocache(MODEM_SYS_MEM_ADDR, MODEM_SYS_MEM_SIZE);
    gudsp_load_addr = (char *)ioremap_nocache(ECS_GUDSP_LOAD_ADDR, ECS_GUDSP_LOAD_SIZE);
    gudsp_flag_addr = gudsp_load_addr;
    gudsp_head_addr = gudsp_flag_addr + GUDSP_IMAGE_FLAG_SIZE;
    gudsp_bin_addr = gudsp_head_addr + GUDSP_IMAGE_HEAD_SIZE;
    modem_sec_imageaddr = modem_baseaddr + MODEM_IMAGE_ADDR - VRL_SIZE;

    buffer_flag = (MODEM_IMAGE_BUFFER_FLAG_STRU*)gudsp_flag_addr;
    buffer_flag->ulProtectWord1 = 0x5A5A5A5A;
    buffer_flag->ulSecNum = 0;
    buffer_flag->ulHeadAddr = (unsigned int)GUDSP_IMAGE_HEAD_ADDR;
    buffer_flag->ulProtectWord2 = 0xA5A5A5A5;

    modem_loadaddr = modem_baseaddr + MODEM_IMAGE_ADDR;
    header = (DSP_IMAGE_HEAD_STRU *)modem_loadaddr;

    /*printk(KERN_ERR "%s:modem_loadaddr: %p modem_baseaddr:%p \n",__FUNCTION__, modem_loadaddr, modem_baseaddr);*/


    /* secure check */
    if (check_secure_mode()){
        ret = check_image_in_ram((unsigned int *)modem_sec_imageaddr);
        if (ret == 0){
            printk(KERN_INFO "%s:secure check ok \n",__FUNCTION__);
        }else{
            printk(KERN_ERR "%s:secure check error \n",__FUNCTION__);
            goto error;
        }
    }

    for(i = 0; i < header->uwSecNum; i++)
    {

        /* If attribute is BUFFER, move to buffer */
        if (header->astSections[i].enLoadAttrib == DSP_IMAGE_SEC_LOAD_BUFFER )
        {
            if((GUDSP_IMAGE_HEAD_SIZE < head_len + sizeof(DSP_IMAGE_SEC_STRU))
                || (GUDSP_IMAGE_BUFFER_SIZE < bin_len + header->astSections[i].uwSize)
                || (GUDSP_IMAGE_SECTION_MAX <= buffer_flag->ulSecNum))
            {
                printk(KERN_ERR "load to buffer failed, section's size beyond buffer's size.\n");
                return -1;
            }

            memcpy((void*)(gudsp_bin_addr + bin_len), (void*)(modem_loadaddr + header->astSections[i].uwSrcOffset),
                                        header->astSections[i].uwSize);
            header->astSections[i].uwSrcOffset = (unsigned int)GUDSP_IMAGE_BIN_ADDR + bin_len;
            memcpy((void*)(gudsp_head_addr + head_len), (void*)(&header->astSections[i]),
                                        sizeof(DSP_IMAGE_SEC_STRU));

            head_len += sizeof(DSP_IMAGE_SEC_STRU);
            bin_len += header->astSections[i].uwSize;
            buffer_flag->ulSecNum++;

            //printk(KERN_ERR "%s:section buffer: %p\n",__FUNCTION__,tmp_addr);
        }
        /* If attribute is STATIC/MODEM_ENTRY, move to addr to run */
        else if ((header->astSections[i].enLoadAttrib == DSP_IMAGE_SEC_LOAD_MODEM_ENTRY )
            || (header->astSections[i].enLoadAttrib == DSP_IMAGE_SEC_LOAD_STATIC ))
        {
            tmp_addr = modem_baseaddr + header->astSections[i].uwDesOffset;
            printk(KERN_ERR "%s:section: %p \n",__FUNCTION__, tmp_addr);
            if (header->astSections[i].uwDesOffset != buff_phy + header->astSections[i].uwSrcOffset){
                memmove(tmp_addr,
                   (modem_loadaddr + header->astSections[i].uwSrcOffset),
                    header->astSections[i].uwSize);
                //printk(KERN_ERR "%s:copy %p to %p size 0x%x \n",__FUNCTION__, (buff_phy + header->astSections[i].uwSrcOffset), header->astSections[i].uwDesOffset,header->astSections[i].uwSize);
                //printk(KERN_ERR "%s:copy %p to %p size 0x%x \n",__FUNCTION__, (modem_loadaddr + header->astSections[i].uwSrcOffset), tmp_addr, header->astSections[i].uwSize);
            }
        }
        /* else do nothing */
        else
        {
            /* do nothing */
        }

    }
    printk(KERN_ERR "Load modem done.\n");
    /* unreset modem */
    start_modem();
error:
    iounmap(modem_baseaddr);
    iounmap(gudsp_load_addr);

    platform_ddr_protect_init(1);
    return 0;
}


//arch_initcall(loadmodem_init);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("kernel load modem");
