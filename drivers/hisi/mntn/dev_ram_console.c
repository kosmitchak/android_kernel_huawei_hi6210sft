/*
 *  linux/arch/arm/mach-k3v2/dev_ram_console.c
 *
 * balong ram console platform device declaration
 *
 * Copyright (C) 2012 Hisilicon, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/pstore_ram.h>
#include "soc_peri_sctrl_interface.h"
#include <soc_ao_sctrl_interface.h>
#include <linux/vmalloc.h>
#include "excDrv.h"

static struct ramoops_platform_data ramoops_data;
/* ram console platform device definition*/
static struct platform_device plat_ram_console_dev = {
	.name = "ramoops",
	.id = 0,
	.dev = {
		.platform_data = &ramoops_data,
	},
};

static int __initdata ram_console_disabled = 0; /*1 : disabled , 0 : enabled*/
static int __init ram_console_disable(char *__unused)
{
	ram_console_disabled = 1;

	return 0;
}
early_param("no_ram_console", ram_console_disable);

/*****************************************************************************
 Description : ram console platform device register
  1.Date: 2012/9/13
    Author : x00138766
    Modification : Created function
*****************************************************************************/
static int __init plat_ram_console_dev_init(void)
{
    struct dump_log *dump = NULL;

    if (1 == ram_console_disabled)
        return 0;

    dump = (struct dump_log *)ANDROID_DUMP_LOG_ADDR;

    ramoops_data.mem_address = (ulong)&dump->log_kmsg[0];
    ramoops_data.mem_size = EXCH_LOG_KMSG_SIZE;
    ramoops_data.console_size = (EXCH_LOG_KMSG_SIZE/8)*5;
    ramoops_data.record_size = EXCH_LOG_KMSG_SIZE/8;
    ramoops_data.ftrace_size = (EXCH_LOG_KMSG_SIZE/8)*0;
    ramoops_data.dump_oops = 1;
	
    return platform_device_register(&plat_ram_console_dev);
};
arch_initcall(plat_ram_console_dev_init);

//MODULE_DESCRIPTION("Hisilicon Balong Platform Ram Console Device Declaration");
//MODULE_LICENSE("GPL");
//MODULE_AUTHOR("chenlibo <libo.chen@huawei.com>");
