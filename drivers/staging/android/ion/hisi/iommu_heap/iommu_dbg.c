#define pr_fmt(fmt) "ION IOMMU DBG" fmt

#include <linux/spinlock.h>
#include <linux/err.h>
#include <linux/genalloc.h>
#include <linux/io.h>
#include <linux/ion.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include "../../ion_priv.h"
#include <linux/device.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/anon_inodes.h>
#include <linux/ion.h>
#include <linux/list.h>
#include <linux/memblock.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/rbtree.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/dma-buf.h>
#include <linux/mutex.h>
#include <linux/ion_iommu_heap.h>

extern struct tag_iommu_context iommu_context;

static int iommu_dbg_show(struct seq_file *s, void *unused)
{
#if 0
	struct ion_client *client = (struct ion_client *)s->private;
	struct rb_node *n;
	size_t sizes[ION_NUM_HEAPS] = {0};
	const char *names[ION_NUM_HEAPS] = {0};
	int i;


	mutex_lock(&client->lock);
	for (n = rb_first(&client->handles); n; n = rb_next(n))
	{
		struct ion_handle *handle = rb_entry(n, struct ion_handle,
						     node);
		enum ion_heap_type type = handle->buffer->heap->type;

		if (!names[type])
			names[type] = handle->buffer->heap->name;
		sizes[type] += handle->buffer->size;
	}

	mutex_unlock(&client->lock);
	seq_printf(s, "%16.16s: %16.16s\n", "heap_name", "size_in_bytes");

	for (i = 0; i < ION_NUM_HEAPS; i++)

	{
		if (!names[i])

			continue;

		seq_printf(s, "%16.16s: %16u\n", names[i], sizes[i]);
	}
#endif
	return 0;
}

static int iommu_dbg_mem_node_open(struct inode *inode, struct file *file)
{
	return single_open(file, iommu_dbg_show, inode->i_private);
}

static const struct file_operations dbg_mem_node_fops = {
	.open = iommu_dbg_mem_node_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

void iommu_creat_debugfs_file(struct  tag_mem_node_info* mem_node)
{

    struct  tag_mem_node_info* p_mem_node = NULL;

    if (!mem_node){
        return;
    }
    p_mem_node = mem_node;
	p_mem_node->debug_root = debugfs_create_file(p_mem_node->name, 0664,
						     iommu_context.iommu_dbg_root, p_mem_node,&dbg_mem_node_fops);
}
