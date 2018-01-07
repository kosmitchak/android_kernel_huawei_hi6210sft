/*!
 *****************************************************************************
 *
 * @file       sysdev_fpga.c
 *
 * This file contains kernel module device interface implementation
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

#include "img_types.h"
#include "sysdev_utils.h"
#include "sysmem_utils.h"

#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/version.h>

/* on 32bit arch, it is not a good idea to vmap > 256M, because of fragmentation of
   virtual address space. only really an issue during insmod/rmmod. make it configurable. 
*/
static ulong maxmapsizeMB = (sizeof(void*) == 4) ? 256 : 1024;
module_param(maxmapsizeMB, ulong, 0444);
MODULE_PARM_DESC(maxmapsizeMB, "maximum fpga RAM MBytes: default: 256M on 32bit OS, 1024MB on 64bit OS");


#define PCI_ATLAS_VENDOR_ID     (0x1010)
#define PCI_ATLAS_DEVICE_ID     (0x1CF1)    //!< Atlas V1 - FPGA device ID.
#define PCI_APOLLO_DEVICE_ID    (0x1CF2)    //!< Apollo - FPGA device ID.

#define IS_AVNET_DEVICE(devid)  ((devid)!=PCI_ATLAS_DEVICE_ID && (devid)!=PCI_APOLLO_DEVICE_ID)
#define IS_ATLAS_DEVICE(devid)  ((devid)==PCI_ATLAS_DEVICE_ID)
#define IS_APOLLO_DEVICE(devid) ((devid)==PCI_APOLLO_DEVICE_ID)

// from TCF Support FPGA.Technical Reference Manual.1.0.92.Internal Atlas GEN.External.doc:
#define PCI_ATLAS_SYS_CTRL_REGS_BAR     (0)         //!< Altas - System control register bar
#define PCI_ATLAS_PDP_REGS_BAR          (0)         //!< Altas - PDP register bar
#define PCI_ATLAS_PDP_REGS_SIZE         (0x2000)    //!< Atlas - size of PDP register area
#define PCI_ATLAS_SYS_CTRL_REGS_OFFSET  (0x0000)    //!< Altas - System control register offset
#define PCI_ATLAS_PDP1_REGS_OFFSET      (0xC000)    //!< Atlas - PDP1 register offset into bar
#define PCI_ATLAS_PDP2_REGS_OFFSET      (0xE000)    //!< Atlas - PDP2 register offset into bar
#define PCI_ATLAS_INTERRUPT_STATUS      (0x00E0)    //!< Atlas - Offset of INTERRUPT_STATUS
#define PCI_ATLAS_INTERRUPT_ENABLE      (0x00F0)    //!< Atlas - Offset of INTERRUPT_ENABLE
#define PCI_ATLAS_INTERRUPT_CLEAR       (0x00F8)    //!< Atlas - Offset of INTERRUPT_CLEAR
#define PCI_ATLAS_MASTER_ENABLE         (1<<31)     //!< Atlas - Master interrupt enable
#define PCI_ATLAS_DEVICE_INT            (1<<13)     //!< Atlas - Device interrupt
#define PCI_ATLAS_PDP1_INT              (1<<14)     //!< Atlas - PDP1 interrupt
#define PCI_ATLAS_PDP2_INT              (1<<15)     //!< Atlas - PDP2 interrupt
#define PCI_ATLAS_SCB_RESET             (1<<4)      //!< Atlas - SCB Logic soft reset
#define PCI_ATLAS_PDP2_RESET            (1<<3)      //!< Atlas - PDP2 soft reset
#define PCI_ATLAS_PDP1_RESET            (1<<2)      //!< Atlas - PDP1 soft reset
#define PCI_ATLAS_DDR_RESET             (1<<1)      //!< Atlas - soft reset the DDR logic
#define PCI_ATLAS_DUT_RESET             (1<<0)      //!< Atlas - soft reset the device under test
#define PCI_ATLAS_RESET_REG_OFFSET      (0x0080)
#define PCI_ATLAS_RESET_BITS            (PCI_ATLAS_DDR_RESET | PCI_ATLAS_DUT_RESET |PCI_ATLAS_PDP1_RESET| PCI_ATLAS_PDP2_RESET | PCI_ATLAS_SCB_RESET )

#define PCI_ATLAS_TEST_CTRL		(0xb0)
#define PCI_APOLLO_TEST_CTRL	(0x98)

#define PCI_APOLLO_INTERRUPT_STATUS     (0x00C8)    //!< Atlas - Offset of INTERRUPT_STATUS
#define PCI_APOLLO_INTERRUPT_ENABLE     (0x00D8)    //!< Atlas - Offset of INTERRUPT_ENABLE
#define PCI_APOLLO_INTERRUPT_CLEAR      (0x00E0)    //!< Atlas - Offset of INTERRUPT_CLEAR

static DEFINE_PCI_DEVICE_TABLE(pci_pci_ids) =
{
    { PCI_ATLAS_VENDOR_ID, PCI_ATLAS_DEVICE_ID,  PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0, },
    { PCI_ATLAS_VENDOR_ID, PCI_APOLLO_DEVICE_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0, },
    { 0,0,0,0,0,0,0, }
};
MODULE_DEVICE_TABLE(pci, pci_pci_ids);

static int interrupt_status_reg = -1;
static int interrupt_clear_reg = -1;
static int interrupt_enable_reg = -1;
static int test_ctrl_reg = -1;

struct imgpci_prvdata {
    int irq;
    struct {
        IMG_PHYSADDR addr;
        unsigned long size;
        void __iomem *km_addr;
    } memmap[3];
    SYSDEVU_sInfo *dev;
    struct pci_dev *pci_dev;
};


static void pciremove_func(struct pci_dev *dev);
static int pciprobe_func(struct pci_dev *dev, const struct pci_device_id *id);

struct img_pci_driver {
    struct pci_dev *pci_dev;
    struct pci_driver pci_driver;
} img_pci_driver =
{
    .pci_driver = {
        .name     = "imgpciif",    /* can change this name as necessary */
        .id_table = pci_pci_ids,
        .probe    = pciprobe_func,
        .remove   = pciremove_func,
    }
};

static unsigned int readreg32(
    struct pci_dev *  dev,
    int               bar,
    unsigned long     offset
)
{
    void __iomem *reg;
    struct imgpci_prvdata *data = (struct imgpci_prvdata *)pci_get_drvdata(dev);

    reg = (void __iomem *)(data->memmap[bar].km_addr + offset);
    return ioread32(reg);
}

static void writereg32(
    struct pci_dev *  dev,
    int               bar,
    unsigned long     offset,
    int               val
)
{
    void __iomem *reg;
    struct imgpci_prvdata *data = (struct imgpci_prvdata *)pci_get_drvdata(dev);

    reg = (void __iomem *)(data->memmap[bar].km_addr + offset);
    iowrite32(val, reg);
}

static void reset_all(
    struct pci_dev *  dev
)
{
    struct imgpci_prvdata *data;
    if(!dev)
        return;

    data = pci_get_drvdata(dev);
    if(!data)
        return;


    if(IS_ATLAS_DEVICE(dev->device))
    {
        // toggle the ATLAS reset line.
        volatile u32 * reg = (u32*)((char*)((unsigned long)data->memmap[0].km_addr) + PCI_ATLAS_RESET_REG_OFFSET);
        u32 val;
        printk("resetting ATLAS fpga\n");
        msleep(10);
        val = *reg;
        *reg = val & ~PCI_ATLAS_RESET_BITS;
        udelay(100);        // arbitrary delays, just in case!
        *reg = val | PCI_ATLAS_RESET_BITS;
        msleep(500);
    }
    else if(IS_APOLLO_DEVICE(dev->device))
    {
        printk("resetting APOLLO fpga\n");
        // toggle the reset line
        iowrite32(0x20000, (data->memmap[0].km_addr) + 0x0080);
        udelay(100);
        iowrite32(0x2041f, (data->memmap[0].km_addr) + 0x0080);
        msleep(500);
    }
}

static irqreturn_t pci_isrcb(
    int irq,
    void *dev_id
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
    , struct pt_regs * regs
#endif
)
{
    unsigned int intstatus;
    int handled;
    struct pci_dev *dev = (struct pci_dev *)dev_id;
    struct imgpci_prvdata *data = pci_get_drvdata(dev);
    if(dev_id == NULL)
    {
        // spurious interrupt: not yet initialised.
        return IRQ_NONE;
    }

    /* If Atlas FPGA...*/
    if (!IS_AVNET_DEVICE(dev->device))
    {
        intstatus = readreg32(dev, PCI_ATLAS_SYS_CTRL_REGS_BAR, interrupt_status_reg);
    }
    else
    {
        /* Avnet...*/
        intstatus = 1;
    }

    /* If there is a LISR registered...*/
    if ( (intstatus) && (data) && (data->dev) && (data->dev->pfnDevKmLisr != IMG_NULL) )
    {
        /* Call it...*/
        SYSOSKM_DisableInt();
        handled = data->dev->pfnDevKmLisr(data->dev->pvParam);
        SYSOSKM_EnableInt();

        /* If the LISR handled the interrupt...*/
        if (handled)
        {
            /* If Atlas FPGA...*/
            if (!IS_AVNET_DEVICE(dev->device))
            {
                /* We need to clear interrupts for the embedded device via the Atlas interrupt controller...*/
                writereg32(dev,    PCI_ATLAS_SYS_CTRL_REGS_BAR, interrupt_clear_reg,
                                (PCI_ATLAS_MASTER_ENABLE | intstatus));
            }

            /* Signal this...*/
            return IRQ_HANDLED;
        }
    }

    /* If Atlas FPGA...*/
    if ( (intstatus) && !IS_AVNET_DEVICE(dev->device) )
    {
        /* We need to clear interrupts for the embedded device via the Atlas interrupt controller...*/
        writereg32(dev, PCI_ATLAS_SYS_CTRL_REGS_BAR, interrupt_clear_reg, intstatus);
    }

    return IRQ_NONE;
}

/*!
******************************************************************************

@Function free_device

******************************************************************************/
static void free_device(
    SYSDEVU_sInfo *  psInfo
)
{
    unsigned bar;
    struct imgpci_prvdata *data;

    data = (struct imgpci_prvdata *)psInfo->pPrivate;

    free_irq(data->irq, (void *)data->pci_dev);

    // reset the hardware
    reset_all(data->pci_dev);

    /* Unregister the driver from the OS */
    pci_unregister_driver(&(img_pci_driver.pci_driver));

    for (bar = 0; bar < 3; ++bar)
    {
        // Bar 1 is mapped and unmapped by the secure side
        if (bar == 1) continue;

        iounmap(data->memmap[bar].km_addr);

        printk("%s bar %u address 0x%llx size 0x%lx km addr 0x%p\n", __func__,
                bar, data->memmap[bar].addr, data->memmap[bar].size, data->memmap[bar].km_addr);

        data->memmap[bar].km_addr = NULL;
    }

    kfree(data);
}

static int pciprobe_func(
    struct pci_dev *              dev,
    const struct pci_device_id *  id
)
{
    int bar;
    int ret;
    struct imgpci_prvdata *data;
    size_t maxmapsize = maxmapsizeMB * 1024 * 1024;

    /* Enable the device */
    if (pci_enable_device(dev))
    {
        goto out_free;
    }

    /* Reserve PCI I/O and memory resources */
    if (pci_request_regions(dev, "imgpci"))
    {
        goto out_disable;
    }

    /* Create a kernel space mapping for each of the bars */
    data = (struct imgpci_prvdata *)kmalloc(sizeof(*data), GFP_KERNEL);
    memset(data, 0, sizeof(*data));
    for    (bar = 0; bar < 3; bar++)
    {
        // Don't map bar1. Bar 1 (registers) is mapped by the secure side.
        if(bar == 1) continue;

        data->memmap[bar].addr = pci_resource_start(dev, bar);
        data->memmap[bar].size = pci_resource_len(dev, bar);
        if (data->memmap[bar].size > maxmapsize)
        {
            /* We avoid mapping too big regions: we do not need such a big amount of memory
               and some times we do not have enough contiguous 'vmallocable' memory. */
            printk(KERN_WARNING "%s Not mapping all available memory for bar %u\n",
                   __FUNCTION__, bar);
            data->memmap[bar].size = maxmapsize;
        }
        data->memmap[bar].km_addr = ioremap(pci_resource_start(dev, bar), data->memmap[bar].size);

        printk("%s bar %u address 0x%llx size 0x%lx km addr 0x%p\n", __func__,
                bar, data->memmap[bar].addr, data->memmap[bar].size, data->memmap[bar].km_addr);
    }

    /* Get the IRQ...*/
    data->irq = dev->irq;
    data->pci_dev = dev;
    img_pci_driver.pci_dev = dev;

    pci_set_drvdata(dev, (void *)data);

    reset_all(dev);

    if(IS_ATLAS_DEVICE(dev->device))
    {
        interrupt_status_reg = PCI_ATLAS_SYS_CTRL_REGS_OFFSET + PCI_ATLAS_INTERRUPT_STATUS;
        interrupt_clear_reg  = PCI_ATLAS_SYS_CTRL_REGS_OFFSET + PCI_ATLAS_INTERRUPT_CLEAR;
        interrupt_enable_reg = PCI_ATLAS_SYS_CTRL_REGS_OFFSET + PCI_ATLAS_INTERRUPT_ENABLE;
        test_ctrl_reg = PCI_ATLAS_SYS_CTRL_REGS_OFFSET + PCI_ATLAS_TEST_CTRL;
    }
    else if(IS_APOLLO_DEVICE(dev->device))
    {
        interrupt_status_reg = PCI_ATLAS_SYS_CTRL_REGS_OFFSET+PCI_APOLLO_INTERRUPT_STATUS;
        interrupt_clear_reg  = PCI_ATLAS_SYS_CTRL_REGS_OFFSET+PCI_APOLLO_INTERRUPT_CLEAR;
        interrupt_enable_reg = PCI_ATLAS_SYS_CTRL_REGS_OFFSET+PCI_APOLLO_INTERRUPT_ENABLE;
        test_ctrl_reg = PCI_ATLAS_SYS_CTRL_REGS_OFFSET + PCI_APOLLO_TEST_CTRL;
    }

    /* Install the ISR callback...*/
    ret = request_irq(data->irq, &pci_isrcb, IRQF_SHARED, "msvdx", (void *)dev);
    IMG_ASSERT(ret == 0);
    if (ret != 0)
    {
        return -ENODEV;
    }

    /* If Atlas FPGA...*/
    if(!IS_AVNET_DEVICE(dev->device))
    {
        /* We need to enable interrupts for the embedded device via the Atlas interrupt controller...*/
        IMG_UINT32 ui32Enable = readreg32(dev, PCI_ATLAS_SYS_CTRL_REGS_BAR, interrupt_enable_reg);
        ui32Enable |= PCI_ATLAS_MASTER_ENABLE | PCI_ATLAS_DEVICE_INT;

        writereg32(dev,    PCI_ATLAS_SYS_CTRL_REGS_BAR, interrupt_enable_reg, ui32Enable);
    }

#ifdef FPGA_BUS_MASTERING
	writereg32(dev, PCI_ATLAS_SYS_CTRL_REGS_BAR, test_ctrl_reg, 0x0);
#endif

    return 0;

out_disable:
    pci_disable_device(dev);

out_free:
    return -ENODEV;
}

static void pciremove_func(
    struct pci_dev *  dev
)
{
    /* If Atlas FPGA...*/
    if (!IS_AVNET_DEVICE(dev->device))
    {
        /* We need to disable interrupts for the embedded device via the Atlas interrupt controller...*/
        writereg32(dev, PCI_ATLAS_SYS_CTRL_REGS_BAR, interrupt_enable_reg, 0x00000000);
    }

    pci_release_regions(dev);
    pci_disable_device(dev);
}


static IMG_PHYSADDR paddr_to_devpaddr(struct SYSDEVU_sInfo *sysdev, IMG_PHYSADDR paddr) {
#ifdef FPGA_BUS_MASTERING
	return paddr;
#else
	IMG_UINT64 size;
	IMG_PHYSADDR membase, devpaddr = 0x0;
	struct imgpci_prvdata *data = sysdev->pPrivate;

	membase = data->memmap[2].addr;
	size = data->memmap[2].size;

	if ((paddr >= membase) && (paddr < membase + size))
		devpaddr = paddr - membase;
	else {
		IMG_ASSERT(IMG_FALSE);
	}

	/* Return the device physical address...*/
	return devpaddr;
#endif
}


struct SYSDEV_ops device_ops = {
		.free_device = free_device,
		.paddr_to_devpaddr = paddr_to_devpaddr
};


/*
******************************************************************************

@Function    SYSDEVU_VDECRegisterDriver

******************************************************************************/
IMG_RESULT SYSDEVU_VDECRegisterDriver(
    SYSDEVU_sInfo *  psInfo
)
{
    IMG_RESULT ui32Result = IMG_SUCCESS;
    int ret;
    struct imgpci_prvdata *data;
    struct pci_dev *dev;

    ret = pci_register_driver(&img_pci_driver.pci_driver);
    BUG_ON(ret != 0);
    if(ret != 0)
    {
        pci_unregister_driver(&img_pci_driver.pci_driver);
        return IMG_ERROR_FATAL;
    }

    dev = img_pci_driver.pci_dev;
    BUG_ON(dev == IMG_NULL);

    data = (struct imgpci_prvdata *)pci_get_drvdata(dev);
    data->dev = psInfo;

    /* Save register pointer etc....*/
#ifdef FPGA_BUS_MASTERING
    SYSDEVU_SetDevMap(psInfo, 0, 0, 0, 0, IMG_NULL, 0, 0);
#else
    SYSDEVU_SetDevMap(psInfo, 0, 0, 0, data->memmap[2].addr,
                      (IMG_UINT32 *)data->memmap[2].km_addr, data->memmap[2].size, 0);
#endif
    SYSDEVU_SetDeviceOps(psInfo, &device_ops);

    psInfo->pPrivate = data;

#ifdef FPGA_BUS_MASTERING
    ui32Result = SYSMEMKM_AddSystemMemory(&psInfo->sMemPool);
#else
    ui32Result = SYSMEMKM_AddCarveoutMemory((IMG_UINTPTR)psInfo->pui32KmMemBase,
                                            psInfo->paPhysMemBase,
                                            psInfo->ui32MemSize,
                                            &psInfo->sMemPool);
#endif
    IMG_ASSERT(IMG_SUCCESS == ui32Result);

    return ui32Result;
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
