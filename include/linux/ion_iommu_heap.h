#ifndef _ION_MMU_HEAP_H_
#define _ION_MMU_HEAP_H_

#include <linux/mm_types.h>
#include <linux/genalloc.h>
#include <linux/semaphore.h>
#include <linux/dcache.h>
#include <linux/ion.h>
#include <linux/kernel.h>
#include <linux/of.h>

#define IOMMU_CONFIG_4_512_128_64       (0x110)
#define IOMMU_CONFIG_4_512_256_128      (0x160)
#define IOMMU_CONFIG_4_1024_256_128     (0x164)
#define IOMMU_CONFIG_8_1024_256_64      (0x156)
#define IOMMU_CONFIG_8_512_256_64       (0x152)
#define IOMMU_CONFIG_8_512_512_128      (0xA2)
#define IOMMU_CONFIG_8_1024_512_128     (0x1A6)
#define IOMMU_CONFIG_8_2048_512_128     (0x1AA)

#define IOMMU_KERN_MAP        (1<<0)
#define IOMMU_USER_MAP        (1<<1)
#define IOMMU_IO_MAP          (1<<2)
#define PAGE_ENTRY_VALID_MASK (0x3)
#define PAGE_ENTRY_VALID      (0x1)
#define IOMMU_IO_DOMAIN       (256<<20)             
#define IOMMU_PAGE_SIZE       (4<<10)               
#define IOMMU_PAGE_ALIGN_16K  (16<<10)
#define IOMMU_DDR_SIZE        (1024<<20)            
#define IOMMU_PAGE_ALIGN(len)  ALIGN(len, 8<<10)
#define IOMMU_PAGE_SHIFT      (IOMMU_PAGE_SIZE / (8<<10)? 13:12)
#define IOMMU_DEBUG_REG_SECTION_NUM (2)

#if defined(IOMMU_DBG)
#define iommu_pr_debug(fmt,...) \
	printk(KERN_ERR fmt,##__VA_ARGS__)
#else
#define iommu_pr_debug(fmt,...)
#endif
#define iommu_pr_error(fmt,...) \
	printk(KERN_ERR fmt,##__VA_ARGS__)

struct  tag_page_nodes_info
{
     struct page* page;                            /*标志PAGE_SIZE*/
     struct tag_page_node *next;
};

struct  tag_mem_node_info
{
    char name[14];                                 /*apps alloc mem name*/
    unsigned char map_flag;                        /*KERN_MAP|USER_MAP|IO_MAP*/
    unsigned char reserve;                         /* reserve the space       */
    unsigned int alloc_jiffies;                    /*just for calculate the use time*/
    unsigned int free_jiffies;
    unsigned int size;                             /*    the domain size     */
    unsigned int iova;                             /*   apps get  io addr  */
    void *userva;                           /*apps get  useraddr*/
    void *kernva;                           /*apps get  kerneladdr*/
    struct tag_page_node* sg_list;                 /*store page_node list*/
    struct ion_buffer *ion_buffer;                 /*point to the ion_handle*/
    struct dentry *debug_root;
    struct tag_mem_node_info *next;                /*mem_domain */

};

struct tag_iommu_run_param
{
     unsigned int page_size;
     unsigned int io_domain;
     unsigned int reserve;
};

struct  tag_iommu_page_table
{
     unsigned int page_tbl_size;
     void* page_tbl_base;
     struct semaphore page_tbl_sema;
};

struct tag_iommu_debug_reg{
    unsigned int reg_offset;
    unsigned int reg_size;
};

struct tag_iommu_chip
{
      unsigned int   irq;/*iommu ip interrupt*/
      unsigned char  chip_state;
      unsigned char  chip_ver;
      unsigned char  reserve[2];       /*maybe useful but not now*/
      struct tag_iommu_debug_reg debug_reg[IOMMU_DEBUG_REG_SECTION_NUM];
      void*  irq_handle;               /*IOMMU IP ‘s irq handle*/
      void __iomem   *reg_base;
      struct clk     *iommu_peri_clk;
      struct clk     *iommu_clk;
      struct clk     *media_sc_clk;
};

struct tag_heap_buffer_mem
{
    struct tag_mem_node_info *next;
    struct semaphore heap_buffer_sema;
    int    cur_cnt;
};

struct tag_log_zone
{
    void *log_base;
    void *log_head;
    void *log_tail;
    unsigned int log_size;
    struct semaphore log_sema;
};

struct tag_iommu_context
{
     unsigned int init_flag; 
     struct  device_node *of_node;
     struct  tag_iommu_run_param*  iommu_run_param;         /*malloc to run param*/
     struct  tag_iommu_page_table* iommu_page_table_info;   /*malloc to pg tbl*/
     struct  tag_heap_buffer_mem* heap_buffer_mem;         /*point to the buffer list*/
     struct  tag_pages_pool*      pages_pool;              /*point to the pg pool*/
     struct  tag_iommu_chip*       iommu_chip;              /*point to the iommu chip*/
     struct  dentry * iommu_dbg_root;                      /*debugfs entry point*/
     struct  tag_log_zone   log_zone;                       /*log zone size 512K*/     

};
typedef unsigned int page_tbl_entry;


struct tag_page_node
{
     struct page *base;                   /*申请物理页描述符地址*/
	 unsigned int order;
     struct tag_page_node * next;         /*指向下一个page_node*/
};

struct tag_pages_pool
{
    unsigned int page_size;
    unsigned int counts;
    unsigned int pages_threshold;
    struct tag_page_node*  head;
    struct semaphore page_pool_sema;
};

enum iommu_init_flag_type
{
    IOMMU_CTX_INIT        = (1 << 0),
    IOMMU_DBGFS_INIT      = (1 << 1),
    IOMMU_PAGES_POOL_INIT = (1 << 2),
    IOMMU_IPV100_INIT     = (1 << 3),
};

extern void iommu_free_pages(struct tag_page_node* head);
extern struct tag_page_node* iommu_alloc_pages(gfp_t gfp_mask, unsigned int counts);
extern int iommu_map_iodomain(unsigned int iova_start,
                       unsigned int size,
                       struct tag_mem_node_info* p_mem_node_info,
                       struct tag_iommu_context* p_iommu_ctx);

extern void iommu_unmap_iodomain(unsigned int iova_start,
                       unsigned int size,
                       struct tag_mem_node_info* p_mem_node_info,
                       struct tag_iommu_context* p_iommu_ctx);

typedef void (*iommu_vpu_cb)(void);
extern void iommu_set_vpu_onoff(int onoff, iommu_vpu_cb vpu_cb);
extern void iommu_ip_init(struct tag_iommu_context* p_iommu_ctx);
extern void iommu_pages_pool_init(struct tag_iommu_context* p_iommu_ctx);
extern void iommu_proload_entry(unsigned int start_index, unsigned int end_index);
extern void iommu_invalid_entry(unsigned int start_index, unsigned int end_index);
extern struct ion_heap *ion_iommu_heap_create(struct ion_platform_heap *heap_data);
extern struct tag_iommu_context* iommu_build_params(void);
void iommu_init(struct tag_iommu_context* context);
extern void iommu_ip_prepare(void);
extern int iommu_remap_pfn_range(struct ion_handle *handle,struct vm_area_struct *vma, unsigned long addr1,
		    unsigned long pfn, unsigned long size, pgprot_t prot);
#endif
