/*!
 *****************************************************************************
 *
 * @file       page_alloc.c
 *
 * This file contains the Page Allocator.
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

#include "img_defs.h"
#include "img_types.h"
#include "img_errors.h"
#include "page_alloc.h"
#include "page_alloc_km.h"
#include "dman_api_km.h"
#include "rman_api.h"
#include "sysmem_utils.h"
#include "sysmem_api_km.h"
#include "sysdev_api_km.h"
#include "sysdev_utils.h"
#include "sysos_api_km.h"
#include "report_api.h"
#include "sysbrg_utils.h"
#include "sysmem_utils.h"
#include "sysmem_api_km.h"

#if defined ANDROID_ION_BUFFERS
# include "page_alloc_ion.h"
#endif

#if defined ANDROID_DMABUF_BUFFERS
#include "page_alloc_dmabuf.h"
#endif


#define PALLOC_RES_TYPE_1    (0x09090001)    /*!< Resource type */

/*!
******************************************************************************
 This structure contains attachment information.
******************************************************************************/
typedef struct
{
    IMG_HANDLE  hDevHandle;     /*<! Device handle          */
    IMG_HANDLE  hResBHandle;    /*<! Resource bucket handle */
    IMG_HANDLE  hSysDevHandle;  /*<! SYSDEVKN device handle */

} PALLOC_sAttachContext;

/*!
******************************************************************************
 This structure contains allocation information.
******************************************************************************/
typedef struct
{
    IMG_HANDLE           hDevHandle;    /*!< DMAN device handle             */
    IMG_HANDLE           hPagesHandle;  /*!< SYSMEM handle                  */
    PALLOCKM_sAllocInfo  sAllocInfo;    /*!< Allocation information         */
    IMG_HANDLE           hBufHandle;    /*!< Handle to specific buffer data */
    PALLOC_eImportBufType      eBufType;      /*!< Buffer type                    */
} PALLOC_sKmAlloc;


typedef IMG_RESULT (*pallockm_import)(
		SYS_eMemPool eMemPool,
		PALLOC_sUmAlloc *sUmAllocCp,
		PALLOC_sKmAlloc *psKmAlloc,
		IMG_UINT32 ui32PageNo,
		IMG_PVOID *pvCpuKmAddr,
		int buff_fd
);

typedef IMG_RESULT (*pallockm_free)(PALLOC_sKmAlloc *psKmAlloc, IMG_UINT numPages);

typedef struct {
	pallockm_import import;
	pallockm_free free;
} pallockm_importer;


static IMG_RESULT pallockm_UserAllocImport(
		SYS_eMemPool eMemPool,
		PALLOC_sUmAlloc *sUmAllocCp,
		PALLOC_sKmAlloc *psKmAlloc,
		IMG_UINT32 ui32PageNo,
		IMG_PVOID *pvCpuKmAddr,
		int buff_fd
)
{
    /* User space allocated buffer */
    IMG_VOID __user *  pvUmBuff = SYSBRG_POINTER_FROM_USER(sUmAllocCp->pvCpuUmAddr);
    IMG_ASSERT(pvUmBuff);
    psKmAlloc->hBufHandle = SYSBRG_POINTER_FROM_USER(sUmAllocCp->pvCpuUmAddr);
    psKmAlloc->eBufType = PALLOC_IMPORTBUFTYPE_USERALLOC;
    /* Assign and lock physical addresses to the user space buffer.
       The mapping of the first page in the kernel is also returned */
    return SYSOSKM_CpuUmAddrToCpuPAddrArray(pvUmBuff, psKmAlloc->sAllocInfo.ppaPAddr,
                                                  ui32PageNo, pvCpuKmAddr);
}

static IMG_RESULT pallockm_UserAllocFree(
		PALLOC_sKmAlloc *psKmAlloc,
		IMG_UINT numPages
)
{
	SYSOSKM_ReleaseCpuPAddrArray(((SYSMEMU_sPages *) psKmAlloc->hPagesHandle)->pvCpuKmAddr,
	                                     psKmAlloc->hBufHandle,
	                                     psKmAlloc->sAllocInfo.ppaPAddr, numPages);
	SYSMEMU_FreePages(psKmAlloc->hPagesHandle);
	return IMG_SUCCESS;
}

static pallockm_importer importer_user = {
		pallockm_UserAllocImport,
		pallockm_UserAllocFree
};


static IMG_RESULT pallockm_AndroidNativeImport(
		SYS_eMemPool eMemPool,
		PALLOC_sUmAlloc *sUmAllocCp,
		PALLOC_sKmAlloc *psKmAlloc,
		IMG_UINT32 ui32PageNo,
		IMG_PVOID *pvCpuKmAddr,
		int buff_fd
)
{
#ifdef ANDROID_ION_BUFFERS
	psKmAlloc->eBufType = PALLOC_IMPORTBUFTYPE_ANDROIDNATIVE;
	return palloc_GetIONPages(eMemPool, buff_fd, sUmAllocCp->ui32Size,
								psKmAlloc->sAllocInfo.ppaPAddr,
								NULL,
								&psKmAlloc->hBufHandle);
#else
	IMG_ASSERT(!"Android Native buffers isn't enabled.");
	return IMG_ERROR_GENERIC_FAILURE;
#endif
}

static IMG_RESULT pallockm_AndroidNativeFree(
		PALLOC_sKmAlloc *psKmAlloc,
		IMG_UINT numPages
)
{
#ifdef ANDROID_ION_BUFFERS
	palloc_ReleaseIONBuf(psKmAlloc->hBufHandle, NULL);
	SYSMEMU_FreePages(psKmAlloc->hPagesHandle);
	return IMG_SUCCESS;
#else
	IMG_ASSERT(!"Android Native buffers isn't enabled.");
	return IMG_ERROR_GENERIC_FAILURE;
#endif
}

static pallockm_importer importer_androidNative = {
		pallockm_AndroidNativeImport,
		pallockm_AndroidNativeFree
};


static IMG_RESULT pallockm_AndroidAshMemImport(
		SYS_eMemPool eMemPool,
		PALLOC_sUmAlloc *sUmAllocCp,
		PALLOC_sKmAlloc *psKmAlloc,
		IMG_UINT32 ui32PageNo,
		IMG_PVOID *pvCpuKmAddr,
		int buff_fd
)
{
#ifdef ANDROID_ASHMEM_BUFFERS
    /* User space allocated buffer */
    IMG_VOID __user *  pvUmBuff = SYSBRG_POINTER_FROM_USER(sUmAllocCp->pvCpuUmAddr);
    IMG_ASSERT(pvUmBuff);
    psKmAlloc->hBufHandle = SYSBRG_POINTER_FROM_USER(sUmAllocCp->pvCpuUmAddr);
    psKmAlloc->eBufType = PALLOC_IMPORTBUFTYPE_ANDROIDASHMEM;
    /* Assign and lock physical addresses to the user space buffer.
       The mapping of the first page in the kernel is also returned */
    return SYSOSKM_CpuUmAddrToCpuPAddrArray(pvUmBuff, psKmAlloc->sAllocInfo.ppaPAddr,
                                                  ui32PageNo, pvCpuKmAddr);
#else
    IMG_ASSERT(!"Android ashmem buffers isn't enabled.");
    return IMG_ERROR_GENERIC_FAILURE;
#endif
}

static pallockm_importer importer_androidAshMem = {
		pallockm_AndroidAshMemImport,
		pallockm_UserAllocFree
};

static IMG_RESULT pallockm_DMABUFImport(
		SYS_eMemPool eMemPool,
		PALLOC_sUmAlloc *sUmAllocCp,
		PALLOC_sKmAlloc *psKmAlloc,
		IMG_UINT32 ui32PageNo,
		IMG_PVOID *pvCpuKmAddr,
		int buff_fd
)
{
#ifdef ANDROID_DMABUF_BUFFERS
	psKmAlloc->eBufType = PALLOC_IMPORTBUFTYPE_DMABUF;
	return palloc_GetDMABufPages(eMemPool, buff_fd, sUmAllocCp->ui32Size,
								psKmAlloc->sAllocInfo.ppaPAddr,
								NULL,
								&psKmAlloc->hBufHandle);
#else
	IMG_ASSERT(!"DMABUF buffers isn't enabled.");
	return IMG_ERROR_GENERIC_FAILURE;
#endif
}

static IMG_RESULT pallockm_DMABUFFree(
		PALLOC_sKmAlloc *psKmAlloc,
		IMG_UINT numPages
)
{
#ifdef ANDROID_DMABUF_BUFFERS
	palloc_ReleaseDMABuf(psKmAlloc->hBufHandle, psKmAlloc->sAllocInfo.ui32Size, NULL);
	SYSMEMU_FreePages(psKmAlloc->hPagesHandle);
	return IMG_SUCCESS;
#else
	IMG_ASSERT(!"DMABUF buffers isn't enabled.");
	return IMG_ERROR_GENERIC_FAILURE;
#endif
}

static pallockm_importer importer_dmabuf = {
		pallockm_DMABUFImport,
		pallockm_DMABUFFree
};

static pallockm_importer *pallockm_importers[PALLOC_IMPORTBUFTYPE_MAX] = {
		&importer_user, // [PALLOC_IMPORTBUFTYPE_USERALLOC]
		&importer_androidNative, // [PALLOC_IMPORTBUFTYPE_ANDROIDNATIVE]
		&importer_androidAshMem, // [PALLOC_IMPORTBUFTYPE_ANDROIDASHMEM]
		&importer_dmabuf, // [PALLOC_IMPORTBUFTYPE_DMABUF]
};

#define PALLOC_GetKMImporter(id) (((id) < PALLOC_IMPORTBUFTYPE_MAX) ? pallockm_importers[id] : IMG_NULL)



/*!
******************************************************************************

 @Function                PALLOC_Initialise1

******************************************************************************/
IMG_RESULT PALLOC_Initialise1(IMG_VOID)
{
    /* Return success...*/
    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function                PALLOC_Deinitialise1

******************************************************************************/
IMG_VOID PALLOC_Deinitialise1(IMG_VOID)
{
}


/*!
******************************************************************************

 @Function              palloc_fnCompConnect

******************************************************************************/
static IMG_RESULT palloc_fnCompConnect (
    IMG_HANDLE   hAttachHandle,
    IMG_VOID **  ppvCompAttachmentData
)
{
    PALLOC_sAttachContext *  psAttachContext;
    IMG_UINT32               ui32Result;
    IMG_CHAR *               pszDeviceName;

    /* Allocate attachment context structure...*/
    psAttachContext = IMG_MALLOC(sizeof(*psAttachContext));
    IMG_ASSERT(psAttachContext != IMG_NULL);
    if (psAttachContext == IMG_NULL)
    {
        return IMG_ERROR_OUT_OF_MEMORY;
    }
    IMG_MEMSET(psAttachContext, 0, sizeof(*psAttachContext));

    /* Ensure the resource manager is initialised...*/
    ui32Result = RMAN_Initialise();
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error_rman_init;
    }

    /* Create a bucket for the resources...*/
    ui32Result = RMAN_CreateBucket(&psAttachContext->hResBHandle);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error_rman_bucket;
    }

    /* Get device information...*/
    psAttachContext->hDevHandle = DMANKM_GetDevHandleFromAttach(hAttachHandle);
    pszDeviceName = DMANKM_GetDeviceName(psAttachContext->hDevHandle);
    ui32Result = SYSDEVKM_OpenDevice(pszDeviceName, &psAttachContext->hSysDevHandle);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error_sysdev_open;
    }

    /* Return attachment context...*/
    *ppvCompAttachmentData = psAttachContext;

    /* Return success...*/
    return IMG_SUCCESS;

    /* Error handling. */
error_sysdev_open:
    RMAN_DestroyBucket(psAttachContext->hResBHandle);
error_rman_bucket:
error_rman_init:
    IMG_FREE(psAttachContext);

    return ui32Result;
}

/*!
******************************************************************************

 @Function              palloc_fnCompDisconnect

******************************************************************************/
static IMG_RESULT palloc_fnCompDisconnect (
    IMG_HANDLE  hAttachHandle,
    IMG_VOID *  pvCompAttachmentData
)
{
    PALLOC_sAttachContext *  psAttachContext = pvCompAttachmentData;

    /* Destroy the bucket and it's resources...*/
    RMAN_DestroyBucket(psAttachContext->hResBHandle);

    /* If we opened a device...*/
    if (psAttachContext->hSysDevHandle != IMG_NULL)
    {
        SYSDEVKM_CloseDevice(psAttachContext->hSysDevHandle);
    }

    /* Free attachment context...*/
    IMG_FREE(psAttachContext);

    /* Return success...*/
    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function              palloc_fnCompAttach

******************************************************************************/
static IMG_RESULT palloc_fnCompAttach(
    IMG_HANDLE            hConnHandle,
    DMANKM_sCompAttach *  psCompAttach
)
{
    psCompAttach->pfnCompConnect    = palloc_fnCompConnect;
    psCompAttach->pfnCompDisconnect = palloc_fnCompDisconnect;

    /* Return success...*/
    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function                PALLOC_AttachToConnection

******************************************************************************/
IMG_RESULT PALLOC_AttachToConnection(
    IMG_UINT32           ui32ConnId,
    SYSBRG_POINTER_ARG(IMG_UINT32) pui32AttachId
)
{
    IMG_HANDLE  hDevHandle;
    IMG_UINT32  ui32Result;
    IMG_HANDLE  hConnHandle;
    IMG_UINT32  ui32AttachId;

    /* Get the connection handle from it's ID...*/
    ui32Result = DMANKM_GetConnHandleFromId(ui32ConnId, &hConnHandle);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        return ui32Result;
    }

    /* Get the device handle from the connection...*/
    hDevHandle = DMANKM_GetDevHandleFromConn(hConnHandle);

    /* Lock the device...*/
    DMANKM_LockDeviceContext(hDevHandle);

    /* Call on to the kernel function...*/
    ui32Result = DMANKM_AttachComponent(hConnHandle, "PALLOCBRG", palloc_fnCompAttach, IMG_NULL, &ui32AttachId);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);

    /* Unlock the device...*/
    DMANKM_UnlockDeviceContext(hDevHandle);

    SYSOSKM_CopyToUser(pui32AttachId, &ui32AttachId, sizeof(ui32AttachId));

    /* Return ui32Result...*/
    return ui32Result;
}

/*!
******************************************************************************

 @Function             palloc_fnFree

******************************************************************************/
static IMG_VOID palloc_fnFree(
    IMG_VOID *  pvParam
)
{
    PALLOC_sKmAlloc *psKmAlloc = (PALLOC_sKmAlloc *) pvParam;
    pallockm_importer *importer = IMG_NULL;
    IMG_UINT numPages;

    numPages = (psKmAlloc->sAllocInfo.ui32Size + HOST_MMU_PAGE_SIZE - 1)/HOST_MMU_PAGE_SIZE;

    if(!psKmAlloc->sAllocInfo.bMappingOnly) {
    	SYSMEMU_FreePages(psKmAlloc->hPagesHandle);
    } else {
		if((importer = PALLOC_GetKMImporter(psKmAlloc->eBufType))) {
			importer->free(psKmAlloc,numPages);
		}

		if(!importer)
			IMG_ASSERT(!"palloc_fnFree wrong buffer type");
    }

    IMG_BIGORSMALL_FREE(numPages * sizeof(IMG_PHYSADDR), psKmAlloc->sAllocInfo.ppaPAddr);

    /* Free this structure */
    IMG_FREE(psKmAlloc);
}


/*!
******************************************************************************

 @Function                PALLOC_Alloc1

******************************************************************************/
IMG_RESULT PALLOC_Alloc1(
    IMG_UINT32                ui32AttachId,
    SYS_eMemAttrib            eMemAttrib,
    SYSBRG_POINTER_ARG(PALLOC_sUmAlloc) psUmAlloc
)
{
    IMG_HANDLE               hDevHandle;
    IMG_UINT32               ui32Result;
    PALLOC_sKmAlloc *        psKmAlloc;
    IMG_HANDLE               hAttachHandle;
    PALLOC_sAttachContext *  psAttachContext;
    IMG_UINT32               ui32PageNo;
    PALLOC_sUmAlloc          sUmAllocCp;
    IMG_UINT32               ui32PageIdx;
    IMG_PHYSADDR *           pPhys;
    SYSMEMU_sPages *         psSysMem;
    SYS_eMemPool             eMemPool;
    SYSDEVU_sInfo *          psSysDev;
    /* the following code assumes that IMG_PHYSADDR and IMG_UINT64 are the same size */

#ifndef SYSBRG_BRIDGING
    IMG_VOID *               pvKmAddr;
#endif

    if (SYSOSKM_CopyFromUser(&sUmAllocCp, psUmAlloc, sizeof(sUmAllocCp)) != IMG_SUCCESS)
    {
        return IMG_ERROR_FATAL;
    }

    IMG_ASSERT(!sUmAllocCp.bMappingOnly);

    /* Get the attachment handle from its ID...*/
    ui32Result = DMANKM_GetAttachHandleFromId(ui32AttachId, &hAttachHandle);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        return ui32Result;
    }

    /* Get access to the attachment specific data...*/
    psAttachContext = DMANKM_GetCompAttachmentData(hAttachHandle);

    /* Get access to the device handle...*/
    hDevHandle = DMANKM_GetDevHandleFromAttach(hAttachHandle);

    /* Lock the device...*/
    DMANKM_LockDeviceContext(hDevHandle);

    psSysDev = SYSDEVU_GetDeviceById(SYSDEVKM_GetDeviceID(psAttachContext->hSysDevHandle));
    IMG_ASSERT(psSysDev != IMG_NULL); // I
    if (psSysDev == IMG_NULL)
    {
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }

    eMemPool = (eMemAttrib & SYS_MEMATTRIB_SECURE) ? psSysDev->secureMemPool : psSysDev->sMemPool;

    /* Allocate allocation info...*/
    psKmAlloc = IMG_MALLOC(sizeof(*psKmAlloc));
    IMG_ASSERT(psKmAlloc != IMG_NULL);
    if (psKmAlloc == IMG_NULL)
    {
        ui32Result = IMG_ERROR_OUT_OF_MEMORY;
        goto error_alloc_info;
    }
    IMG_MEMSET(psKmAlloc, 0, sizeof(*psKmAlloc));

    /* Save device handle etc...*/
    psKmAlloc->hDevHandle          = hDevHandle;
    psKmAlloc->sAllocInfo.ui32Size = sUmAllocCp.ui32Size;
    psKmAlloc->hBufHandle          = NULL;

    /* Allocate pages...*/
    ui32Result = SYSMEMU_AllocatePages(sUmAllocCp.ui32Size, eMemAttrib, eMemPool,
                                       &psKmAlloc->hPagesHandle, &pPhys);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error_alloc_pages;
    }

#ifndef SYSBRG_BRIDGING
    SYSMEMU_GetCpuKmAddr(&pvKmAddr, psKmAlloc->hPagesHandle);
    IMG_ASSERT(pvKmAddr != IMG_NULL);
    if(pvKmAddr == IMG_NULL)
    {
        ui32Result = IMG_ERROR_FATAL;
        goto error_cpu_km_addr;
    }
#endif

    /* Return addresses...*/
    psSysMem = psKmAlloc->hPagesHandle;

#ifdef PALLOC_EXPOSE_KM_HANDLE
    sUmAllocCp.hKmAllocHandle = psKmAlloc->hPagesHandle;
#endif

    /* Check if contiguous...*/
    psKmAlloc->sAllocInfo.bIsContiguous = SYSMEMKM_IsContiguous(psKmAlloc->hPagesHandle);

    /* Get the device id...*/
    ui32Result = DMANKM_GetDeviceId(hDevHandle, &sUmAllocCp.ui32DeviceId);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error_get_dev_id;
    }

    sUmAllocCp.ui64Offset = 0;
    if (psSysMem->hRegHandle)
    {
        // Determine the offset to memory if it has been made mappable in UM.
        sUmAllocCp.ui64Offset = pPhys[0];
    }

    /* Calculate the size of the allocation in pages...*/
    ui32PageNo = (sUmAllocCp.ui32Size + HOST_MMU_PAGE_SIZE - 1)/HOST_MMU_PAGE_SIZE;
    psKmAlloc->sAllocInfo.ppaPAddr = IMG_BIGORSMALL_ALLOC(sizeof(IMG_PHYSADDR) * ui32PageNo);
    IMG_ASSERT(psKmAlloc->sAllocInfo.ppaPAddr);
    if (IMG_NULL == psKmAlloc->sAllocInfo.ppaPAddr)
    {
        ui32Result = IMG_ERROR_OUT_OF_MEMORY;
        goto error_page_array;
    }
    IMG_MEMSET(psKmAlloc->sAllocInfo.ppaPAddr, 0, sizeof(IMG_PHYSADDR) * ui32PageNo);

    for (ui32PageIdx = 0; ui32PageIdx < ui32PageNo; ++ui32PageIdx)
    {
        psKmAlloc->sAllocInfo.ppaPAddr[ui32PageIdx] = pPhys[ui32PageIdx];
    }

    /* Register this with the resource manager...*/
    ui32Result = RMAN_RegisterResource(psAttachContext->hResBHandle, PALLOC_RES_TYPE_1,
                                       palloc_fnFree, psKmAlloc, IMG_NULL, &sUmAllocCp.ui32AllocId);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error_resource_register;
    }

    /* Unlock the device...*/
    DMANKM_UnlockDeviceContext(hDevHandle);

    /* Copy to user changed PALLOC_sUmAlloc, including physical device addresses */
    if (SYSOSKM_CopyToUser(psUmAlloc, &sUmAllocCp, sizeof(sUmAllocCp)))
    {
        ui32Result = IMG_ERROR_FATAL;
        goto error_copy_to_user;
    }
    if (SYSOSKM_CopyToUser(sUmAllocCp.ppaDevPAddr, psKmAlloc->sAllocInfo.ppaPAddr,
                           sizeof(*psKmAlloc->sAllocInfo.ppaPAddr) * ui32PageNo))
    {
        ui32Result = IMG_ERROR_FATAL;
        goto error_copy_to_user;
    }

    /* Return. */
    return IMG_SUCCESS;

    /* Error handling. */
error_copy_to_user:
    /* Free everything. */
    PALLOC_Free1(sUmAllocCp.ui32AllocId);
    goto error_return;

error_resource_register:
    IMG_BIGORSMALL_FREE(sizeof(IMG_PHYSADDR) * ui32PageNo,
                        psKmAlloc->sAllocInfo.ppaPAddr);
error_page_array:
error_get_dev_id:
#ifndef SYSBRG_BRIDGING
error_cpu_km_addr:
#endif /* SYSBRG_BRIDGING */
    SYSMEMU_FreePages(psKmAlloc->hPagesHandle);
error_alloc_pages:
    IMG_FREE(psKmAlloc);
error_alloc_info:
    /* Unlock the device. */
    DMANKM_UnlockDeviceContext(hDevHandle);

error_return:
    return ui32Result;
}


/*!
******************************************************************************

 @Function              PALLOC_Import1

******************************************************************************/
IMG_RESULT PALLOC_Import1(
    IMG_UINT32                ui32AttachId,
    PALLOC_eImportBufType	  bufType,
    SYS_eMemAttrib            eMemAttrib,
    IMG_INT32                 buff_fd,
    SYSBRG_POINTER_ARG(PALLOC_sUmAlloc) psUmAlloc
)
{
    IMG_HANDLE               hDevHandle;
    IMG_UINT32               ui32Result;
    PALLOC_sKmAlloc *        psKmAlloc;
    IMG_HANDLE               hAttachHandle;
    PALLOC_sAttachContext *  psAttachContext;
    IMG_UINT32               ui32PageNo;
    IMG_UINT32               ui32PageIdx;
    PALLOC_sUmAlloc          sUmAllocCp;
    IMG_PHYSADDR *           ppaDevAddrs;
    SYSDEVU_sInfo *          psSysDev;
    SYS_eMemPool             eMemPool;
    IMG_PVOID                pvCpuKmAddr;
    pallockm_importer		 *importer;

    DEBUG_REPORT(REPORT_MODULE_PALLOC, "PALLOC_Import1 fd %d", buff_fd);

    if (SYSOSKM_CopyFromUser(&sUmAllocCp, psUmAlloc, sizeof sUmAllocCp) != IMG_SUCCESS)
    {
        return IMG_ERROR_FATAL;
    }

    IMG_ASSERT(sUmAllocCp.bMappingOnly);

    /* Get the attachment handle from its ID... */
    ui32Result = DMANKM_GetAttachHandleFromId(ui32AttachId, &hAttachHandle);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        return ui32Result;
    }

    /* Get access to the attachment specific data...*/
    psAttachContext = DMANKM_GetCompAttachmentData(hAttachHandle);

    /* Get access to the device handle...*/
    hDevHandle = DMANKM_GetDevHandleFromAttach(hAttachHandle);

    /* Lock the device...*/
    DMANKM_LockDeviceContext(hDevHandle);

    psSysDev = SYSDEVU_GetDeviceById(SYSDEVKM_GetDeviceID(psAttachContext->hSysDevHandle));
    IMG_ASSERT(psSysDev != IMG_NULL); // I
    if (psSysDev == IMG_NULL)
    {
        ui32Result = IMG_ERROR_DEVICE_NOT_FOUND;
        goto error_get_dev_by_id;
    }

    eMemPool = (eMemAttrib & SYS_MEMATTRIB_SECURE) ? psSysDev->secureMemPool : psSysDev->sMemPool;

    /* Allocate allocation info...*/
    psKmAlloc = IMG_MALLOC(sizeof *psKmAlloc);
    IMG_ASSERT(psKmAlloc != IMG_NULL);
    if (psKmAlloc == IMG_NULL)
    {
        ui32Result = IMG_ERROR_OUT_OF_MEMORY;
        goto error_alloc_info;
    }
    IMG_MEMSET(psKmAlloc, 0, sizeof *psKmAlloc);

    /* Save device handle etc... */
    psKmAlloc->hDevHandle = hDevHandle;
    psKmAlloc->sAllocInfo.ui32Size = sUmAllocCp.ui32Size;
    psKmAlloc->sAllocInfo.bIsContiguous = IMG_FALSE;

    /* Get the device id...*/
    ui32Result = DMANKM_GetDeviceId(hDevHandle, &sUmAllocCp.ui32DeviceId);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error_get_dev_id;
    }

    psKmAlloc->sAllocInfo.bMappingOnly = IMG_TRUE;

    /* Calculate the size of the allocation in pages */
    ui32PageNo = (sUmAllocCp.ui32Size + HOST_MMU_PAGE_SIZE - 1)/HOST_MMU_PAGE_SIZE;

    psKmAlloc->sAllocInfo.ppaPAddr = IMG_BIGORSMALL_ALLOC(sizeof(IMG_PHYSADDR) * ui32PageNo);
    IMG_ASSERT(psKmAlloc->sAllocInfo.ppaPAddr);
    if (IMG_NULL == psKmAlloc->sAllocInfo.ppaPAddr)
    {
        ui32Result = IMG_ERROR_OUT_OF_MEMORY;
        goto error_page_array;
    }
    ppaDevAddrs = IMG_BIGORSMALL_ALLOC((sizeof *ppaDevAddrs) * ui32PageNo);
    IMG_ASSERT(ppaDevAddrs);
    if (IMG_NULL == ppaDevAddrs)
    {
        ui32Result = IMG_ERROR_OUT_OF_MEMORY;
        goto error_addr_array;
    }

    if((importer = PALLOC_GetKMImporter(bufType))) {
    	ui32Result = importer->import(
			eMemPool,
			&sUmAllocCp,
			psKmAlloc,
			ui32PageNo,
			&pvCpuKmAddr,
			buff_fd
    	);
    } else {
    	IMG_ASSERT(!"PALLOC_Import: import buffer type not supported");
		ui32Result = IMG_ERROR_GENERIC_FAILURE;
    }

	IMG_ASSERT(ui32Result == IMG_SUCCESS);
	if (ui32Result != IMG_SUCCESS)
		goto error_get_pages;

    /* Import pages */
    ui32Result = SYSMEMU_ImportExternalPages(eMemPool, sUmAllocCp.ui32Size, eMemAttrib,
                                             &psKmAlloc->hPagesHandle, pvCpuKmAddr,
                                             psKmAlloc->sAllocInfo.ppaPAddr);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error_import_pages;
    }

    // Access from user space is not needed for the moment. Can be changed.
    sUmAllocCp.ui64Offset = 0;
#if PALLOC_EXPOSE_KM_HANDLE
    sUmAllocCp.hKmAllocHandle = psKmAlloc->hPagesHandle;
#endif /* PALLOC_EXPOSE_KM_HANDLE */

    for (ui32PageIdx = 0; ui32PageIdx < ui32PageNo; ++ui32PageIdx)
    {
        IMG_PHYSADDR paCpuPAddr = psKmAlloc->sAllocInfo.ppaPAddr[ui32PageIdx];
        ppaDevAddrs[ui32PageIdx] =
            SYSDEVKM_CpuPAddrToDevPAddr(psAttachContext->hSysDevHandle, paCpuPAddr);
    }

    /* Register this with the resource manager */
    ui32Result = RMAN_RegisterResource(psAttachContext->hResBHandle, PALLOC_RES_TYPE_1,
                                       palloc_fnFree, psKmAlloc, IMG_NULL, &sUmAllocCp.ui32AllocId);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        goto error_resource_register;
    }

    /* Unlock the device...*/
    DMANKM_UnlockDeviceContext(hDevHandle);

    /* Copy to user changed PALLOC_sUmAlloc, including physical device addresses */
    if (SYSOSKM_CopyToUser(psUmAlloc, &sUmAllocCp, sizeof sUmAllocCp))
    {
        ui32Result = IMG_ERROR_FATAL;
        goto error_copy_to_user;
    }
    if (SYSOSKM_CopyToUser(sUmAllocCp.ppaDevPAddr, ppaDevAddrs,
                           (sizeof *ppaDevAddrs) * ui32PageNo))
    {
        ui32Result = IMG_ERROR_FATAL;
        goto error_copy_to_user;
    }
    /* Free the address array */
    IMG_BIGORSMALL_FREE((sizeof *ppaDevAddrs) * ui32PageNo, ppaDevAddrs);

    /* Return. */
    return IMG_SUCCESS;

    /* Error handling. */
error_copy_to_user:
    /* Free everything. */
    PALLOC_Free1(sUmAllocCp.ui32AllocId);
    goto error_return;

error_resource_register:
    SYSMEMU_FreePages(psKmAlloc->hPagesHandle);
error_import_pages:
	PALLOC_GetKMImporter(bufType)->free(psKmAlloc, ui32PageNo);
error_get_pages:
    IMG_BIGORSMALL_FREE((sizeof *ppaDevAddrs) * ui32PageNo, ppaDevAddrs);
error_addr_array:
    IMG_BIGORSMALL_FREE(sizeof(IMG_PHYSADDR) * ui32PageNo, psKmAlloc->sAllocInfo.ppaPAddr);
error_page_array:
error_get_dev_id:
    IMG_FREE(psKmAlloc);
error_alloc_info:
error_get_dev_by_id:
    /* Unlock the device. */
    DMANKM_UnlockDeviceContext(hDevHandle);

error_return:
    return ui32Result;
}


/*!
******************************************************************************

 @Function                PALLOC_Free1

******************************************************************************/
IMG_RESULT PALLOC_Free1(
    IMG_UINT32  ui32AllocId
)
{
    IMG_UINT32         ui32Result;
    PALLOC_sKmAlloc *  psKmAlloc;
    IMG_HANDLE         hResHandle;
    IMG_HANDLE         hDevHandle;

    /* Get the resource info from the id...*/
    ui32Result = RMAN_GetResource(ui32AllocId, PALLOC_RES_TYPE_1, (IMG_VOID **)&psKmAlloc, &hResHandle);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        return ui32Result;
    }

    hDevHandle = psKmAlloc->hDevHandle;

    /* Lock the device...*/
    DMANKM_LockDeviceContext(hDevHandle);

    /* Free through the resource manager...*/
    RMAN_FreeResource(hResHandle);

    /* Unlock the device...*/
    DMANKM_UnlockDeviceContext(hDevHandle);

    /* Return IMG_SUCCESS...*/
    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function                PALLOCKM_GetAllocInfo

******************************************************************************/
IMG_RESULT PALLOCKM_GetAllocInfo(
    IMG_UINT32             ui32AllocId,
    PALLOCKM_sAllocInfo *  psAllocInfo
)
{
    IMG_UINT32         ui32Result;
    PALLOC_sKmAlloc *  psKmAlloc;
    IMG_HANDLE         hResHandle;

    /* Get the resource info from the id...*/
    ui32Result = RMAN_GetResource(ui32AllocId, PALLOC_RES_TYPE_1, (IMG_VOID **)&psKmAlloc, &hResHandle);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        return ui32Result;
    }

    /* Return the allocation info...*/
    *psAllocInfo = psKmAlloc->sAllocInfo;

    /* Return IMG_SUCCESS...*/
    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function                PALLOCKM_GetPagesHandle

******************************************************************************/
IMG_RESULT PALLOCKM_GetPagesHandle(
    IMG_UINT32   ui32AllocId,
    IMG_HANDLE * pPagesHandle
)
{
    IMG_UINT32         ui32Result;
    PALLOC_sKmAlloc *  psKmAlloc;

    /* Get the resource info from the id...*/
    ui32Result = RMAN_GetResource(ui32AllocId, PALLOC_RES_TYPE_1, (IMG_VOID **)&psKmAlloc, IMG_NULL);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        return ui32Result;
    }

    /* Return the pages handle */
    *pPagesHandle = psKmAlloc->hPagesHandle;

    /* Return IMG_SUCCESS...*/
    return IMG_SUCCESS;
}
IMGVIDEO_EXPORT_SYMBOL(PALLOCKM_GetPagesHandle)

/*!
******************************************************************************

 @Function                PALLOCKM_CloneAlloc

******************************************************************************/
IMG_RESULT PALLOCKM_CloneAlloc(
    IMG_UINT32    ui32AllocId,
    IMG_HANDLE    hResBHandle,
    IMG_HANDLE *  phResHandle,
    IMG_UINT32 *  pui32AllocId
)
{
    IMG_UINT32  ui32Result;
    IMG_HANDLE  hResHandle;

    /* Get the resource info from the id...*/
    ui32Result = RMAN_GetResource(ui32AllocId, PALLOC_RES_TYPE_1, IMG_NULL, &hResHandle);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        return ui32Result;
    }

    /* Create a cloned reference...*/
    ui32Result = RMAN_CloneResourceHandle(hResHandle, hResBHandle, phResHandle, pui32AllocId);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        return ui32Result;
    }

    /* Return IMG_SUCCESS...*/
    return IMG_SUCCESS;
}
