#define pr_fmt(fmt) "ION IOMMU PTB " fmt

#include <linux/spinlock.h>
#include <linux/err.h>
#include <linux/genalloc.h>
#include <linux/io.h>
#include <linux/ion.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/gfp.h>                    /*alloc_pages*/
#include "../../ion_priv.h"
#include <linux/ion_iommu_heap.h>
#include <linux/device.h>
#include <linux/dma-direction.h>
#include <linux/kref.h>
#include <linux/mm_types.h>
#include <linux/mutex.h>
#include <linux/rbtree.h>
#include <linux/sched.h>
#include <linux/shrinker.h>
#include <linux/types.h>
#include <linux/device.h>

extern struct tag_iommu_context iommu_context;
#define iova_to_index(iova) \
                (iova/(iommu_context.iommu_run_param->page_size))

#define size_to_page_counts(size)\
                  (!!(size%iommu_context.iommu_run_param->page_size)? \
                  (size/iommu_context.iommu_run_param->page_size+1):  \
                  (size/iommu_context.iommu_run_param->page_size))

#define page_size_to_order(page_size) \
        ((g_iommu_pages_pool.page_size/PAGE_SIZE)-1)

struct tag_pages_pool g_iommu_pages_pool = {0};
static unsigned int iommu_high_order_gfp_flags = (GFP_HIGHUSER | __GFP_NOWARN |
                                                    __GFP_NORETRY) & ~__GFP_WAIT;
static unsigned int iommu_low_order_gfp_flags = (GFP_HIGHUSER | __GFP_NOWARN);
static const unsigned int iommu_orders[] = {8, 4, 0};
static const int iommu_num_orders = ARRAY_SIZE(iommu_orders);
static int iommu_alloc_failed_order = 0;
unsigned long iommu_alloc_failed_time_stamp;

static int is_page_entry_valid(unsigned int index,unsigned int lenth)
{
    int i;
    unsigned int* page_table = (unsigned int*)
                                iommu_context.iommu_page_table_info->page_tbl_base;

    for (i=0;i<lenth;i++){
        if ((page_table[index+i]&PAGE_ENTRY_VALID_MASK) != 0){
            return (page_table[index+i]&PAGE_ENTRY_VALID_MASK);
        }
    }
    return 0;
}

static int is_page_entry_invalid(unsigned int index,unsigned int lenth)
{
    int i;
    unsigned int* page_table = (unsigned int*)
                                iommu_context.iommu_page_table_info->page_tbl_base;

    for (i=0;i<lenth;i++){

        if ((page_table[index+i]&PAGE_ENTRY_VALID_MASK) == 0)
        {
            return (page_table[index+i]&PAGE_ENTRY_VALID_MASK);
        }
    }

    return 1;
}

static void insert_page_entry(unsigned int index,unsigned int entry)
{
    unsigned int *page_table = (unsigned int *)
                                iommu_context.iommu_page_table_info->page_tbl_base;

    page_table[index] = entry;
}

static void del_page_entry(unsigned int index,unsigned int lenth)
{
    int i;
    unsigned int* page_table = (unsigned int *)
                               iommu_context.iommu_page_table_info->page_tbl_base;
    for(i=0;i<lenth;i++){
         page_table[index+i] = 0;
    }
}

static unsigned int format_page_tbl_entry(struct page* page_node)
{
    unsigned int iommu_entry = page_to_phys(page_node);
    iommu_entry |= PAGE_ENTRY_VALID;
    return iommu_entry;
}

int iommu_map_iodomain(unsigned int iova_start,
                       unsigned int size,
                       struct tag_mem_node_info* p_mem_node_info,
                       struct tag_iommu_context* ctx)
{
    unsigned int i = 0;
    int map_numb = 0;
    struct page* page;
    unsigned int page_entry  = 0;
    struct tag_page_node* tmp_page = NULL;
    unsigned int index_start = iova_to_index(iova_start);
    unsigned int tmp_start   = index_start;
    unsigned int pg_cnts       = size_to_page_counts(size);

    //pr_err("in %s line is %d\n",__func__,__LINE__);
    if ((!p_mem_node_info)
       ||(!p_mem_node_info->sg_list)
       ||(!ctx)){
        pr_err("in %s @%d param invalid\n",__func__,__LINE__);
        return -1;
    }
    //pr_err("in %s line is %d\n",__func__,__LINE__);

    if (is_page_entry_valid(index_start,pg_cnts)){
        pr_err("in %s @%d entry_valid invalid\n",__func__,__LINE__);
        return -1;
    }
    //pr_err("in %s line is %d\n",__func__,__LINE__);

    tmp_page = p_mem_node_info->sg_list;
    while (NULL != tmp_page) {
        
        map_numb = (PAGE_SIZE << tmp_page->order)/g_iommu_pages_pool.page_size;
        page = tmp_page->base;
        while (map_numb--){
            i++;
    	    page_entry = format_page_tbl_entry(page);
    	    insert_page_entry(tmp_start,page_entry);
    	    tmp_start++;
            page += (g_iommu_pages_pool.page_size/PAGE_SIZE);
        }
        
	    tmp_page = tmp_page->next;

    }
    //pr_err("in %s line is %d\n",__func__,__LINE__);
    iommu_invalid_entry(index_start, index_start + pg_cnts - 1);
    return 0;
}
EXPORT_SYMBOL(iommu_map_iodomain);

void iommu_unmap_iodomain(unsigned int iova_start,
                       unsigned int size,
                       struct tag_mem_node_info* p_mem_node_info,
                       struct tag_iommu_context* ctx)
{
    unsigned int lenth = size_to_page_counts(size);
    unsigned int start_index = iova_to_index(iova_start);

    if (!is_page_entry_invalid(start_index,lenth)){
        pr_err("is_page_entry_invalid \n");
        return;
    }
    del_page_entry(start_index,lenth);
    iommu_invalid_entry(start_index, start_index + lenth - 1);
}
EXPORT_SYMBOL(iommu_unmap_iodomain);

void* get_iommu_pages_pool(void)
{
    return (void*)&g_iommu_pages_pool;
}
EXPORT_SYMBOL(get_iommu_pages_pool);

void iommu_pages_pool_init(struct tag_iommu_context* ctx)
{
    struct tag_pages_pool *pg_pl = &g_iommu_pages_pool;

    memset((void *)pg_pl,0x0,sizeof(*pg_pl));
    BUG_ON(ctx->iommu_run_param == NULL);

    pg_pl->page_size = ctx->iommu_run_param->page_size;
    pg_pl->head = NULL;
    sema_init(&(pg_pl->page_pool_sema),1);
    pg_pl->counts = 0;
    pg_pl->pages_threshold = 0;

    ctx->pages_pool = pg_pl;

    ctx->init_flag |= IOMMU_PAGES_POOL_INIT;
}
EXPORT_SYMBOL(iommu_pages_pool_init);

#if 1
struct tag_page_node* iommu_alloc_pages(gfp_t gfp_mask, unsigned int counts)
{
    int i = 0;
    unsigned int order = 0;
    unsigned int iommu_flags;
    struct tag_page_node  head        = {0};
    struct tag_page_node* p_tmp       = NULL;
    struct tag_page_node* pg_nd       = NULL;
    int remain_size = (int)(counts * g_iommu_pages_pool.page_size);
    //pr_err("remain_size is %dK\n",remain_size>>10);
    while (remain_size > 0){
         pg_nd = (struct tag_page_node*)kmalloc(sizeof(*pg_nd),GFP_KERNEL);

        if(NULL == pg_nd){
            pr_err("in %s @%d kmalloc p_page_node is fail\n",__func__,__LINE__);
            goto iommu_fail;
        }
        if (time_before(jiffies, iommu_alloc_failed_time_stamp + HZ * 10)) {
            if (8 == iommu_alloc_failed_order) {
                i = 1;
            }
            else if (4 == iommu_alloc_failed_order){
               
                i = 2;
            }
            //pr_err("i %d iommu_alloc_failed_order is %d\n",i,iommu_alloc_failed_order);
        } else {
            if (iommu_alloc_failed_order != 0) {
                iommu_alloc_failed_order = 0;
                i = 0;
                //pr_err("i %d iommu_alloc_failed_order is %d\n",i,iommu_alloc_failed_order);
            
            }
        }
        
    	for (; i < iommu_num_orders; i++) {
    		if (remain_size < (PAGE_SIZE << (iommu_orders[i])))
    			continue;

            if (i == iommu_num_orders - 1){
                iommu_flags = iommu_low_order_gfp_flags;
            }
            else{
                iommu_flags = iommu_high_order_gfp_flags;
            }
            pg_nd->next = NULL;
            //pr_err("i %d alloc_pages order is %d line %d\n",i,iommu_orders[i],__LINE__);
    		pg_nd->base = alloc_pages(iommu_flags,iommu_orders[i]);
    		if (!pg_nd->base) {
    			iommu_alloc_failed_time_stamp = jiffies;
    			iommu_alloc_failed_order = iommu_orders[i];
                //pr_err("i %d alloc_pages order is %d line %d\n",i,iommu_orders[i],__LINE__);
    			continue;
    		}
    		pg_nd->order = iommu_orders[i];

            p_tmp       = head.next;
            head.next   = pg_nd;
            pg_nd->next = p_tmp;
            pg_nd = NULL;

            remain_size -= (PAGE_SIZE << (iommu_orders[i]));
            break;
    	}

        //pr_err("i %d iommu_num_orders %d\n",i,iommu_num_orders);
        if (i == iommu_num_orders){
            pr_err("alloc_pages is failed\n");
            goto iommu_fail;        
        }    
    }
    
    return (head.next);

iommu_fail:
    while (NULL != head.next){
        p_tmp = head.next;
        __free_pages(p_tmp->base,p_tmp->order);
        head.next = p_tmp->next;
        kfree(p_tmp);
    }
    if(NULL != pg_nd){
        kfree(pg_nd);
    }


    return NULL;
}
#else
struct tag_page_node* iommu_alloc_pages(gfp_t gfp_mask, unsigned int counts)
{
    int i = 0;
    unsigned int order = 0;
    struct tag_page_node  head        = {0};
    struct tag_page_node* p_tmp       = NULL;
    struct tag_page_node* pg_nd = NULL;

    order = g_iommu_pages_pool.page_size/PAGE_SIZE - 1;

    for (i = 0;i < counts;i++){
         pg_nd = (struct tag_page_node*)kmalloc(sizeof(*pg_nd),GFP_KERNEL);

        if(NULL == pg_nd){
            pr_err("in %s @%d kmalloc p_page_node is fail\n",__func__,__LINE__);
            goto iommu_fail;
        }
        pg_nd->next = NULL;
        pg_nd->base = alloc_pages(iommu_order_gfp_flags,order);
        if(NULL == pg_nd->base){
            pr_err("in %s @line %d alloc page is failed i is %d\n",__func__,__LINE__,i);
            goto iommu_fail;
        }
        p_tmp             = head.next;
        head.next         = pg_nd;
        pg_nd->next = p_tmp;
        pg_nd = NULL;
    }
    return (head.next);

iommu_fail:
    while (NULL != head.next){
        p_tmp = head.next;
        __free_pages(p_tmp->base,order);
        head.next = p_tmp->next;
        kfree(p_tmp);
    }
    if(NULL != pg_nd){
        kfree(pg_nd);
    }


    return NULL;
}
#endif


EXPORT_SYMBOL(iommu_alloc_pages);

void iommu_free_pages(struct tag_page_node* node)
{
    struct tag_page_node* p_cur       = NULL;
    struct tag_page_node* p_before    = NULL;

    if (NULL == node){
        pr_err("in %s @%d node is null\n",__func__,__LINE__);
        return;
    }

    p_cur = node;
    while(NULL != p_cur) {
        p_before = p_cur;
        __free_pages(p_cur->base,p_cur->order);
        p_cur = p_cur->next;
        kfree(p_before);
    }
}
EXPORT_SYMBOL(iommu_free_pages);
