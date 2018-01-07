/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 *  fastbootlog.c    hisi fastbootlog module
 *                   record fastbootlog into filesystem when booting kernel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/stat.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/hisi/util.h>					/* For mach_call_usermoduleshell*/
#include <linux/uaccess.h>				/* For copy_to_user*/
#include <linux/vmalloc.h>				/* For vmalloc*/
#include "global_ddr_map.h"

static void *fastboot_log_buff = NULL;
static size_t fastboot_log_size = 0;

/*read dump file content*/
static ssize_t fastbootlog_dump_file_read(struct file *file, char __user *userbuf, size_t bytes, loff_t *off)
{
	ssize_t copy;

	if (*off > fastboot_log_size){
		printk(KERN_ERR"read offset error 1\n");
		return 0;
	}

	if (*off == fastboot_log_size){
		/*end of file*/
		return 0;
	}

	copy = (ssize_t)min(bytes, (size_t)(fastboot_log_size - *off));

	if (copy_to_user(userbuf, fastboot_log_buff + *off, copy)){
		printk(KERN_ERR"%s copy to user error \n", __FUNCTION__);
		copy = -EFAULT;
		goto copy_err;
	}

	*off += copy;

copy_err:
	return copy;
}

static const struct file_operations fastbootlog_dump_file_fops = {
	.read = fastbootlog_dump_file_read,
};

/*****************************************************************************
Description : fastbootlog init
check emmc leaves log to record
if there is log, launch work queue, and create /proc/balong/fastbootlog
History
1.Date: 2012/9/13
Author : x00138766
Modification : Created function
 *****************************************************************************/
static int __init fastbootlog_dump_init(void)
{
	void *fastboot_buff = phys_to_virt(FASTBOOT_DUMP_LOG_ADDR);

	fastboot_log_buff = (void *)vmalloc(FASTBOOT_DUMP_LOG_SIZE);
	if(!fastboot_log_buff){
		printk(KERN_ERR"%s fail to vmalloc %d bytes \n", __FUNCTION__, FASTBOOT_DUMP_LOG_SIZE);
		return -1;
	}

	memcpy(fastboot_log_buff, fastboot_buff, FASTBOOT_DUMP_LOG_SIZE);
	fastboot_log_size = FASTBOOT_DUMP_LOG_SIZE;

	balong_create_memory_proc_entry("fastboot", S_IRUGO, &fastbootlog_dump_file_fops, NULL);

	return 0;
}
module_init(fastbootlog_dump_init);

/*****************************************************************************
Description : fastbootlog exit
destroy the workqueue
History
1.Date: 2012/9/13
Author : x00138766
Modification : Created function
 *****************************************************************************/
static void __exit fastbootlog_dump_exit(void)
{
	balong_remove_log_proc_entry("fastboot");

	if (fastboot_log_buff){
		vfree(fastboot_log_buff);
		fastboot_log_buff = NULL;
	}
	fastboot_log_size = 0;

	return;
}
module_exit(fastbootlog_dump_exit);


