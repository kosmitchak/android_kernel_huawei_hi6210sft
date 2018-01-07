#define pr_fmt(fmt) "ION IOMMU HEAP  " fmt

#include <linux/spinlock.h>
#include <linux/err.h>
#include <linux/genalloc.h>
#include <linux/io.h>
#include <linux/ion.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include "../../ion_priv.h"
#include <linux/ion_iommu_heap.h>
#include <linux/dma-mapping.h>
#include <linux/hisi/hisi_ion.h>


struct ion_iommu_heap
{
	struct ion_heap heap;
	struct gen_pool *pool;
	ion_phys_addr_t base;
};

struct tag_iommu_context iommu_context = {0};

void show_pg_tbl(void)
{
    int i;
    struct tag_iommu_page_table* p = iommu_context.iommu_page_table_info;
    unsigned int  lenth = p->page_tbl_size;
    unsigned int* base  = (unsigned int*)p->page_tbl_base;

    for (i=0;i<lenth;i++){
        iommu_pr_error("%#x\n",base[i]);
    }
}
EXPORT_SYMBOL(show_pg_tbl);

static struct  tag_mem_node_info* iommu_mem_node_alloc(void)
{
	struct  tag_mem_node_info* pnode = vmalloc(sizeof(* pnode));

    if(NULL == pnode){
        pr_err("in %s\n",__func__);
        return NULL;
    }   
    memset(pnode,0x0,sizeof(*pnode));   
    return pnode;
}


static void iommu_mem_node_free(struct  tag_mem_node_info* pnode)
{

    if(NULL == pnode){
        pr_err("in %s pnode is null",__func__);
        return;
    }

    if(pnode->map_flag&IOMMU_KERN_MAP){
        pr_err("why the kernel not unmap\n");
    }
    if(pnode->map_flag&IOMMU_IO_MAP)
    {
        pr_err("why the io not unmap\n");
    }

    vfree(pnode);
}

static void mem_info_to_file(struct tag_mem_node_info* p_mem_node_info)
{
    iommu_pr_debug("START JIFFIES:%d  END JIFFIES:%d",
           p_mem_node_info->alloc_jiffies,p_mem_node_info->free_jiffies);
}
#if 0
void add_mem_node_info(struct tag_mem_node_info* p_mem_node)
{
    struct tag_mem_node_info* tmp = NULL;
    struct tag_heap_buffer_mem* head = NULL;


    head = &st_heap_buffer_mem;

    if (NULL == p_mem_node){
        return;
    }

    down(&head->heap_buffer_sema);
    tmp = head->next;
    head->next = p_mem_node;
    p_mem_node->next = tmp;
    head->cur_cnt++;
    up(&head->heap_buffer_sema);
}

void del_mem_node_info(struct tag_mem_node_info* p_mem_node)
{
    struct tag_mem_node_info* tmp_mem_node = NULL;
    struct tag_mem_node_info* tmp_mem_node_before = NULL;
    struct tag_heap_buffer_mem* phead = &st_heap_buffer_mem;
    int no=0;


    if (NULL == p_mem_node){
        pr_err("NULL == p_mem_node\n");
        return;
    }


    down(&phead->heap_buffer_sema);

    tmp_mem_node = phead->next;
    while(tmp_mem_node != NULL)
    {

        no++;

        if (tmp_mem_node == p_mem_node){
            break;
        }

        tmp_mem_node_before = tmp_mem_node;
        tmp_mem_node = tmp_mem_node->next;
    }

    if (tmp_mem_node == NULL){
        pr_err("cannot find the mem_node\n");
        up(&phead->heap_buffer_sema);
        return;
    }

    if (no != 1) {
        tmp_mem_node_before->next = tmp_mem_node->next;

    }
    else{
        st_heap_buffer_mem.next = tmp_mem_node->next;

    }

    phead->cur_cnt--;
    up(&phead->heap_buffer_sema);

}
#endif

int is_iova_err(ion_phys_addr_t addr)
{
    return 0;
}
EXPORT_SYMBOL(is_iova_err);

ion_phys_addr_t ion_iommu_allocate(struct ion_heap *heap,
                                   struct ion_buffer *buffer,
				                          unsigned long size,
				                          unsigned long align)
{
	unsigned long offset = 0;
	unsigned int pg_cnts = 0;
	unsigned int pg_sz = 0;
	struct ion_iommu_heap*    iommu_heap      = NULL;
    struct tag_mem_node_info* p_mem_node_info = NULL;


	if ((heap == NULL)
	   ||(buffer == NULL)){
        return -1;
	}

	iommu_heap = container_of(heap, struct ion_iommu_heap, heap);

    if (buffer->flags & ION_FLAG_ALLOC_FROM_LAST)
    {
        offset = gen_pool_last_alloc(iommu_heap->pool, size);
    }else{
        offset = gen_pool_alloc(iommu_heap->pool, size);
    }
    
	if (!offset){
	    pr_err("no io domain  so size %08lx\n",size);
	    pr_err("now the pool only have %08lx domain\n",(unsigned long)gen_pool_avail(iommu_heap->pool));
		return ION_CARVEOUT_ALLOCATE_FAIL;
	}

	p_mem_node_info = iommu_mem_node_alloc();
	if (!p_mem_node_info){
	    pr_err("build mem_node_info is failed\n");
        gen_pool_free(iommu_heap->pool, offset, size);
		return ION_CARVEOUT_ALLOCATE_FAIL;
	}
    p_mem_node_info->alloc_jiffies = jiffies;
    p_mem_node_info->ion_buffer    = buffer;
    p_mem_node_info->iova          = offset;
    p_mem_node_info->size          = size;
    p_mem_node_info->next          = NULL;
    p_mem_node_info->map_flag     |= IOMMU_IO_MAP;

    pg_sz     = iommu_context.iommu_run_param->page_size;
    pg_cnts = (!!(size%pg_sz))?(size/pg_sz)+1:(size/pg_sz);

    p_mem_node_info->sg_list = iommu_alloc_pages(align,pg_cnts);
    if (NULL == p_mem_node_info->sg_list){
        pr_err("system no free page\n");
        goto err1;
    }

    if(0 != iommu_map_iodomain(offset,size,p_mem_node_info,&iommu_context)) {
        pr_err("in system iommu_map_iodomain fail\n");
        goto err2;        
    }

    buffer->mem_node_info = p_mem_node_info;

    /*pr_info("alloc IOMMU VA: %x ~ %x\n",p_mem_node_info->iova,
                 p_mem_node_info->iova+p_mem_node_info->size);*/
    
	return offset;
	
err2:
    p_mem_node_info->map_flag &= (~IOMMU_IO_MAP);
    iommu_free_pages(p_mem_node_info->sg_list);
    p_mem_node_info->sg_list = NULL;
err1:
    iommu_mem_node_free(p_mem_node_info);
    gen_pool_free(iommu_heap->pool, offset, size);
    return ION_CARVEOUT_ALLOCATE_FAIL;

}


void ion_iommu_free(struct ion_heap *heap,
                    struct ion_buffer *buffer,
                    ion_phys_addr_t addr,
		            unsigned long size)
{
	struct ion_iommu_heap *iommu_heap = NULL;
    struct tag_mem_node_info* p_mem_node_info = NULL;

	if ((heap == NULL)
	   ||(buffer == NULL)){
        return;
	}
	
	iommu_heap = container_of(heap, struct ion_iommu_heap, heap);
    if (!iommu_heap){
        return;
    }

    if(is_iova_err(addr)){
        return;
    }

    p_mem_node_info = buffer->mem_node_info;
    p_mem_node_info->free_jiffies = jiffies;

    pr_info("free IOMMU VA: %x ~ %x\n",p_mem_node_info->iova,
                 p_mem_node_info->iova+p_mem_node_info->size);

    mem_info_to_file(p_mem_node_info);

    if ((p_mem_node_info->map_flag & IOMMU_KERN_MAP)
       /*||(p_mem_node_info->map_flag & IOMMU_USER_MAP)*/){
        pr_err("MEM map_flag %#x\n",(unsigned int)p_mem_node_info->map_flag);
    }

    p_mem_node_info->map_flag &= (~IOMMU_IO_MAP);
    
    if ((addr != p_mem_node_info->iova)
       ||(size != p_mem_node_info->size)){
        pr_err("addr and size is equal\n");

    }

	if (addr == ION_CARVEOUT_ALLOCATE_FAIL){
		return;
	}

	iommu_unmap_iodomain(addr,size,p_mem_node_info,&iommu_context);
	
    iommu_free_pages(p_mem_node_info->sg_list);
    p_mem_node_info->sg_list = NULL;

    gen_pool_free(iommu_heap->pool, addr, size);
    
    iommu_mem_node_free(p_mem_node_info);

    buffer->mem_node_info = NULL;
}


static int ion_iommu_heap_phys(struct ion_heap *heap,
				  struct ion_buffer *buffer,
				  ion_phys_addr_t *addr, size_t *len)
{
	*addr = buffer->priv_phys;
	*len  = buffer->size;

	return 0;
}


static int ion_iommu_heap_allocate(struct ion_heap *heap,
				      struct ion_buffer *buffer,
				      unsigned long size,
				      unsigned long align,
				      unsigned long flags)
{
    //long size_remaining = IOMMU_PAGE_ALIGN(size);


	buffer->priv_phys = ion_iommu_allocate(heap,buffer, size, align);

	return buffer->priv_phys == ION_CARVEOUT_ALLOCATE_FAIL ? -ENOMEM : 0;
}

static void ion_iommu_heap_free(struct ion_buffer *buffer)
{
	struct ion_heap *heap = buffer->heap;


	ion_iommu_free(heap, buffer,buffer->priv_phys, buffer->size);
	buffer->priv_phys = ION_CARVEOUT_ALLOCATE_FAIL;


}

struct sg_table *ion_iommu_heap_map_dma(struct ion_heap *heap,
					                    struct ion_buffer *buffer)
{
	int ret;
	int npages = 0;
	int i = 0;
	struct scatterlist *sg = NULL;
	struct sg_table *table = NULL;
	struct page *tmp_page = NULL;
	struct tag_page_node* p_page_node = NULL;

    if ((heap == NULL)
       ||(buffer == NULL)){
        return NULL;
    }

    if (buffer->mem_node_info == NULL){
        return NULL;
    }

    //BUG_ON(IOMMU_PAGE_ALIGN(buffer->size) != buffer->mem_node_info->size);
    npages = (buffer->size) / PAGE_SIZE;
    table = kzalloc(sizeof(struct sg_table), GFP_KERNEL);

	if (!table){
		return ERR_PTR(-ENOMEM);
	}
	ret = sg_alloc_table(table, npages, GFP_KERNEL);
	if (ret){
		kfree(table);
		return ERR_PTR(ret);
	}

    sg = table->sgl;
	p_page_node = buffer->mem_node_info->sg_list;
	while(p_page_node != NULL){
         for(tmp_page = p_page_node->base,i=0;
             i<((PAGE_SIZE << (p_page_node->order))/PAGE_SIZE);i++)
         {
			 sg_set_page(sg, tmp_page + i, PAGE_SIZE, 0);
			 sg = sg_next(sg);
         }
         p_page_node = p_page_node->next;
	}
	return table;
}

void ion_iommu_heap_unmap_dma(struct ion_heap *heap,
				 struct ion_buffer *buffer)
{
    if ((!heap)
       ||(!buffer)){
        return;
    }

    if (!buffer->sg_table) {
        return;
    }

	sg_free_table(buffer->sg_table);
	kfree(buffer->sg_table);
	buffer->sg_table = NULL;
}


void *ion_iommu_heap_map_kernel(struct ion_heap *heap,
				   struct ion_buffer *buffer)
{
	int i;
    int npages = 0;
    pgprot_t pgprot;
	void *vaddr;
    struct page **tmp;
	struct page **pages;
	struct page *tmp_page;
	struct tag_page_node* p_page_node;
    struct tag_mem_node_info* p_mem_node_info;


    if ((!heap)
       ||(!buffer)){
        return 0;
    }
	p_mem_node_info = buffer->mem_node_info;

	npages = (buffer->size) / PAGE_SIZE;
	pages = vmalloc(sizeof(struct page *) * npages);
	tmp = pages;

	if (!pages){
		return 0;
	}

	if (buffer->flags & ION_FLAG_CACHED){
		pgprot = PAGE_KERNEL;
	}
	else{
	    pr_err("buffer->flags %#x\n",(unsigned int)buffer->flags);
		pgprot = pgprot_writecombine(PAGE_KERNEL);
	}

	p_page_node = buffer->mem_node_info->sg_list;
	while(p_page_node != NULL){
         for(tmp_page = p_page_node->base,i=0;
             i<((PAGE_SIZE << (p_page_node->order))/PAGE_SIZE);i++){
             *(tmp++) = tmp_page++;
         }
         p_page_node = p_page_node->next;
	}
	vaddr = vmap(pages, npages, VM_MAP, pgprot);
	vfree(pages);
    p_mem_node_info->kernva   = vaddr;
    p_mem_node_info->map_flag |= IOMMU_KERN_MAP;
    pr_info("map kernel domain: %p ~ %p\n",p_mem_node_info->kernva,
                 p_mem_node_info->kernva+p_mem_node_info->size);

	return vaddr;
}

void ion_iommu_heap_unmap_kernel(struct ion_heap *heap,
				    struct ion_buffer *buffer)
{
    if ((heap == NULL)
       ||(buffer == NULL)){
        pr_err("heap and buffer is null\n");
        return;
    }

    if (buffer->mem_node_info == NULL){
        pr_err("buffer->mem_node_info is null\n");
        return;
    }

    BUG_ON(buffer->vaddr != (struct ion_buffer *)buffer->mem_node_info->kernva);
    
    pr_info("ummap kernel domain: %p ~ %p\n",buffer->mem_node_info->kernva,
                 buffer->mem_node_info->kernva+buffer->mem_node_info->size);
                 
	vunmap((void *)buffer->mem_node_info->kernva);
    buffer->mem_node_info->kernva = NULL;
	buffer->vaddr = NULL;
	buffer->mem_node_info->map_flag &= (~IOMMU_KERN_MAP);
	return;
}


int ion_iommu_heap_map_user(struct ion_heap *heap, struct ion_buffer *buffer,
			       struct vm_area_struct *vma)
{
	unsigned long addr;
	struct tag_page_node* p_page_node;
	struct page *tmp_page;


    if ((heap == NULL)
       ||(buffer == NULL)
       ||(vma == NULL)){
        pr_err("  %s: parameter error \n",__func__);
        return -EINVAL;
    }

    addr = (unsigned long)vma->vm_start;
	p_page_node = buffer->mem_node_info->sg_list;
	while(p_page_node != NULL){
        if((buffer->flags  & ION_FLAG_CACHED) == 0) {
        	  vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
        }


        remap_pfn_range(vma, addr, page_to_pfn(p_page_node->base),
                      (PAGE_SIZE << (p_page_node->order)),vma->vm_page_prot);
        addr += (PAGE_SIZE << (p_page_node->order));
        //pr_err("in %s line is %d\n",__func__,__LINE__);
        p_page_node = p_page_node->next;
	}
    buffer->mem_node_info->userva = (void*)vma->vm_start;
    buffer->mem_node_info->map_flag |= IOMMU_USER_MAP;

	return 0;
}


int iommu_remap_pfn_range(struct ion_handle *handle,struct vm_area_struct *vma, unsigned long addr1,
		    unsigned long pfn, unsigned long size, pgprot_t prot)
{
	unsigned long addr;
	struct tag_page_node* p_page_node;
	struct page *tmp_page;
	struct ion_buffer *buffer;


    if ((handle == NULL)
       ||(vma == NULL))
    {
        WARN_ON(1);
        return -EINVAL;
    }

    addr = (unsigned long)vma->vm_start;
    buffer = ion_get_buffer_by_handle(handle);
	addr = vma->vm_start;

	p_page_node = buffer->mem_node_info->sg_list;
	while(p_page_node != NULL)
	{
        if((buffer->flags  & ION_FLAG_CACHED) == 0)
        {

        	  vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
        }
        remap_pfn_range(vma, addr, page_to_pfn(p_page_node->base),
                          (PAGE_SIZE << (p_page_node->order)),vma->vm_page_prot);
        addr += (PAGE_SIZE << (p_page_node->order));
        
        p_page_node = p_page_node->next;

	}

    buffer->mem_node_info->userva = (void*)vma->vm_start;
    buffer->mem_node_info->map_flag |= IOMMU_USER_MAP;

	return 0;
}
EXPORT_SYMBOL(iommu_remap_pfn_range);


static struct ion_heap_ops iommu_heap_ops = {
	.allocate = ion_iommu_heap_allocate,
	.free = ion_iommu_heap_free,
	.map_dma = ion_iommu_heap_map_dma,
	.unmap_dma = ion_iommu_heap_unmap_dma,
	.phys = ion_iommu_heap_phys,
	.map_user = ion_iommu_heap_map_user,
	.map_kernel = ion_iommu_heap_map_kernel,
	.unmap_kernel = ion_iommu_heap_unmap_kernel,
};

struct ion_heap *ion_iommu_heap_create(struct ion_platform_heap *heap_data)
{

	struct ion_iommu_heap *iommu_heap = NULL;
	int result = 1;
    pr_err("in %s\n",__func__);

	iommu_heap = kzalloc(sizeof(*iommu_heap), GFP_KERNEL);
	if (!iommu_heap){
		return ERR_PTR(-ENOMEM);
	}
	
	iommu_heap->pool = gen_pool_create(13, -1);//4K ¶ÔÆë
	if (!iommu_heap->pool){
		kfree(iommu_heap);
		return ERR_PTR(-ENOMEM);
	}

    iommu_heap->base = heap_data->base;

    pr_err("ion_pool is base 0x%lx is0x%x\n",heap_data->base,(unsigned int)heap_data->size);
	result = gen_pool_add(iommu_heap->pool, iommu_heap->base, heap_data->size,-1);
	iommu_heap->heap.ops = &iommu_heap_ops;
    iommu_heap->heap.type = heap_data->type;
    gen_pool_alloc(iommu_heap->pool, iommu_context.iommu_run_param->page_size*4);
	return &iommu_heap->heap;
}

void ion_iommu_heap_destroy(struct ion_heap *heap)
{
	struct ion_iommu_heap *iommu_heap = NULL;

	if (heap == NULL){
         return;
	}

	iommu_heap = container_of(heap, struct  ion_iommu_heap, heap);
	gen_pool_destroy(iommu_heap->pool);
	kfree(iommu_heap);
	iommu_heap = NULL;

}

struct tag_iommu_context* iommu_build_params(void)
{
    struct  device_node* np = NULL;
    struct  tag_iommu_context*     ctx = &iommu_context;
    struct  tag_iommu_run_param*   params = NULL;

    memset((void *)ctx,0x0,sizeof(*ctx));
    params = (struct tag_iommu_run_param *)
                             kmalloc(sizeof(*params),GFP_KERNEL);
    if(!params) {
        pr_err("in %s @%d kmalloc iommu_build_params fail",__func__,__LINE__);
        return NULL;
    }
    np = of_find_compatible_node(NULL,NULL,"hisi,iommu-runtime");
    if(np){

        of_property_read_u32_array(np,"hisi,iommu-params",
                    (unsigned int*)params,(sizeof(*params)/sizeof(unsigned int)));
        pr_info("dts"
                "io_domain is %#x MB  "
                "page_size is %u KB  "
                "page_size reg %#x\n",
                params->io_domain>>20,params->page_size>>10,params->reserve);
    }
    else{
        
        params->io_domain = IOMMU_IO_DOMAIN;
        params->page_size = IOMMU_PAGE_SIZE;
        params->reserve   = 0;

        pr_info("no dts "
                "io_domain is %#x MB "
                "page_size is %u KB\n",
                params->io_domain>>20,params->page_size>>10);

    }
    ctx->iommu_run_param = params;
    pr_err("ok in %s ctx 0x%p  params 0x%p\n",__func__,ctx,params);
    return ctx;
}

void iommu_init(struct tag_iommu_context* context)
{    
    iommu_pages_pool_init(context);

    iommu_ip_init(context);
}

EXPORT_SYMBOL(iommu_init);

#if 0
unsigned int of_hisi_get_io_size(int * size)
{
    struct device_node *np = NULL;
    struct tag_iommu_run_param params = {0};

    memset((void *)&params,0x0,sizeof(params));   
    np = of_find_compatible_node(NULL,NULL,"hisi,iommu-runtime");
    if (np){
        pr_err("in of_find_compatible_node \n");
        of_property_read_u32_array(np,"hisi,iommu-params",(unsigned int*)&params,4);
        pr_err("reserve is %x\n",params.reserve);
    }
    else{
        params.io_domain = IOMMU_IO_DOMAIN;
    }
    
    if (size){
        *size = params.io_domain;
        pr_err("in %s ion io_domain is %#x\n",__func__,*size);
    }
    return 0;
}
EXPORT_SYMBOL(of_hisi_get_io_size);

#endif

