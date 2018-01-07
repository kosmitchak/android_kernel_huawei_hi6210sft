/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : debug_func.c
  版 本 号   : 初稿
  作    者   : 卢彦胜 00212112
  生成日期   : 2014年3月29日
  最近修改   :
  功能描述   : 主要用于可维可测函数的定义,如通过ecall调用的关闭/开启看门狗函数，
               修改看门狗时间的函数等
  函数列表   :
  修改历史   :
  1.日    期   : 2014年3月29日
    作    者   : 卢彦胜 00212112
    修改内容   : 创建文件

******************************************************************************/
#include "debug_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
变量引用
*****************************************************************************/

/*****************************************************************************
函数声明
*****************************************************************************/
extern unsigned int BSP_BBPGetCurTime(int *pCurTime);
extern unsigned int omTimerGet(void);
extern void reg_dbg_dump(unsigned int pAddr, unsigned int size, unsigned int nodesize);
extern unsigned int  wdg_dbg_readreg(unsigned char* uaddr);
extern void wdg_dbg_enable_all(void);
extern void wdg_dbg_disable_all(void);
#ifdef _DRV_LLT_
int kernel_access_stub(int a, int b)
{
    return a;
}
#ifdef probe_kernel_address(a, b)
#undef probe_kernel_address(a, b)
#define probe_kernel_address(a, b) kernel_access_stub(a, b)
#endif
int ioremap_stub(int a, int b)
{
    return a;
}
#ifdef ioremap(a,b)
#undef ioremap(a,b)
#define ioremap(a,b) ioremap_stub(a,b)
#endif

void iounmap_stub(int a)
{

}
#ifdef iounmap(a)
#undef iounmap(a)
#define iounmap(a) iounmap_stub(a)
#endif
int alloc_tty_driver_stub(int a)
{
    return a;
}
#ifdef alloc_tty_driver(a)
#undef alloc_tty_driver(a)
#define alloc_tty_driver(a) alloc_tty_driver_stub(a)
#endif
#endif
/*****************************************************************************
函数定义
*****************************************************************************/
/***********************************************************
 Function: wdg_dbg_enable
 Description: Enable watch dog of A core,C core or M core
 Input:
        ewdg_type, wdg type:0,Acore wdg;1,Ccore wdg;2,mcu local wdg;3,mcu global wdg.
 return:   0,if ok;-1 if ko.
 History:
 1.    20140121   Creat
************************************************************/
int wdg_dbg_enable(dbg_wdg_type ewdg_type)
{
    int     iret = 0;
    unsigned char*    ulBaseAddr = NULL;

    switch (ewdg_type)
    {
    case DBG_WDG_ACORE:
    /* 监控ACPU，由ACPU负责监控，由MCU处理复位请求或引起全局复位 */
        ulBaseAddr = ioremap(SOC_AO_SCTRL_BASE_ADDR,SZ_8K);
        writel(BIT(SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_12pclk_wdt0_START),
              SOC_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(ulBaseAddr));

        break;
    case DBG_WDG_CCORE:
        ulBaseAddr = ioremap(SOC_MODEM_SCTRL_BASE_ADDR,SZ_8K);
        writel(BIT(SOC_MODEM_SCTRL_MDMSC_AO_CLKEN_ao_peri_clken_watchdog0_pclk_START),
              SOC_MODEM_SCTRL_MDMSC_AO_CLKEN_ADDR(ulBaseAddr));

        break;
    case DBG_WDG_LOCAL_MCU:
        /*mcu local wdg*/
        ulBaseAddr = ioremap(SOC_AO_SCTRL_BASE_ADDR,SZ_8K);
        writel(BIT(SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_6pclk_cm3_wdt0_START),
              SOC_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(ulBaseAddr));

        break;
    case DBG_WDG_GLOBAL_MCU:
        /*mcu global wdg*/
        ulBaseAddr = ioremap(SOC_AO_SCTRL_BASE_ADDR,SZ_8K);
        writel(BIT(SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_7pclk_cm3_wdt1_START),
              SOC_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(ulBaseAddr));

        break;

    default:
        printk("wrong core to set wdg!!\n");
        iret = -1;
        break;
    }
    iounmap((void*)ulBaseAddr);
    ulBaseAddr = NULL;

    return iret;
}

/***********************************************************
 Function: wdg_dbg_disable
 Description: Disable watch dog of A core,C core or M core
 Input:
        ewdg_type, wdg type:0,Acore wdg;1,Ccore wdg;2,mcu local wdg;3,mcu global wdg.

 return:   0,if ok;-1 of ko.
 History:
 1.    20140121   Creat
************************************************************/
int wdg_dbg_disable(dbg_wdg_type ewdg_type)
{
    int     iret = 0;
	unsigned char*    ulBaseAddr = NULL;

    /*reg write referencing tsBSP_WDT_unInit in C core*/
    switch (ewdg_type)
    {
    case DBG_WDG_ACORE:
    /* 监控ACPU，由ACPU负责监控，由MCU处理复位请求或引起全局复位 */
        ulBaseAddr = ioremap(SOC_AO_SCTRL_BASE_ADDR,SZ_8K);
        writel(BIT(SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_12pclk_wdt0_START),
              SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_ADDR(ulBaseAddr));
        break;
    case DBG_WDG_CCORE:
        ulBaseAddr = ioremap(SOC_MODEM_SCTRL_BASE_ADDR,SZ_8K);
        writel(BIT(SOC_MODEM_SCTRL_MDMSC_AO_CLKDIS_ao_peri_clkdis_watchdog0_pclk_START),
              SOC_MODEM_SCTRL_MDMSC_AO_CLKDIS_ADDR(ulBaseAddr));

        break;
    case DBG_WDG_LOCAL_MCU:
        /*mcu local wdg*/
        ulBaseAddr = ioremap(SOC_AO_SCTRL_BASE_ADDR,SZ_8K);
        writel(BIT(SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_6pclk_cm3_wdt0_START),
              SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_ADDR(ulBaseAddr));

        break;
    case DBG_WDG_GLOBAL_MCU:
        /*mcu global wdg*/
        ulBaseAddr = ioremap(SOC_AO_SCTRL_BASE_ADDR,SZ_8K);
        writel(BIT(SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_periph_clkdis4_7pclk_cm3_wdt1_START),
              SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_ADDR(ulBaseAddr));
        break;

    default:
        printk("wrong core to set wdg!!\n");
        iret = -1;
        break;
    }
    iounmap((void*)ulBaseAddr);
    ulBaseAddr = NULL;

    return iret;
}

/***********************************************************
 Function: wdg_dbg_enable_all
 Description: Enable watch dog of A core,C core or M core
 Input:none
 return: none
 History:
 1.    20140121   Creat
************************************************************/
void wdg_dbg_enable_all(void)
{   
    wdg_dbg_enable(DBG_WDG_ACORE);
    wdg_dbg_enable(DBG_WDG_CCORE);
    wdg_dbg_enable(DBG_WDG_LOCAL_MCU);
    wdg_dbg_enable(DBG_WDG_GLOBAL_MCU);
}

/***********************************************************
 Function: wdg_dbg_disable_all
 Description: Disable watch dog of A core,C core or M core
 Input:none
 return: none
 History:
 1.    20140121   Creat
************************************************************/
void wdg_dbg_disable_all(void)
{
    wdg_dbg_disable(DBG_WDG_ACORE);
    wdg_dbg_disable(DBG_WDG_CCORE);
    wdg_dbg_disable(DBG_WDG_LOCAL_MCU);
    wdg_dbg_disable(DBG_WDG_GLOBAL_MCU);
}
unsigned int  wdg_dbg_readreg(unsigned char* uaddr)
{
    return (*(volatile unsigned *) (uaddr));
}
/***********************************************************
 Function: wdg_dbg_settime
 Description: Change the time of the watch dog
 Input:
        ewdg_type, wdg type:0,Acore wdg;1,Ccore wdg;2,mcu local wdg;3,mcu global wdg.
         ival_msec, milliseconds
 return:   0,if ok;-1 of ko.
 History:
 1.    20140121   Creat
************************************************************/
int wdg_dbg_settime(dbg_wdg_type ewdg_type,int ival_msec)
{
    int     iret = 0;
    unsigned int    ulRegValue = 0;
    unsigned char*    ulBaseAddr = NULL;

    switch (ewdg_type)
    {
    case DBG_WDG_ACORE:
        ulBaseAddr = ioremap(SOC_Watchdog0_BASE_ADDR, SZ_4K);
        /* open R/W */
        writel(CCORE_WDT_REG_VALUE_UNLOCK, SOC_WDT_WDG_LOCK_ADDR(ulBaseAddr));

        /* set watchdog timerout count */
        writel(CCORE_WDT_CLK_CNT_PER_S*ival_msec/1000, SOC_WDT_WDG_LOAD_ADDR(ulBaseAddr));

        iounmap((void*)ulBaseAddr);
        ulBaseAddr = NULL;

        break;
    case DBG_WDG_CCORE:
        ulBaseAddr = ioremap(SOC_MWatchDog0_BASE_ADDR, SZ_4K);
        /* open R/W */
        writel(CCORE_WDT_REG_VALUE_UNLOCK, SOC_WDT_WDG_LOCK_ADDR(ulBaseAddr));

        writel(0, SOC_WDT_WDG_CONTROL_ADDR(ulBaseAddr));
        /* set watchdog timerout count */
        writel(CCORE_WDT_CLK_CNT_PER_S*ival_msec/1000, SOC_WDT_WDG_LOAD_ADDR(ulBaseAddr));

        writel(0x3, SOC_WDT_WDG_CONTROL_ADDR(ulBaseAddr));
        /* close R/W ,have to remove this, or acpu will be dead here!!(on sft platform)*/
        writel(CCORE_WDT_REG_VALUE_LOCK, SOC_WDT_WDG_LOCK_ADDR(ulBaseAddr));
        iounmap((void*)ulBaseAddr);
        ulBaseAddr = NULL;
        break;
    case DBG_WDG_LOCAL_MCU:
        /*mcu local wdg*/
        ulBaseAddr = (unsigned int)ioremap(SOC_CM3Watchdog0_BASE_ADDR, SZ_4K);
        /*判断WDT是否被锁*/
        ulRegValue = wdg_dbg_readreg(SOC_WDT_WDG_LOCK_ADDR(ulBaseAddr));
        if(0 != ulRegValue)
        {
            /*解锁*/
            writel(MCU_WDT_UNLOCK_VALUE, SOC_WDT_WDG_LOCK_ADDR(ulBaseAddr));
        }

        ulRegValue = M3_WDT_NOT_GLOBAL_FREQUENCY * (ival_msec/1000) - 1;

        /*Clear the WDT INT register*/
        writel(0, SOC_WDT_WDG_INTCLR_ADDR(ulBaseAddr));

        /*Set Counter Start Num*/
        writel(ulRegValue, SOC_WDT_WDG_LOAD_ADDR(ulBaseAddr));
        /*锁*/
        writel(0, SOC_WDT_WDG_LOCK_ADDR(ulBaseAddr));

        iounmap((void*)ulBaseAddr);
        ulBaseAddr = NULL;
        break;

    case DBG_WDG_GLOBAL_MCU:
        /*mcu global wdg*/
        ulBaseAddr = ioremap(SOC_CM3Watchdog1_BASE_ADDR, SZ_4K);
        /*判断WDT是否被锁*/
        ulRegValue = wdg_dbg_readreg(SOC_WDT_WDG_LOCK_ADDR(ulBaseAddr));
        if(0 != ulRegValue)
        {
            /*解锁*/
            writel(MCU_WDT_UNLOCK_VALUE, SOC_WDT_WDG_LOCK_ADDR(ulBaseAddr));
        }

        ulRegValue = M3_WDT_NOT_GLOBAL_FREQUENCY * (ival_msec/1000) - 1;

        /*Clear the WDT INT register*/
        writel(0, SOC_WDT_WDG_INTCLR_ADDR(ulBaseAddr));

        /*Set Counter Start Num*/
        writel(ulRegValue, SOC_WDT_WDG_LOAD_ADDR(ulBaseAddr));

        /*锁*/
        writel(0, SOC_WDT_WDG_LOCK_ADDR(ulBaseAddr));
        iounmap((void*)ulBaseAddr);
        ulBaseAddr = NULL;
        break;

    default:
        printk("wrong core to set wdg!!\n");
        iret = -1;
        break;
    }
    return iret;
}

/*****************************************************************************
 Function: show_memory
 Description: show the data of memory given addr and size
 Input: void  *addr_in, addr of memory
            int nbytes,size of memory
            int ibool, ibool>0,vir addr;0>=ibool, phy addr
 Out: 无
 Return value: void
 History:
 1.     20140121    Creat
*****************************************************************************/
void show_memory(unsigned long addr_in, unsigned int nbytes, int ibool)
{
	int i, j;
	u32 *p;
	unsigned long virtual_addr;
	unsigned int itotalbytes = nbytes;
	const unsigned int line_size = 32;
	unsigned int nlines;
	unsigned int size;

	printk("\naddress 0x%lx:\n", addr_in);

	/*vir addr*/
	if (ibool) {
		virtual_addr = addr_in;

		/* round address down to a 32/64 bit boundary */
		p = (u32 *)(virtual_addr & ~(sizeof(unsigned long) - 1));
		/* always dump a multiple of 32 bytes */
		itotalbytes += (virtual_addr & (sizeof(unsigned long) - 1));
		nlines = (itotalbytes + (line_size-1)) / line_size;

		for (i=0; i<nlines; i++) {
			/*
			* just display low 16 bits of address to keep
			* each line of the dump < 80 characters
			*/
			printk("%04lx ", (unsigned long)p & 0xffff);
			for (j=0; j < line_size/sizeof(u32); j++) {
				u32	data;
				if (probe_kernel_address(p++, data)) {
					printk(" ********");
				} else {
					printk(" %08x", data);
				}
			}
			printk("\n");
		}
	}else {	/*phy addr*/
		/* always dump a multiple of 32 bytes */
		nlines = (itotalbytes + (line_size-1)) / line_size;
		size = nlines*line_size / sizeof(u32);
		
		reg_dbg_dump(addr_in, size, sizeof(u32));
	}
}

/*****************************************************************************
 Function: test_get_slice
 Description: only for testting, to check whether the slices are same.
 Input:
 Out: 无
 Return value: void
 History:
 1.     20140121    Creat
*****************************************************************************/
void test_get_slice(void)
{
    unsigned int    utime1 = 0;
    unsigned int    u64time2[2] = {0};
    unsigned int    uresult = 0;

    utime1 = omTimerGet();
    uresult = BSP_BBPGetCurTime(u64time2);

	printk("\nom slice is %d, bbp slice is %d %d, result = %d\n", utime1, u64time2[0],u64time2[1],uresult);

}
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
