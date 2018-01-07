#define pr_fmt(fmt) "ION IOMMU CHIP  " fmt

#include <linux/spinlock.h>
#include <linux/err.h>
#include <linux/genalloc.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/ion.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include "../../ion_priv.h"
#include <linux/ion_iommu_heap.h>
#include <linux/hisi/hisi_ion.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/sizes.h>

#include "soc_media_sctrl_interface.h"
#include "soc_baseaddr_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "soc_pmctrl_interface.h"
#include "soc_ao_sctrl_interface.h"
#include "soc_peri_sctrl_interface.h"
#include "soc_smmu_interface.h"
#include "soc_irqs.h"
#include <linux/hisi/reg_ops.h>


#define IOMMU_INIT       0x1
#define IOMMU_RUNNING    0x2
#define IOMMU_SUSPEND    0x3
#define IOMMU_STOP       0x4

#define SMMU_CTRL_OFFSET             (0x0000)
#define SMMU_ENABLE_OFFSET           (0x0004)
#define SMMU_PTBR_OFFSET             (0x0008)
#define SMMU_START_OFFSET            (0x000C)
#define SMMU_END_OFFSET              (0x0010)
#define SMMU_INTMASK_OFFSET          (0x0014)
#define SMMU_RINTSTS_OFFSET          (0x0018)
#define SMMU_MINTSTS_OFFSET          (0x001C)
#define SMMU_INTCLR_OFFSET           (0x0020)
#define SMMU_STATUS_OFFSET           (0x0024)
#define SMMU_AXIID_OFFSET            (0x0028)
#define SMMU_CNTCTRL_OFFSET          (0x002C)
#define SMMU_TRANSCNT_OFFSET         (0x0030)
#define SMMU_L0TLBHITCNT_OFFSET      (0x0034)
#define SMMU_L1TLBHITCNT_OFFSET      (0x0038)
#define SMMU_WRAPCNT_OFFSET          (0x003C)
#define SMMU_SEC_START_OFFSET        (0x0040)
#define SMMU_SEC_END_OFFSET          (0x0044)
#define SMMU_VERSION_OFFSET          (0x0048)
#define SMMU_IPTSRC_OFFSET           (0x004C)
#define SMMU_IPTPA_OFFSET            (0x0050)
#define SMMU_TRBA_OFFSET             (0x0054)
#define SMMU_BYS_START_OFFSET        (0x0058)
#define SMMU_BYS_END_OFFSET          (0x005C)
#define SMMU_RAM_OFFSET              (0x1000)
#define SMMU_REGS_MAX                (15)

#define IOS_STRING "LogZoneDeadLinE"

static DEFINE_SPINLOCK(iommu_spinlock);
extern void dump_isp_mac_reg(void);
extern struct tag_iommu_context iommu_context;
static dma_addr_t iommu_tlb_base_phys;
void iommu_ip_enable(struct tag_iommu_chip *chip);
static irqreturn_t iommu_interrupt_handle(int irq, void *dev_id);

static struct  tag_iommu_chip st_iommu_chip = {0};

static unsigned int iommu_regs_value[SMMU_REGS_MAX] = {0};

/*get ddr size from cmdline*/
static int ddr_size;

static int __init early_hisi_ddr_size(char *p)
{
   phys_addr_t size;
   char *endp;
   size  = memparse(p, &endp);
   ddr_size = size;
   return 0;
}
early_param("iommu_ddr_size", early_hisi_ddr_size);

static iommu_vpu_cb vpu_callback = NULL;
static int vpu_is_on = 0;

void iommu_set_vpu_onoff(int onoff,iommu_vpu_cb vpu_cb)
{
    vpu_is_on = onoff;
    vpu_callback = vpu_cb;

    //when vpu off, unmask iommu
    if (onoff == 0) {
        if (NULL != st_iommu_chip.reg_base) {
             writel(0x0,st_iommu_chip.reg_base+SMMU_INTMASK_OFFSET);
        }
    }
}
EXPORT_SYMBOL(iommu_set_vpu_onoff);

static int iommu_ip_resume(struct platform_device *pdev)
{
    unsigned int tbl_sz = iommu_context.iommu_page_table_info->page_tbl_size;
    struct tag_iommu_chip* chip = &st_iommu_chip;
    unsigned int value = 0;
    unsigned int reg = 0;
    int wait_cnt = 10000;

    /*
     *  enable iommu peri clk
     */    
    if(chip->media_sc_clk){
        clk_prepare_enable(chip->media_sc_clk);
	}  

    /*
     *  enable iommu media clk
    */    
    if(chip->iommu_clk){
        clk_prepare_enable(chip->iommu_clk); 
	}  

    /* enable SMMU clk */
    //phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_ADDR(0), 8, 8, 1);

    /* disreset SMMU */
    value = readl((void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));
    writel((value|(0x1<<7)),(void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR))); 
    //phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(0), 7, 7, 1);

    /* bypass 去使能smmu */
    value = readl((void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));
    writel((value&(~(0x1<<2))),(void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));

    //phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(0), 2, 2, 0);


    writel(iommu_regs_value[2],chip->reg_base+SMMU_PTBR_OFFSET);
    writel(iommu_regs_value[5],chip->reg_base+SMMU_STATUS_OFFSET);
    writel(iommu_regs_value[6],chip->reg_base+SMMU_AXIID_OFFSET);
    writel(iommu_regs_value[7],chip->reg_base+SMMU_SEC_START_OFFSET);
    writel(iommu_regs_value[8],chip->reg_base+SMMU_SEC_END_OFFSET);
    writel(iommu_regs_value[9],chip->reg_base+SMMU_VERSION_OFFSET);
    writel(iommu_regs_value[10],chip->reg_base+SMMU_IPTSRC_OFFSET);
    writel(iommu_regs_value[11],chip->reg_base+SMMU_IPTPA_OFFSET);
    writel(iommu_regs_value[12],chip->reg_base+SMMU_TRBA_OFFSET);
    writel(iommu_regs_value[13],chip->reg_base+SMMU_BYS_START_OFFSET);
    writel(iommu_regs_value[14],chip->reg_base+SMMU_BYS_END_OFFSET);

    writel(iommu_tlb_base_phys + sizeof(page_tbl_entry),chip->reg_base+SMMU_START_OFFSET);
    writel(iommu_tlb_base_phys + tbl_sz,chip->reg_base+SMMU_END_OFFSET);
    /*最后使能iommu*/
    writel(iommu_regs_value[1],chip->reg_base+SMMU_ENABLE_OFFSET);

    writel((iommu_regs_value[0]|(1<<10)),chip->reg_base+SMMU_CTRL_OFFSET);
    while(--wait_cnt) {
        reg = readl((chip->reg_base+SMMU_CTRL_OFFSET));
            if ( 0x0 == (reg & 0x400)){
            pr_err("IOMMU IP INIT SUCCESS\n");
            break;
        }
    }
    WARN_ON(!wait_cnt);
    chip->chip_state = IOMMU_RUNNING;

    return 0;
}

static void restore_iommu_regs_value(struct  tag_iommu_chip* chip)
{
    iommu_regs_value[0]= readl((void *)(chip->reg_base + SMMU_CTRL_OFFSET));
    iommu_regs_value[1]= readl((void *)(chip->reg_base + SMMU_ENABLE_OFFSET));
    iommu_regs_value[2]= readl((void *)(chip->reg_base + SMMU_PTBR_OFFSET));
    iommu_regs_value[3]= readl((void *)(chip->reg_base + SMMU_START_OFFSET));
    iommu_regs_value[4]= readl((void *)(chip->reg_base + SMMU_END_OFFSET));
    iommu_regs_value[5]= readl((void *)(chip->reg_base + SMMU_STATUS_OFFSET));
    iommu_regs_value[6]= readl((void *)(chip->reg_base + SMMU_AXIID_OFFSET));
    iommu_regs_value[7]= readl((void *)(chip->reg_base + SMMU_SEC_START_OFFSET));
    iommu_regs_value[8]= readl((void *)(chip->reg_base + SMMU_SEC_END_OFFSET));
    iommu_regs_value[9]= readl((void *)(chip->reg_base + SMMU_VERSION_OFFSET));
    iommu_regs_value[10]= readl((void *)(chip->reg_base + SMMU_IPTSRC_OFFSET));
    iommu_regs_value[11]= readl((void *)(chip->reg_base + SMMU_IPTPA_OFFSET));
    iommu_regs_value[12]= readl((void *)(chip->reg_base + SMMU_TRBA_OFFSET));
    iommu_regs_value[13] = readl((void *)(chip->reg_base + SMMU_BYS_START_OFFSET));
    iommu_regs_value[14] = readl((void *)(chip->reg_base + SMMU_BYS_END_OFFSET));
}

static int iommu_ip_suspend(struct platform_device *pdev , pm_message_t state)
{
    struct tag_iommu_chip* chip = &st_iommu_chip;

    unsigned int value = 0;

    restore_iommu_regs_value(chip);

    /* bypass 去使能smmu */
    value = readl((void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));
    writel((value|(0x1<<2)),(void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));
    //phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(0), 2, 2, 1);

    /* reset SMMU */
    value = readl((void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR)));
    writel((value&(~(0x1<<7))),(void *)HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_ADDR(SOC_MEDIA_SCTRL_BASE_ADDR))); 
    //phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_RSTEN_ADDR(0), 7, 7, 0);

    /*
     *  disable iommu media clk
    */    
    if(chip->iommu_clk){
        clk_disable_unprepare(chip->iommu_clk); 
	}  

    /*
     *  disable iommu peri clk
     */    
    if(chip->media_sc_clk){
        clk_disable_unprepare(chip->media_sc_clk);
	}  
    
    /* disable SMMU clk */
    //phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_CLKDIS_ADDR(0), 8, 8, 0);

    chip->chip_state = IOMMU_SUSPEND;
	return 0;
}
static void iommu_ctx_init(struct tag_iommu_context* ctx, struct device* dev)
{	
	struct  tag_iommu_page_table  *pg_tlb = NULL;
	struct  tag_iommu_run_param   *param = NULL;
	unsigned int va_size = 0x100000;
	void *va_iommu_tlb = NULL;
	dma_addr_t phys_iommu_tlb_base = 0;	
	
	pr_info("in %s start\n",__func__);
	if (!ctx){
		return;
	}
	param = ctx->iommu_run_param;

	/*
	*  init pg_tlb
	*/

	pg_tlb = (struct tag_iommu_page_table*)kmalloc(sizeof(*pg_tlb),GFP_KERNEL);
	if(!pg_tlb){
		pr_err("in %s @%d kmalloc pg_tlb is failed\n",__func__,__LINE__);
		return;
	}
	pg_tlb->page_tbl_size = ((param->io_domain)/(param->page_size)) * sizeof(page_tbl_entry);
    
	sema_init(&pg_tlb->page_tbl_sema,1);

	dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	pg_tlb->page_tbl_base = dma_alloc_coherent(dev,va_size,&iommu_tlb_base_phys,GFP_KERNEL);
	
	if(!pg_tlb->page_tbl_base){
		kfree(pg_tlb);
		pr_err("dma_alloc_coherent is failed\n");
		return;
	}

	/*
	*phys of iommu_ptb need align 1M
	*/
	if (iommu_tlb_base_phys & (SZ_1M - 1)) {
		dma_free_coherent(dev,va_size,pg_tlb->page_tbl_base,iommu_tlb_base_phys);
		pg_tlb->page_tbl_base = NULL;
		iommu_tlb_base_phys = 0;
		va_iommu_tlb = dma_alloc_coherent(dev,va_size * 2,&phys_iommu_tlb_base,GFP_KERNEL);
		if(!va_iommu_tlb){
			kfree(pg_tlb);
			pr_err("dma_alloc_coherent is failed\n");
			return;
		}
		
		iommu_tlb_base_phys = (phys_iommu_tlb_base + (SZ_1M - 1)) & (~(SZ_1M - 1));
		pg_tlb->page_tbl_base = (void*)(((unsigned long)va_iommu_tlb + (SZ_1M - 1)) & (~(SZ_1M - 1)));
	}

	memset(pg_tlb->page_tbl_base,0x0,pg_tlb->page_tbl_size);

	/*
	 * bind pg_tbl to ctx
	 */
	ctx->iommu_page_table_info = pg_tlb;


    /*
     * set iommu init flag
     */
    ctx->init_flag |= IOMMU_CTX_INIT;

}

static int get_iommu_debug_info(struct tag_iommu_chip * chip)
{
    struct  device_node* np = NULL;
    struct  device_node* cnp = NULL;
    int ret,index=0;
    np = of_find_compatible_node(NULL,NULL,"hisi,iommu-debug");
    if(np)
    {
        for_each_child_of_node(np,cnp){
            ret = of_property_read_u32(cnp, "reg_base", &chip->debug_reg[index].reg_offset);
            if(ret < 0){
                pr_err("in %s read reg base error! \n",__func__);
                continue;
            }

            ret = of_property_read_u32(cnp, "reg_size", &chip->debug_reg[index].reg_size);
            if(ret < 0){
                pr_err("in %s read reg size error! \n",__func__); 
                continue;
            } 
            pr_info("reg_base =0x%x,reg_size =0x%x \n",chip->debug_reg[index].reg_offset,
            chip->debug_reg[index].reg_size); 
            index ++;
        }

    }else{
        pr_err("in %s can not read iommu-debug node! \n",__func__); 
        return -1;
    }
    return 0;
}

static int iommu_probe(struct platform_device *pdev)
{
 	unsigned int *ver = NULL;
 	struct resource *res = NULL; 
    struct tag_iommu_chip *chip = &st_iommu_chip;
    struct tag_iommu_context* ctx = &iommu_context;
    struct device_node  *np  = pdev->dev.of_node;
    const char* level_zero_cache ;
    int ret;
        
    pr_info("in %s iommu driver is start\n",__func__);

    chip->chip_state = IOMMU_INIT;

    memset((void *)chip,0x0,sizeof(*chip));

    /*
     * get reg resouce
     */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		pr_err("platform_get_resource err\n");
		return -ENOENT;
	}
	
	if (!request_mem_region(res->start,resource_size(res),pdev->name)) {
		pr_err("cannot claim register memory\n");
		return -ENOMEM;
	}
	chip->reg_base = ioremap(res->start,resource_size(res));
	if (!chip->reg_base) {
		pr_err("cannot map register memory\n");
		return -ENOMEM;
	}
	/*
	 * get iommu version
	 */
 	ver = (unsigned int *)of_get_property(np, "hisi,iommu-version", NULL);
 	if(ver){
	    chip->chip_ver = be32_to_cpu(*ver); 
        pr_err("iommu version is %u\n",be32_to_cpu(*ver));
	}

    chip->media_sc_clk = of_clk_get(np, 2);
    if(!chip->media_sc_clk){
	    pr_err("in %s of media_sc_clk failed\n",__func__);
		return -ENODEV;
	}  
    clk_prepare_enable(chip->media_sc_clk);
    /*
     *  get iommu peri clk
     */    
    chip->iommu_peri_clk  = of_clk_get(np, 0);
    if(!chip->iommu_peri_clk){
	    pr_err("in %s of get clk peri failed\n",__func__);
		return -ENODEV;
	}  
    clk_prepare_enable(chip->iommu_peri_clk);
    
    /*
     *  get iommu media clk
    */    
    chip->iommu_clk  = of_clk_get(np, 1);
    if(!chip->iommu_clk){
	    pr_err("in %s of get media clk failed\n",__func__);
		return -ENODEV;
	}  
    clk_prepare_enable(chip->iommu_clk);

	pr_err("in %s of get media clk ok\n",__func__);
    /*
     *get debug reg 
     */
    get_iommu_debug_info(chip);
	/*
	* init iommu context
	*/
	iommu_ctx_init(ctx, &pdev->dev);
    /*
     * media subsys prepare
     */
    iommu_ip_prepare();  

    /*
     * init iommu core 
     */
    iommu_ip_enable(chip);   
	
	/*
	 * get irq resouce
	 */
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res){
	    pr_err("in %s get IRQ IS failed\n",__func__);
		return -ENODEV;
	}
    chip->irq = res->start;
    chip->irq_handle = (void*)iommu_interrupt_handle;
    if(request_irq(chip->irq,(irq_handler_t)chip->irq_handle,IRQF_DISABLED,"iommu",(void *)chip)){
        BUG_ON(1);
    }

    /*
     *  get iommu clk
    
    chip->iommu_clk  = of_clk_get(np, 0);
    if(!chip->iommu_clk){
	    pr_err("in %s of get clk failed\n");
		return -ENODEV;
	}  
    */ 
    chip->chip_state = IOMMU_RUNNING;
    ctx->iommu_chip = chip;
    
    ctx->init_flag |= IOMMU_IPV100_INIT;  
    
    return 0;
}

static int iommu_stub_remove(struct platform_device *pdev)
{
    pr_err("%s iommu driver is start\n",__func__);
    return 0;
}
static struct of_device_id of_iommu_match_tbl[] = {
	{
		.compatible = "hisi,hi6210-iommu",
	},
	{ /* end */ }
};
static struct platform_driver plat_iommu_driver = {
	.driver  ={
		.owner = THIS_MODULE,
		.name = "iommu",
		.of_match_table = of_iommu_match_tbl,		
	},
	.probe  =  iommu_probe,
	.remove =  iommu_stub_remove,
#ifdef CONFIG_PM
	.suspend = iommu_ip_suspend,
	.resume  = iommu_ip_resume,
#endif
};

static irqreturn_t iommu_interrupt_handle(int irq, void *dev_id)
{
	int          i, j;
    int          index;
    unsigned int irq_stat;
    unsigned int irq_rawstat;
    unsigned int pgt = 0;
    unsigned int pc_pgt = 0;
	int offset, end;
	unsigned int value = 0;
	unsigned int reg_val[8] = {0};

    struct tag_iommu_chip *chip = (struct tag_iommu_chip *)dev_id;

    dump_isp_mac_reg();

    irq_stat = readl(chip->reg_base+SMMU_MINTSTS_OFFSET);
    irq_rawstat = readl(chip->reg_base+SMMU_RINTSTS_OFFSET);

    if(vpu_is_on){
        pr_err("in %s vpu is on and mask smmu int\n",__func__);
        writel(0xFF,chip->reg_base+SMMU_INTMASK_OFFSET);
    }

    writel(0xff,chip->reg_base+SMMU_INTCLR_OFFSET);
    pr_err(" irq raw status is %#x \n",irq_rawstat);
    
    pgt = readl(chip->reg_base+SMMU_PTBR_OFFSET);
    pc_pgt = readl(chip->reg_base+SMMU_IPTSRC_OFFSET);

    /*dump all register*/
	pr_err("********* dump smmu reg value*********\n");
	for (i = 0; i < 96; i += 32) {
		pr_err("base: %08x [%08x][%08x][%08x][%08x][%08x][%08x][%08x][%08x]\n",
			chip->reg_base+i,
			readl(chip->reg_base+i),
			readl(chip->reg_base+i+4),
			readl(chip->reg_base+i+8),
			readl(chip->reg_base+i+12),
			readl(chip->reg_base+i+16),
			readl(chip->reg_base+i+20),
			readl(chip->reg_base+i+24),
			readl(chip->reg_base+i+28));
	}

	/*for v8r2 print more debug info*/
	offset = chip->debug_reg[0].reg_offset;
	pr_err("base: %08x [%08x][%08x][%08x][%08x]\n",
		chip->reg_base+offset,
		readl(chip->reg_base+offset),
		readl(chip->reg_base+offset+4),
		readl(chip->reg_base+offset+8),
		readl(chip->reg_base+offset+12));
	offset += 16;
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 4; ++j) {
			value = readl(chip->reg_base+offset);
			value &= ~0x1F;
			value |= 4 * i + j;
			writel(value, chip->reg_base+offset);
			reg_val[2*j] = readl(chip->reg_base+offset);
			reg_val[2*j+1] = readl(chip->reg_base+offset+4);
		}

		pr_err("base: %08x [%08x][%08x][%08x][%08x][%08x][%08x][%08x][%08x]\n",
			chip->reg_base+offset,
			reg_val[0], reg_val[1],
			reg_val[2], reg_val[3],
			reg_val[4], reg_val[5],
			reg_val[6], reg_val[7]);
	}

	for (i = 1; i < IOMMU_DEBUG_REG_SECTION_NUM; i++) {
		offset = chip->debug_reg[i].reg_offset;
		end = offset + chip->debug_reg[i].reg_size;
		for (j = offset; j < end; j += 32) {
			pr_err("base: %08x [%08x][%08x][%08x][%08x][%08x][%08x][%08x][%08x]\n",
				chip->reg_base+j,
				readl(chip->reg_base+j),
				readl(chip->reg_base+j+4),
				readl(chip->reg_base+j+8),
				readl(chip->reg_base+j+12),
				readl(chip->reg_base+j+16),
				readl(chip->reg_base+j+20),
				readl(chip->reg_base+j+24),
				readl(chip->reg_base+j+28));
		}
	}
    
    /*test bit0 to bit5*/
    if (irq_stat & 0x1){
        pr_err("configure error \n");
    }else if (irq_stat & (1<<1)){  
        index = (pc_pgt - pgt)/4;
        pr_err("page table entry is error:IOMMU_VA=0x%x \n",(index * IOMMU_PAGE_SIZE));
    }else if (irq_stat & (1<<2)) {
        pr_err("AXI master0 receive error response \n");
    }else if (irq_stat & (1<<3)) {
        pr_err("AXI master1 receive error response \n");
    }else if (irq_stat & (1<<4)) {
        pr_err("AXI master0 access timeout,not receive peer's response \n");
    }else if (irq_stat & (1<<5)) {
        pr_err("AXI master1 access timeout,not receive peer's response \n");
    }

    if (vpu_callback != NULL){
       (void)vpu_callback();
    }
    WARN_ON(irq_stat&0x3f);
	return IRQ_HANDLED;
}

void iommu_ip_prepare(void)
{
   /*
     this function is used to disable the smmu bypass and disable the smmu reset.
     all the functions is done in the fastboot. so we do not do it again.
    */
        //writel(0x7801,IO_ADDRESS(SOC_PMCTRL_BASE_ADDR + SOC_PMCTRL_GPUPLLCTRL_ADDR(0)));//GPU PLL EN
        //writel(0x7801,IO_ADDRESS(SOC_PMCTRL_BASE_ADDR + SOC_PMCTRL_MEDPLLCTRL_ADDR(0)));//MEDIA PLL EN
        //writel(0x400,IO_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR + SOC_PERI_SCTRL_SC_PERIPH_CLKEN12_ADDR(0)));//MEDIA CLK EN

        //mdelay(10);

        //writel(0x00000004,IO_ADDRESS(SOC_AO_SCTRL_BASE_ADDR + SOC_AO_SCTRL_SC_PW_RSTDIS0_ADDR(0)));// PW RST DIS
        //writel(0x00000004,IO_ADDRESS(SOC_AO_SCTRL_BASE_ADDR + SOC_AO_SCTRL_SC_PW_ISODIS0_ADDR(0)));// ISO DIS
        //writel(0x00000004,IO_ADDRESS(SOC_AO_SCTRL_BASE_ADDR + SOC_AO_SCTRL_SC_PW_CLKEN0_ADDR(0)));// PW CLK EN

        
        //phy_reg_writel(SOC_MEDIA_SCTRL_BASE_ADDR, SOC_MEDIA_SCTRL_SC_MEDIA_SUBSYS_CTRL5_ADDR(0), 2, 2, 0);

        //writel(0x85,IO_ADDRESS(SOC_MEDIA_SCTRL_BASE_ADDR + SOC_MEDIA_SCTRL_SC_MEDIA_CLKCFG0_ADDR(0)));//NOC CLOCK DIV

        //writel(0x40,HISI_VA_ADDRESS(SOC_MEDIA_SCTRL_BASE_ADDR + SOC_MEDIA_SCTRL_SC_MEDIA_RSTDIS_ADDR(0)));//DISABLE sft RESET
        //writel(0x840,IO_ADDRESS(SOC_PERI_SCTRL_BASE_ADDR + SOC_PERI_SCTRL_SC_PERIPH_CLKEN8_ADDR(0)));//MMU&XG2RAM peri CLOCK en
        //writel(0x100,IO_ADDRESS(SOC_MEDIA_SCTRL_BASE_ADDR + SOC_MEDIA_SCTRL_SC_MEDIA_CLKEN_ADDR(0)));//MMU sft CLOCK en

        //pr_info(" set SMMU IP OK !!!!!!!!!!!!!!!!!!\n");
 
}


void iommu_ip_init(struct tag_iommu_context* ctx)
{
    pr_err("start in %s \n",__func__);
    if(platform_driver_register(&plat_iommu_driver)) {
        BUG_ON(1);
    }
    pr_err("end in %s \n",__func__);
}

void iommu_ip_enable(struct tag_iommu_chip *chip)
{
    unsigned int tbl_sz = iommu_context.iommu_page_table_info->page_tbl_size;
    unsigned int iommu_ctrl = iommu_context.iommu_run_param->reserve;
    struct ion_heap_info_data data;
    pr_info("chip->reg_base1 %p \n",chip->reg_base+SMMU_STATUS_OFFSET);


    /*配置SMMU_CTRL寄存器
    0x000001A6:Iommu page size为8K，DDRsize 1G,VA size 512MB，TLB size 128KB
    0x00000160:iommu page size为4K，DDRsize 512MB,VA size 256MB，TLB size 128KB
    0x00000164:iommu page size为4K，DDRsize 1G,VA size 256MB，TLB size 128KB*/
    pr_info("chip->reg_base2 %p \n",chip->reg_base+SMMU_CTRL_OFFSET);
    
    /*set ddr size for smmu*/
    if(SZ_2G == ddr_size){
       iommu_ctrl |= 2<<SOC_SMMU_CTRL_ddr_size_START;
   
    }else if(SZ_1G == ddr_size){
       iommu_ctrl |= 1<<SOC_SMMU_CTRL_ddr_size_START;
       
    }else{
       pr_err("------ddr size is not correct! \n");
    }
    pr_info("------ddr size is 0x%x,iommu_ctrl is 0x%x\n",ddr_size,iommu_ctrl);
    writel(iommu_ctrl,chip->reg_base+SMMU_CTRL_OFFSET);
    pr_info("chip->reg_base3 %p \n",(chip->reg_base+SMMU_PTBR_OFFSET));

    /*配置SMMU_PTBR寄存器*/
    writel(iommu_tlb_base_phys,(chip->reg_base+SMMU_PTBR_OFFSET));
    /*配置SMMU_START/SMMU_END寄存器*/
    writel(iommu_tlb_base_phys + sizeof(page_tbl_entry),(chip->reg_base+SMMU_START_OFFSET));

    writel(iommu_tlb_base_phys + tbl_sz,(chip->reg_base+SMMU_END_OFFSET));

   /*配置SMMU_INTCLR寄存器,清除所有中断*/
    writel(0xFF,(chip->reg_base+SMMU_INTCLR_OFFSET));

    /*
    when the interrupt of invalid pte happend, 
    smmu will access address 0x00000000.
    */
    writel(0x00000000,(chip->reg_base+SMMU_IPTPA_OFFSET));


    if (hisi_ion_get_heap_info(ION_FB_HEAP_ID,&data)){
        pr_err("hisi_ion_get_heap_info is failed\n");
    }
 
}


void iommu_invalid_entry(unsigned int start_index,unsigned int end_index)
{
    unsigned int iommu_ctrl = 0;
    unsigned int try_count = 1000;
    unsigned long flags;
    struct tag_iommu_chip *chip = iommu_context.iommu_chip;
    /*protect control register from access simutaneously*/
    spin_lock_irqsave(&iommu_spinlock,flags);
    writel(iommu_tlb_base_phys + start_index * 4,chip->reg_base+SMMU_START_OFFSET);
    writel((iommu_tlb_base_phys + (end_index+1) * 4 ),chip->reg_base+SMMU_END_OFFSET);
    iommu_ctrl = readl(chip->reg_base+SMMU_CTRL_OFFSET);
    iommu_ctrl = iommu_ctrl | (1<<10);
    writel(iommu_ctrl,chip->reg_base+SMMU_CTRL_OFFSET);
    do{
        iommu_ctrl = readl(chip->reg_base+SMMU_CTRL_OFFSET);
	if ( 0x0 == (iommu_ctrl & 0x400))
	{
	    spin_unlock_irqrestore(&iommu_spinlock,flags);
            return;
        }
        
    }while(try_count--);
    spin_unlock_irqrestore(&iommu_spinlock,flags);
    WARN_ON(!try_count);
}
#if 0
void iommu_disable(void)
{
     struct tag_iommu_chip *chip = iommu_context.iommu_chip;
     struct tag_mem_node_info *tmp_node_info = iommu_context.heap_buffer_mem->next;

     if(NULL == tmp_node_info){
         writel(0xf,chip->reg_base+SMMU_ENABLE_OFFSET);
     }
     else{
         pr_err("SMMU is still busy,Please try again later!\n");
     }
}

void iommu_tlb_domain_pre(void)
{

     struct tag_iommu_chip *chip = iommu_context.iommu_chip;
     unsigned int reg = 0;

     writel(0x2,chip->reg_base+SMMU_CNTCTRL_OFFSET);

	for (;;)
	{
        reg = readl(chip->reg_base+SMMU_CNTCTRL_OFFSET);
		if ( 0x0 == (reg & 0x2))
		{
            pr_err("IOMMU TLBCNT CLEAR SUCCESS\n");
            writel(0x1,chip->reg_base+SMMU_CNTCTRL_OFFSET);
            break;
        }
        mdelay(1);
    }
    writel(0x1,chip->reg_base+SMMU_CNTCTRL_OFFSET);
	 return;
}

unsigned int iommu_tlb_domain(void)
{
     struct tag_iommu_chip *chip = iommu_context.iommu_chip;

	 unsigned int L0_cont_total = 0;
     unsigned int L1_cont_total = 0;
     unsigned int trans_cont_total = 0;
     unsigned int hit_rant = 0;


     writel(0x0,chip->reg_base+SMMU_CNTCTRL_OFFSET);

     L0_cont_total = (readl(chip->reg_base+SMMU_L0TLBHITCNT_OFFSET) >> 4 )+
                                   ((readl(chip->reg_base+SMMU_WRAPCNT_OFFSET) & 0xf00) << 20);

     L1_cont_total = (readl(chip->reg_base+SMMU_L1TLBHITCNT_OFFSET) >> 4) +
                                   ((readl(chip->reg_base+SMMU_WRAPCNT_OFFSET) & 0xf0) << 24);

     trans_cont_total = (readl(chip->reg_base+SMMU_TRANSCNT_OFFSET) >> 10) +
                                   ((readl(chip->reg_base+SMMU_WRAPCNT_OFFSET) & 0xf) << 21);

     if(trans_cont_total){
         hit_rant = (L0_cont_total + L1_cont_total)  / trans_cont_total;
     }
     else{
         pr_err("trans_cont_total is wrong!\n");
     }

     pr_err("iommu hit rant = %ld\n",hit_rant);

     return hit_rant;
}
#endif
