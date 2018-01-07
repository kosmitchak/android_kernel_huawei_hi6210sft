/*!
 *****************************************************************************
 *
 * @file       sysmem_api_km.c
 *
 * This file contains kernel mode implementation of the
 * System Memory Kernel Mode API.
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

#include "sysdev_utils1.h"
#include <linux/kobject.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/page.h>
#include <linux/mm.h>
#include <linux/version.h>
#include <linux/module.h>
#include "img_defs.h"
#include "target.h"
#include "sysenv_api_km.h"
#include "system.h"
#include "sysmem_utils.h"

#include "sysmem_api_km.h"
#include "sysbrg_utils.h"
#include "sysos_api_km.h"
#include "report_api.h"
#include <asm/io.h>
#include <linux/mm.h>
#include <linux/gfp.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/dma-mapping.h>

#include <linux/highmem.h>
#include <asm/cacheflush.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
#include <asm/barrier.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
#include <linux/export.h>
#endif

/*!
******************************************************************************

 @Function                SYSMEMKM_Initialise

******************************************************************************/
static IMG_RESULT Initialise(
    SYSMEM_Heap *  heap
)
{
    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function                SYSMEMKM_Deinitialise

******************************************************************************/
static IMG_VOID Deinitialise(
    SYSMEM_Heap *  heap
)
{
}


#define VAL64(val) (((int64_t)(val)) & 0x00000000ffffffffL)
#define VAL32(val) (((int32_t)(val)) & 0xffffffffL)

/*!
******************************************************************************

 @Function                SYSMEMKM_AllocPages

******************************************************************************/
static IMG_RESULT AllocPages(
    SYSMEM_Heap *     heap,
    IMG_UINT32        ui32Size,
    SYSMEMU_sPages *  psPages,
    SYS_eMemAttrib    eMemAttrib
)
{
    IMG_UINT32 Res;
    dma_addr_t dma;
    unsigned numPages, pg_i;
    IMG_UINT64 *pCpuPhysAddrs;
    IMG_VOID **pCpuKernAddrs = IMG_NULL;
    size_t physAddrArrSize;

    // This heap only supports uncached | write-combined memory allocations
    IMG_ASSERT(eMemAttrib == (SYS_MEMATTRIB_UNCACHED | SYS_MEMATTRIB_WRITECOMBINE));
    eMemAttrib = SYS_MEMATTRIB_UNCACHED | SYS_MEMATTRIB_WRITECOMBINE;

    numPages = (ui32Size + HOST_MMU_PAGE_SIZE - 1)/HOST_MMU_PAGE_SIZE;

    // Memory for physical addresses
    physAddrArrSize = sizeof(*pCpuPhysAddrs) * numPages;
    pCpuPhysAddrs = IMG_BIGORSMALL_ALLOC(physAddrArrSize);
    if (!pCpuPhysAddrs) {
        Res = IMG_ERROR_OUT_OF_MEMORY;
        goto errPhysAddrsAlloc;
    }

    psPages->pvCpuKmAddr = dma_alloc_coherent(NULL, ui32Size, &dma, GFP_KERNEL | __GFP_HIGHMEM);
    if (!psPages->pvCpuKmAddr) {
    	pCpuKernAddrs = IMG_BIGORSMALL_ALLOC(numPages*(sizeof(IMG_VOID **)));
    	if (!pCpuKernAddrs) {
    		Res = IMG_ERROR_OUT_OF_MEMORY;
    		goto errKernAddrsAlloc;
    	}

    	for (pg_i = 0; pg_i < numPages; ++pg_i) {
    		pCpuKernAddrs[pg_i] = dma_alloc_coherent(NULL, PAGE_SIZE, &dma, GFP_KERNEL | __GFP_HIGHMEM);
    		if (!pCpuKernAddrs[pg_i]) {
    			Res = IMG_ERROR_OUT_OF_MEMORY;
    			goto errPageAlloc;
    		}

			pCpuPhysAddrs[pg_i] = VAL64(dma);
        }
    	psPages->pvImplData = (IMG_VOID *)((long)pCpuKernAddrs | 1);
    } else {
    	int paddr;

		psPages->pvImplData = (IMG_VOID *)dma;
    	paddr = dma;

        for (pg_i = 0; pg_i < numPages; ++pg_i) {
			pCpuPhysAddrs[pg_i] = VAL64(paddr + (PAGE_SIZE * pg_i));
        }
    }

    // Set pointer to physical address in structure
    psPages->ppaPhysAddr = pCpuPhysAddrs;

    Res = SYSBRGU_CreateMappableRegion(psPages->ppaPhysAddr[0], ui32Size, eMemAttrib,
                                       IMG_TRUE, psPages, &psPages->hRegHandle);
    DEBUG_REPORT(REPORT_MODULE_SYSMEM, "%s (unified) region of size %u phys 0x%llx",
                 __FUNCTION__, ui32Size, psPages->ppaPhysAddr[0]);
    IMG_ASSERT(Res == IMG_SUCCESS);
    if (Res != IMG_SUCCESS)
    {
        goto errCreateMapRegion;
    }

    return IMG_SUCCESS;

errCreateMapRegion:
errPageAlloc:
	for (--pg_i;pg_i >= 0; pg_i--) {
		dma_free_coherent(NULL, PAGE_SIZE, pCpuKernAddrs[pg_i], psPages->ppaPhysAddr[pg_i]);
	}
	IMG_BIGORSMALL_FREE(numPages * sizeof(*pCpuKernAddrs), pCpuKernAddrs);
errKernAddrsAlloc:
    IMG_BIGORSMALL_FREE(numPages * sizeof(*pCpuPhysAddrs), pCpuPhysAddrs);
errPhysAddrsAlloc:
    return Res;
}


/*!
******************************************************************************

 @Function              SYSMEMKM_ImportExternalPages

******************************************************************************/
IMG_RESULT ImportExternalPages(
    SYSMEM_Heap *     heap,
    IMG_UINT32        ui32Size,
    SYSMEMU_sPages *  psPages,
    SYS_eMemAttrib    eMemAttrib,
    IMG_VOID *        pvCpuKmAddr,
    IMG_UINT64 *      pPhyAddrs
)
{
    size_t  numPages = (ui32Size + HOST_MMU_PAGE_SIZE - 1)/HOST_MMU_PAGE_SIZE;
    size_t  physAddrArrSize = sizeof *(psPages->ppaPhysAddr) * numPages;
    size_t  phy_i;

    psPages->bMappingOnly = IMG_TRUE;
    psPages->bImported = IMG_TRUE;
    psPages->pvCpuKmAddr = pvCpuKmAddr;
    //psPages->pvImplData = pvCpuKmAddr;


    psPages->ppaPhysAddr = IMG_BIGORSMALL_ALLOC(physAddrArrSize);
    IMG_ASSERT(IMG_NULL != psPages->ppaPhysAddr);
    if (IMG_NULL == psPages->ppaPhysAddr)
    {
        return IMG_ERROR_OUT_OF_MEMORY;
    }
    for (phy_i = 0; phy_i < numPages; ++phy_i)
        psPages->ppaPhysAddr[phy_i] = pPhyAddrs[phy_i];
    /* No SYSBRGU_sMappableReg - SYSBRGU_CreateMappableRegion not called. */
    psPages->hRegHandle = NULL;

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function                SYSMEMKM_ImportPages

******************************************************************************/
static IMG_RESULT ImportPages(
    SYSMEM_Heap *  heap,
    IMG_UINT32     ui32Size,
    IMG_HANDLE     hExtImportHandle,
    IMG_HANDLE *   phPagesHandle
)
{
    SYSMEMU_sPages *  psPages = hExtImportHandle;
    SYSMEMU_sPages *  psNewPages;

    /**** In this implementation we are importing buffers that we have allocated. */
    /* Allocate a new memory structure...*/
    psNewPages = IMG_MALLOC(sizeof(*psNewPages));
    IMG_ASSERT(psNewPages != IMG_NULL);
    if (psNewPages == IMG_NULL)
    {
        return IMG_ERROR_OUT_OF_MEMORY;
    }

    /* Take a copy of the original page allocation. */
    *psNewPages = *psPages;

    /* Set imported. */
    psNewPages->bImported    = IMG_TRUE;
    psNewPages->bMappingOnly = IMG_FALSE;

    *phPagesHandle = psNewPages;
    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function                SYSMEMKM_GetCpuKmAddr

******************************************************************************/
static IMG_RESULT GetCpuKmAddr(
    SYSMEM_Heap *  heap,
    IMG_VOID **    ppvCpuKmAddr,
    IMG_HANDLE     hPagesHandle
)
{
    SYSMEMU_sPages *  psPages = hPagesHandle;


    if(psPages->pvCpuKmAddr == IMG_NULL)
    {
        IMG_UINT32 numPages;
        pgprot_t pageProt;
        unsigned pg_i;
        struct page **pages;

        pageProt = PAGE_KERNEL;
        numPages = (psPages->ui32Size + HOST_MMU_PAGE_SIZE - 1)/HOST_MMU_PAGE_SIZE;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
        /* Write combined implies non-cached in Linux x86. If we additionally call
           pgprot_noncached, we will not have write combining, just non-cached. */
        if ((psPages->eMemAttrib & SYS_MEMATTRIB_WRITECOMBINE) != 0)
        {
            pageProt = pgprot_writecombine(pageProt);
        }
#if defined(CONFIG_X86)
        else
#endif
#endif
        /* If uncached...*/
        if ((psPages->eMemAttrib & SYS_MEMATTRIB_UNCACHED) != 0)
        {
            pageProt = pgprot_noncached(pageProt);
        }

        pages = IMG_BIGORSMALL_ALLOC(numPages*(sizeof *pages));
        IMG_ASSERT(IMG_NULL != pages);
        if(IMG_NULL == pages)
        {
            return IMG_ERROR_OUT_OF_MEMORY;
        }
        for (pg_i = 0; pg_i < numPages; ++pg_i) {
        	pages[pg_i] = pfn_to_page(VAL32((psPages->ppaPhysAddr[pg_i]) >> PAGE_SHIFT));
        }

        psPages->pvCpuKmAddr = vmap(pages, numPages, VM_MAP, pageProt);

        IMG_BIGORSMALL_FREE(numPages*sizeof(*pages), pages);

    }

    *ppvCpuKmAddr = psPages->pvCpuKmAddr;

    /* Return success...*/
    return IMG_SUCCESS;
}


/*!
******************************************************************************
 @Function                SYSMEMKM_FreePages

******************************************************************************/
static IMG_VOID FreePages(
    SYSMEM_Heap *  heap,
    IMG_HANDLE     hPagesHandle
)
{
    SYSMEMU_sPages *  psPages = hPagesHandle;
    size_t            numPages;

    numPages = (psPages->ui32Size + HOST_MMU_PAGE_SIZE - 1)/HOST_MMU_PAGE_SIZE;

    /* If mapping then free on the copy of the page structure. */
    /* This will happen by sysmemutils function */
    if (psPages->bImported)
    {
        if(psPages->bMappingOnly && (IMG_NULL != psPages->ppaPhysAddr))
        {
            IMG_BIGORSMALL_FREE(numPages * sizeof(psPages->ppaPhysAddr[0]), psPages->ppaPhysAddr);
            psPages->ppaPhysAddr = IMG_NULL;
        }
        return;
    }

    IMG_ASSERT((!psPages->bMappingOnly));            /* Mapping is not supported via this implementation of the SYSMEM API. */

    /* Removed this from the list of mappable regions...*/
    SYSBRGU_DestroyMappableRegion(psPages->hRegHandle);

    /* Free pages and mapping if present */
    {
        unsigned pg_i;

        if ((long)psPages->pvImplData & 0x1) {
			IMG_VOID **pCpuKernAddrs;

        	if (psPages->pvCpuKmAddr) {
				vunmap(psPages->pvCpuKmAddr);
			}

        	pCpuKernAddrs = (IMG_VOID **)((long)psPages->pvImplData & ~0x1);
			for (pg_i = 0; pg_i < numPages; ++pg_i) {
				dma_free_coherent(NULL, PAGE_SIZE, pCpuKernAddrs[pg_i], psPages->ppaPhysAddr[pg_i]);
			}
			IMG_BIGORSMALL_FREE(numPages * sizeof(IMG_VOID *), pCpuKernAddrs);
        } else {
        	dma_free_coherent(NULL, psPages->ui32Size, psPages->pvCpuKmAddr, (dma_addr_t)(psPages->pvImplData));
        }
    }

    IMG_BIGORSMALL_FREE(numPages * sizeof(psPages->ppaPhysAddr[0]), psPages->ppaPhysAddr);
}

static IMG_VOID *CpuPAddrToCpuKmAddr(
        SYSMEM_Heap *heap, IMG_UINT64 pvCpuPAddr
)
{
    IMG_ASSERT(!"SYSDEVU1_CpuPAddrToCpuKmAddr not implemented");
    return NULL;
}

static IMG_UINT64 CpuKmAddrToCpuPAddr(
    SYSMEM_Heap *  heap,
    IMG_VOID *     pvCpuKmAddr
)
{
    IMG_UINT64 ret = 0;

    if(virt_addr_valid(pvCpuKmAddr))
    {
        /* direct mapping of kernel addresses.
         * this works for kmalloc.
         */
        ret = virt_to_phys(pvCpuKmAddr);
    }
    else
    {
        /* walk the page table.
         * Works for ioremap, vmalloc, and kmalloc(GPF_DMA),
          but not, for some reason, kmalloc(GPF_KERNEL)
         */
        struct page * pg = vmalloc_to_page(pvCpuKmAddr);
        if(pg) {
            ret = page_to_phys(pg);
        }
        else {
            IMG_ASSERT(!"vmalloc_to_page failure");
        }
    }

    return VAL64(ret);
}

static IMG_VOID UpdateMemory(
    struct SYSMEM_Heap *    heap,
    IMG_HANDLE              hPagesHandle,
    SYSMEM_UpdateDirection  dir
)
{
	if(dir == CPU_TO_DEV) {
#ifdef CONFIG_ARM
		/* ARM Cortex write buffer needs to be synchronised before device can access it */
		dmb();
#else
		mb();
#endif
	}
	return;
}

static IMG_VOID UpdateMemoryRegion(
    struct SYSMEM_Heap *    heap,
    IMG_HANDLE              hPagesHandle,
    SYSMEM_UpdateDirection  dir,
    IMG_UINT32 offset, IMG_UINT32 size
)
{
	UpdateMemory(heap, hPagesHandle, dir);
}

static SYSMEM_Ops coherent_ops = {
        .Initialise = Initialise,
        .Deinitialise = Deinitialise,

        .AllocatePages = AllocPages,
        .FreePages = FreePages,

        .GetCpuKmAddr = GetCpuKmAddr,
        .CpuKmAddrToCpuPAddr = CpuKmAddrToCpuPAddr,
        .CpuPAddrToCpuKmAddr = CpuPAddrToCpuKmAddr,

        .ImportExternalPages = ImportExternalPages,
        .ImportPages = ImportPages,

        .UpdateMemory = UpdateMemory,
        .UpdateMemoryRegion = UpdateMemoryRegion
};

IMG_RESULT SYSMEMKM_AddCoherentMemory(
    SYS_eMemPool *peMemPool
)
{
    IMG_RESULT ui32Result;
    SYS_eMemPool memPool;

	ui32Result = SYSMEMU_AddMemoryHeap(&coherent_ops, IMG_TRUE, IMG_NULL, &memPool);
	IMG_ASSERT(IMG_SUCCESS == ui32Result);
	if (IMG_SUCCESS != ui32Result)
	{
		return ui32Result;
	}

    *peMemPool = memPool;

    return IMG_SUCCESS;
}
IMGVIDEO_EXPORT_SYMBOL(SYSMEMKM_AddCoherentMemory)
