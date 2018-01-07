/*!
 *****************************************************************************
 *
 * @file       sysdev_arm_goldfish.c
 *
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
#include <linux/platform_device.h>
#include "sysmem_utils.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
    #include <linux/export.h>
#endif

static SYSDEVU_sInfo * psSysDev;

static IMG_BOOL device_detected = IMG_FALSE;

//Module parameters used only by memory mapped devices (ie not PCI)
static int module_irq = -1;

void *vdec_reg_vaddr;
IMG_PHYSADDR vdec_reg_paddr = 0;
unsigned int vdec_reg_size = 0;

static int driver_probe(struct platform_device *pdev)
{
    struct resource *plat_res = NULL;
	printk("probing platform device : %s\n", pdev->name);

	plat_res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if(!plat_res)
		return -1;

	module_irq = plat_res->start;

	plat_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!plat_res)
		return -1;

	vdec_reg_paddr = plat_res->start;
	vdec_reg_size = plat_res->end - plat_res->start + 1;

	device_detected = IMG_TRUE;
	return 0;
}

static struct platform_driver local_driver = {
    .probe = driver_probe,
    .driver = { .name = "goldfish_msvdx", .owner = THIS_MODULE, },
};

/*!
******************************************************************************

@Function IsrCb

******************************************************************************/
static irqreturn_t IsrCb(int irq, void *dev_id)
{
    IMG_BOOL bHandled;

    if ( (psSysDev->pfnDevKmLisr != IMG_NULL) )
    {
        //Call it
        SYSOSKM_DisableInt();
        bHandled = psSysDev->pfnDevKmLisr(psSysDev->pvParam);
        SYSOSKM_EnableInt();

        //If the LISR handled the interrupt
        if (bHandled)
        {
            //Signal this
            return IRQ_HANDLED;
        }
    }

    return IRQ_NONE;
}


/*!
******************************************************************************

@Function free_device

******************************************************************************/
static IMG_VOID free_device(SYSDEVU_sInfo *psInfo)
{
    if(device_detected == IMG_TRUE)
    {
        free_irq(module_irq, psSysDev);
        platform_driver_unregister(&local_driver);
        device_detected = IMG_FALSE;
    }

}


/*!
******************************************************************************

@Function handleResume

******************************************************************************/
static IMG_VOID handle_resume(SYSDEVU_sInfo *psInfo, IMG_BOOL forAPM)
{
}


/*!
******************************************************************************

@Function handleSuspend

******************************************************************************/
static IMG_VOID handle_suspend(SYSDEVU_sInfo *psInfo, IMG_BOOL forAPM)
{
}

static struct SYSDEV_ops device_ops = {
		.free_device = free_device,

		.resume_device = handle_resume,
		.suspend_device = handle_suspend
};

/*!
******************************************************************************

@Function    SYSDEVU_VDECRegisterDriver

******************************************************************************/
IMG_RESULT SYSDEVU_VDECRegisterDriver(
    SYSDEVU_sInfo *  psInfo
)
{
    IMG_UINT32 ret = IMG_ERROR_GENERIC_FAILURE;

    ret = platform_driver_register(&local_driver);
	if (ret != IMG_SUCCESS)
	{
		ret = IMG_ERROR_DEVICE_NOT_FOUND;
		goto failure_register;
	}

	if (device_detected != IMG_TRUE) {
		ret = IMG_ERROR_DEVICE_NOT_FOUND;
		goto failure_detect;
	}

	psSysDev = psInfo;

	vdec_reg_vaddr = ioremap(vdec_reg_paddr, vdec_reg_size);
	if (!vdec_reg_vaddr) {
		printk(KERN_ERR "goldfish_device: Failed to remap registers\n");
		ret = IMG_ERROR_GENERIC_FAILURE;
		goto failure_map_reg;
	}

	if (request_irq(module_irq, IsrCb, 0, "goldfish_msvdx", psSysDev)) {
		printk(KERN_ERR "goldfish_msvdx: Failed to get IRQ\n");
		ret = IMG_ERROR_GENERIC_FAILURE;
		goto failure_request_irq;
	}

	SYSDEVU_SetDevMap(psInfo, vdec_reg_paddr, ((IMG_UINT32 *)vdec_reg_vaddr), vdec_reg_size, 0, IMG_NULL, 0, 0);
	SYSDEVU_SetDeviceOps(psInfo, &device_ops);

	ret = SYSMEMKM_AddSystemMemory(&psInfo->sMemPool);
	if(IMG_SUCCESS != ret)
	{
		goto failure_add_sys_mem;
	}

    return ret;
failure_add_sys_mem:
failure_request_irq:
	iounmap(vdec_reg_vaddr);
failure_map_reg:
failure_detect:
	platform_driver_unregister(&local_driver);
failure_register:
	psSysDev = IMG_NULL;
	return ret;
}
/*!
******************************************************************************

@Function    SYSDEVU_VDECUnRegisterDriver

******************************************************************************/
IMG_RESULT SYSDEVU_VDECUnRegisterDriver(
    SYSDEVU_sInfo *  psInfo
)
{
    SYSMEMU_RemoveMemoryHeap(psInfo->sMemPool);
    psInfo->ops->free_device(psInfo);
    return IMG_SUCCESS;
}
