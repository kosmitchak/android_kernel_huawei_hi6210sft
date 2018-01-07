/*!
 *****************************************************************************
 *
 * @file       sysdev_utils.c
 *
 * This file contains the System Device Kernel Mode Utilities API.
 * ---------------------------------------------------------------------------
 *
 * Copyright (c) Imagination Technologies Ltd.
 * 
 * The contents of this file are subject to the MIT license as set out below.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 * 
 * Alternatively, the contents of this file may be used under the terms of the 
 * GNU General Public License Version 2 ("GPL")in which case the provisions of
 * GPL are applicable instead of those above. 
 * 
 * If you wish to allow use of your version of this file only under the terms 
 * of GPL, and not to allow others to use your version of this file under the 
 * terms of the MIT license, indicate your decision by deleting the provisions 
 * above and replace them with the notice and other provisions required by GPL 
 * as set out in the file called “GPLHEADER” included in this distribution. If 
 * you do not delete the provisions above, a recipient may use your version of 
 * this file under the terms of either the MIT license or GPL.
 * 
 * This License is also included in this distribution in the file called 
 * "MIT_COPYING".
 *
 *****************************************************************************/

#include "sysdev_utils.h"
#include "sysdev_utils1.h"
#include "system.h"

#ifdef IMG_KERNEL_MODULE
#include <linux/module.h>
#endif

static LST_T gsDevList;

static IMG_HANDLE hNextDeviceIdMutex;
static IMG_UINT32 gui32NextDeviceId;

IMG_BOOL	gSysDevInitialised = IMG_FALSE;		/*!< Indicates where the API has been initialised	*/

#define MAX_NO_DEVICES (15)
static IMG_UINT32		gui32NoDevices = 0;			/*!< No of devices	*/

/*!
******************************************************************************

 @Function				SYSDEVU_RegisterDevices

******************************************************************************/
IMG_RESULT SYSDEVU_RegisterDevice(
	SYSDEVU_sInfo *psInfo
)
{
	IMG_UINT32			ui32Result;

	IMG_ASSERT(gSysDevInitialised);
	/* Initialise parts of the device info structure...*/
	psInfo->bDevLocated	= IMG_FALSE;
	psInfo->pvLocParam = IMG_NULL;

	SYSOSKM_LockMutex(hNextDeviceIdMutex);
	psInfo->ui32DeviceId = gui32NextDeviceId;
	gui32NextDeviceId += 1;
	SYSOSKM_UnlockMutex(hNextDeviceIdMutex);

	/* Register the device with the device manager...*/
	ui32Result = DMANKM_RegisterDevice(psInfo->sDevInfo.pszDeviceName, psInfo->sDevInfo.pfnDevRegister);
	IMG_ASSERT(ui32Result == IMG_SUCCESS);
	if (ui32Result != IMG_SUCCESS)
	{
		return ui32Result;
	}

	/* Initialise the devices...*/
	LST_add(&gsDevList, psInfo);
	gui32NoDevices++;

	/* Return success...*/
	return IMG_SUCCESS;
}
IMGVIDEO_EXPORT_SYMBOL(SYSDEVU_RegisterDevice)


IMG_RESULT SYSDEVU_UnRegisterDevice(
	SYSDEVU_sInfo *psInfo
)
{
	IMG_UINT32			ui32Result;

	IMG_ASSERT(gSysDevInitialised);
	if(!LST_remove(&gsDevList, psInfo))
	{
		IMG_ASSERT(IMG_FALSE);
		return IMG_ERROR_GENERIC_FAILURE;
	}

	gui32NoDevices--;

	ui32Result = DMANKM_UnRegisterDevice(psInfo->sDevInfo.pszDeviceName);
	IMG_ASSERT(ui32Result == IMG_SUCCESS);
	if(ui32Result != IMG_SUCCESS)
		return ui32Result;

	/* Initialise parts of the device info structure...*/
	psInfo->bDevLocated	= IMG_FALSE;
	psInfo->pvLocParam = IMG_NULL;

	/* Return success...*/
	return IMG_SUCCESS;
}
IMGVIDEO_EXPORT_SYMBOL(SYSDEVU_UnRegisterDevice)



/*!
******************************************************************************

 @Function				SYSDEVU_Initialise

******************************************************************************/
IMG_RESULT SYSDEVU_Initialise(IMG_VOID)
{
	/* If not initialised...*/
	if (!gSysDevInitialised)
	{
		IMG_RESULT eResult;

		LST_init(&gsDevList);
		eResult = SYSOSKM_CreateMutex(&hNextDeviceIdMutex);
		IMG_ASSERT(eResult == IMG_SUCCESS);
		/* use a magic number to help detect dereferences of
		   DeviceId when (wrongly) casted as a pointer */
		gui32NextDeviceId = 0xbeef00;
		/* Now we are initialised...*/
		gSysDevInitialised = IMG_TRUE;
	}

	return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				SYSDEVU_Deinitialise

******************************************************************************/
IMG_VOID SYSDEVU_Deinitialise(IMG_VOID)
{
	if (gSysDevInitialised)
	{
		SYSOSKM_DestroyMutex(hNextDeviceIdMutex);
		gSysDevInitialised = IMG_FALSE;
	}
}

__inline static SYSDEVU_sInfo *findDeviceByName(IMG_CHAR *devName) {
	SYSDEVU_sInfo *dev = (SYSDEVU_sInfo *)LST_first(&gsDevList);
	while(dev != IMG_NULL)
	{
		if(!IMG_STRCMP(devName, dev->sDevInfo.pszDeviceName))
			return dev;
		dev = LST_next(dev);
	}
	return IMG_NULL;
}

__inline static SYSDEVU_sInfo *findDeviceById(IMG_UINT32 devId) {
	SYSDEVU_sInfo *dev = (SYSDEVU_sInfo *)LST_first(&gsDevList);
	while(dev != IMG_NULL)
	{
		if(devId == dev->ui32DeviceId)
			return dev;
		dev = LST_next(dev);
	}
	return IMG_NULL;
}

SYSDEVU_sInfo *SYSDEVU_GetDeviceById(
	IMG_UINT32 ui32DeviceId
)
{
	return findDeviceById(ui32DeviceId);
}
IMGVIDEO_EXPORT_SYMBOL(SYSDEVU_GetDeviceById)

/*!
******************************************************************************

 @Function				SYSDEVU_GetDeviceId

******************************************************************************/
IMG_RESULT SYSDEVU_GetDeviceId(
	IMG_CHAR *				pszDeviceName,
	IMG_UINT32 *			pui32DeviceId
)
{
	SYSDEVU_sInfo *dev;

	if(!gSysDevInitialised)
    {
		IMG_ASSERT(gSysDevInitialised);
        return IMG_ERROR_NOT_INITIALISED;
    }

	dev = findDeviceByName(pszDeviceName);
	if (!dev)
	{
		IMG_ASSERT(dev);
		return IMG_ERROR_DEVICE_NOT_FOUND;
	}

	/* Return device Id...*/
	*pui32DeviceId = dev->ui32DeviceId;

	/* If device not located...*/
	SYSOSKM_DisableInt();
	if (!dev->bDevLocated)
	{
		/* Device now located...*/
		dev->bDevLocated = IMG_TRUE;
		SYSOSKM_EnableInt();
	}
	else
	{
		SYSOSKM_EnableInt();
	}

	/* Return success...*/
	return IMG_SUCCESS;
}

IMGVIDEO_EXPORT_SYMBOL(SYSDEVU_GetDeviceId)

IMG_VOID SYSDEVU_GetMemoryInfo(
	IMG_UINT32			ui32DeviceId,
	IMG_VOID **			ppvKmMemory,
	IMG_PHYSADDR *			ppaPhysMemory,
	IMG_UINT64 *		pui64MemSize,
	IMG_UINT64 *		pui64DevMemoryBase
) {

#ifndef __FAKE_HW__
	SYSDEVU_sInfo *dev = findDeviceById(ui32DeviceId);
	if(!gSysDevInitialised || !dev)
	{
		IMG_ASSERT(gSysDevInitialised);
		IMG_ASSERT(dev);
		return;
	}

	if(ppvKmMemory)
		*ppvKmMemory = dev->pui32KmMemBase;

	if(ppaPhysMemory)
		*ppaPhysMemory = dev->paPhysMemBase;

	if(pui64MemSize)
		*pui64MemSize = dev->ui32MemSize;

	if(pui64DevMemoryBase)
		*pui64DevMemoryBase = (IMG_UINT64)dev->pui64DevMemoryBase;
#endif
}

IMG_VOID SYSDEVU_SetDevMap(SYSDEVU_sInfo *sysdev, IMG_PHYSADDR physreg_base, IMG_UINT32 *kmreg_base, IMG_UINT32 regsize, IMG_PHYSADDR physmem_base,
		IMG_UINT32 *kmmem_base, IMG_UINT32 memsize, IMG_UINT64 devmem_base)
{
	sysdev->paPhysRegBase = physreg_base;
	sysdev->pui32KmRegBase = kmreg_base;
	sysdev->ui32RegSize = regsize;

	sysdev->paPhysMemBase = physmem_base;
	sysdev->pui32KmMemBase = kmmem_base;
	sysdev->pui64DevMemoryBase = devmem_base;
	sysdev->ui32MemSize = memsize;
}
IMGVIDEO_EXPORT_SYMBOL(SYSDEVU_SetDevMap)

/*!
******************************************************************************

 @Function				SYSDEVU_GetCpuAddrs

******************************************************************************/
IMG_UINT32 SYSDEVU_GetCpuAddrs(
	IMG_UINT32				ui32DeviceId,
	SYSDEVKM_eRegionId		eRegionId,
	IMG_VOID **             ppvCpuKmAddr,
	IMG_PHYSADDR *          ppaCpuPhysAddr,
	IMG_UINT32 *            pui32Size
)
{
	SYSDEVU_sInfo *dev = findDeviceById(ui32DeviceId);
    if(!gSysDevInitialised || !dev)
    {
        IMG_ASSERT(gSysDevInitialised);
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

#ifdef __FAKE_HW__
    /* provide fake kernel mode addresses for fake hardware */
    gpsInfo[ui32DeviceId]->pui32KmRegBase = (void*)0x1000;
    gpsInfo[ui32DeviceId]->ui32RegSize = 0x1000;
#endif
	switch (eRegionId)
	{
	case SYSDEVKM_REGID_REGISTERS:
	  if(ppvCpuKmAddr)
			*ppvCpuKmAddr = dev->pui32KmRegBase;
	  if(ppaCpuPhysAddr)
			*ppaCpuPhysAddr = dev->paPhysRegBase;
	  if(pui32Size)
			*pui32Size = dev->ui32RegSize;
	  return IMG_SUCCESS;
	  break;
	  
	case SYSDEVKM_REGID_SLAVE_PORT:
	default:
		IMG_ASSERT(IMG_FALSE);
		break;
	}
    
	return IMG_ERROR_GENERIC_FAILURE;
}


IMG_VOID SYSDEVU_HandleSuspend(
	IMG_UINT32					ui32DeviceId,
	IMG_BOOL					forAPM
) {
	SYSDEVU_sInfo *dev = findDeviceById(ui32DeviceId);
	if(!gSysDevInitialised || !dev)
	{
		IMG_ASSERT(gSysDevInitialised);
		IMG_ASSERT(dev);
		return;
	}

	if(dev->ops->suspend_device)
		dev->ops->suspend_device(dev, forAPM);
}

IMG_VOID SYSDEVU_HandleResume(
	IMG_UINT32					ui32DeviceId,
	IMG_BOOL					forAPM
) {
	SYSDEVU_sInfo *dev = findDeviceById(ui32DeviceId);
	if(!gSysDevInitialised || !dev)
	{
		IMG_ASSERT(gSysDevInitialised);
		IMG_ASSERT(dev);
		return;
	}

	if(dev->ops->resume_device)
		dev->ops->resume_device(dev, forAPM);
}


/*!
******************************************************************************

 @Function				SYSDEVU_RegisterDevKmLisr

******************************************************************************/
IMG_VOID SYSDEVU_RegisterDevKmLisr(
	IMG_UINT32					ui32DeviceId,
	SYSDEVKM_pfnDevKmLisr		pfnDevKmLisr,
    IMG_VOID *                  pvParam
)
{
	SYSDEVU_sInfo *dev = findDeviceById(ui32DeviceId);
	if(!gSysDevInitialised || !dev)
	{
		IMG_ASSERT(gSysDevInitialised);
		IMG_ASSERT(dev);
		return;
	}

	if (
			(pfnDevKmLisr != IMG_NULL) &&
			(dev->pfnDevKmLisr != IMG_NULL)
		)
	{
		IMG_ASSERT(dev->pfnDevKmLisr == pfnDevKmLisr);
		IMG_ASSERT(dev->pvParam == pvParam);
	}
	else
	{
		dev->pfnDevKmLisr = pfnDevKmLisr;
		dev->pvParam = pvParam;
	}
}


/*!
******************************************************************************

 @Function				SYSDEVU_InvokeDevKmLisr

******************************************************************************/
IMG_RESULT SYSDEVU_InvokeDevKmLisr(
	IMG_UINT32					ui32DeviceId
)
{
    IMG_UINT32  ui32Result;
	SYSDEVU_sInfo *dev = findDeviceById(ui32DeviceId);
	if(!gSysDevInitialised || !dev)
	{
		IMG_ASSERT(gSysDevInitialised);
		IMG_ASSERT(dev);

		ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        return ui32Result;
	}

	IMG_ASSERT(dev->pfnDevKmLisr != IMG_NULL);
    if(dev->pfnDevKmLisr == IMG_NULL)
	{
		ui32Result = IMG_ERROR_INVALID_PARAMETERS;
        return ui32Result;
	}

	dev->pfnDevKmLisr(dev->pvParam);

    return IMG_SUCCESS;
}

IMG_VOID SYSDEVU_FreeDevice(
	IMG_UINT32					ui32DeviceId
)
{
	SYSDEVU_sInfo *dev = findDeviceById(ui32DeviceId);
	if(!gSysDevInitialised || !dev)
	{
		IMG_ASSERT(gSysDevInitialised);
		IMG_ASSERT(dev);
		return;
	}

	if (dev->ops->free_device)
		dev->ops->free_device(dev);
}

IMG_VOID SYSDEVU_SetDeviceOps(SYSDEVU_sInfo *sysdev, struct SYSDEV_ops *ops)
{
	IMG_ASSERT(ops != IMG_NULL);

	sysdev->ops = ops;
}
IMGVIDEO_EXPORT_SYMBOL(SYSDEVU_SetDeviceOps)

