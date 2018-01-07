/*!
******************************************************************************
 @file   : memmappedio-linux.c

 @brief

 @Author Imagination Technologies

 @date   10/10/2013

         <b>Copyright 2013 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 \n<b>Description:</b>\n
         This file contains the kernel mode implementation of the 
         memory-mapped device interface.

 \n<b>Platform:</b>\n
	     Linux - Kernel Mode.

******************************************************************************/
/*
******************************************************************************
*****************************************************************************/

#include "secure_device.h"
#include <img_defs.h>
#include "target.h"

#include <linux/pci.h>
#include <linux/kobject.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/page.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/module.h>

#include <linux/platform_device.h>
#include <linux/clk.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
    #include <linux/export.h>
#endif

#ifdef TARGET_HEADER_NAME
    #include TARGET_HEADER_NAME
#else
    #error TARGET_HEADER_NAME has to be defined
#endif

extern void *vdec_reg_vaddr;
extern IMG_PHYSADDR vdec_reg_paddr;
extern unsigned int vdec_reg_size;

//Device information
static TARGET_sTargetConfig		gsFullInfo;
static SECURE_sDev *			gpsInfo = IMG_NULL;
static IMG_CHAR *			    gpszDevName;


/*!
******************************************************************************

 @Function				memmappedio_DeviceAttach

******************************************************************************/
static IMG_RESULT memmappedio_DeviceAttach(
    IMG_CHAR *				pszDevName,
	SECDEV_eMapArea    eArea
)
{
	IMG_ASSERT(gpszDevName != IMG_NULL);
	if (gpszDevName != IMG_NULL)
	{
		IMG_ASSERT(IMG_STRCMP(pszDevName, gpszDevName) == 0);
	}
	else
	{
		return IMG_ERROR_GENERIC_FAILURE;
	}

	return IMG_SUCCESS;
}

/*!
******************************************************************************
 @Function				SECDEV_Initialise

******************************************************************************/
IMG_RESULT SECDEV_Initialise(IMG_VOID)
{
    IMG_UINT32 ui32Result;

    gsFullInfo = gsTargetConfig;

    if (gpszDevName == IMG_NULL)
	{
		gpszDevName = IMG_STRDUP(asDevice[0].pszDeviceName);
        IMG_ASSERT(gpszDevName);
        if (gpszDevName == IMG_NULL)
        {
            ui32Result = IMG_ERROR_OUT_OF_MEMORY;
            goto error;
        }

        if (gsFullInfo.psWrapperControl)
        {
            gsFullInfo.psWrapperControl->ui32HostTargetCycleRatio = asDevice[0].ui32HostTargetRatio;
            gsFullInfo.psWrapperControl->ui32Flags = asDevice[0].ui32WrapFlags;
        }

        gsFullInfo.psPciInterface = &sPciInterface;
    }

	return IMG_SUCCESS;

error:
    return ui32Result;
}


/*!
******************************************************************************

 @Function				SECDEV_DeInitialise

******************************************************************************/
IMG_VOID SECDEV_DeInitialise(SECDEV_eMapArea eArea)
{
    if (gpszDevName)
    {
        IMG_FREE(gpszDevName);
        gpszDevName = IMG_NULL;
    }
}

/*!
******************************************************************************

 @Function				SECDEV_LocateDevice

 @Description
 This implementation can be called twice:
 Once for the main device under test, and once for a PDP subdevice on an ATLAS card

******************************************************************************/
IMG_RESULT SECDEV_LocateDevice(
	SECURE_sDev *			psInfo,
	SECDEV_eMapArea			eArea
)
{
	IMG_UINT32 ui32Result = IMG_SUCCESS;

	if(gpsInfo == NULL)
	{
        gpsInfo = psInfo;
	}
	
	/* Configure the device...*/
	ui32Result = memmappedio_DeviceAttach(psInfo->pszDeviceName, eArea);
	IMG_ASSERT(ui32Result == IMG_SUCCESS);

	if(ui32Result != IMG_SUCCESS)
	{
		return ui32Result;
	}

	//Save register pointer etc
	psInfo->paPhysRegBase = vdec_reg_paddr;
	psInfo->pvKmRegBase = vdec_reg_vaddr;
	psInfo->ui32RegSize	= vdec_reg_size;

	return ui32Result;
}

/*!
******************************************************************************

 @Function				SECDEV_GetMemoryInfo

******************************************************************************/
IMG_VOID SECDEV_GetMemoryInfo(
	IMG_VOID **         ppui32KmMemory,
	IMG_UINT64 *        pui64MemSize
)
{
	*ppui32KmMemory = 0;
	*pui64MemSize = 0;
}

/*!
******************************************************************************

 @Function				SECDEV_CpuVirtAddrToCpuPAddr

******************************************************************************/
IMG_PHYSADDR SECDEV_CpuVirtAddrToCpuPAddr(
	IMG_VOID *pvCpuKmAddr
)
{
	IMG_PHYSADDR ret = 0;
	if(virt_addr_valid(pvCpuKmAddr))
	{
		//direct mapping of kernel addresses.
		//this works for kmalloc.
		ret = virt_to_phys(pvCpuKmAddr);
	}
	else
	{
		//walk the page table. 
		//Works for ioremap, vmalloc, and kmalloc(GPF_DMA),
		//but not, for some reason, kmalloc(GPF_KERNEL)
		struct page * pg = vmalloc_to_page(pvCpuKmAddr);
		if(pg) 
		{
			ret = page_to_phys(pg);
		}
		else 
		{
			IMG_ASSERT(!"vmalloc_to_page failure");
		}
	}
	return ret;
}
