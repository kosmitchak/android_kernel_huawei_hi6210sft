/*
 * axierr irq support.
 *
 * Copyright (c) 2013- Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/cpu.h>
#include <linux/err.h>

#define AFFINITY_A53_cluster0		(0x1 << 0)
#define AFFINITY_A53_cluster1		(0x1 << 4)
#define IRQNR_PER_BYTE		(4)
#define SHIFT_IRQNR_PER_BYTE		(2)
#define SHIFT_IRQAFFINITYNR_PER_BYTE	(3)

static irqreturn_t irq_handler_axierr_A53(int irq, void * dev_id)
{
	u64 axi_err_address;
	/*  clear irq */
	asm volatile(
			"	mov x0, #0x0	\n"
			"	msr S3_1_C11_C0_3, x0	\n"
	);

	asm volatile(
		"mrs %0, FAR_EL3 \n"
		: "=r" (axi_err_address)
	);
	pr_err("axi error address is 0x%x!\n", axi_err_address);

	return IRQ_HANDLED;
}

static int hisi_irqaffinity_register(unsigned int irq, unsigned int cpu_affinity)
{
	void __iomem *hisi_gic_base;
	struct device_node *np;
	u32 target_base;
	u32 target_off;

	np = of_find_compatible_node(NULL, NULL, "arm,cortex-a9-gic");
	if (IS_ERR(np)) {
		pr_err("hisi_irqaffinity_register: of_find_compatible_node failed!\n");
		return -ENODEV;
	}

	hisi_gic_base = of_iomap(np, 0);
	if (!hisi_gic_base) {
		pr_err("hisi_irqaffinity_register: of_iomap failed!\n");
		return -ENOMEM;
	}

	target_base = irq >> SHIFT_IRQNR_PER_BYTE;
	target_off = (irq % IRQNR_PER_BYTE) << SHIFT_IRQAFFINITYNR_PER_BYTE;
	writel(cpu_affinity << target_off,  hisi_gic_base + GIC_DIST_TARGET
			+ (target_base << SHIFT_IRQNR_PER_BYTE));

	iounmap(hisi_gic_base);

	return 0;
}

static int __init hisi_axi_init(void)
{
	u32 ret = 0;
	u32 irq_num;
	u32 hifi_addr;
	struct device_node *node;

	pr_err("axi init begin\n");
	node = of_find_compatible_node(NULL, NULL, "hisilicon,axierr");
	if (IS_ERR(node)) {
		pr_err("Can not find axierr node\n");
		goto err;
	}

	irq_num = irq_of_parse_and_map(node, 0);
	ret = request_irq(irq_num, irq_handler_axierr_A53,
			IRQF_TRIGGER_RISING, "axierr_A53_cluster0", NULL);
	if (ret) {
		pr_err("%s: cannot register IRQ %d for A53. Line: %d\n",
				__func__, irq_num, __LINE__);
		goto err;
	}

	ret = hisi_irqaffinity_register(irq_num, AFFINITY_A53_cluster0);
	if (ret < 0) {
		pr_err("hisi axi init failed: A53 irqaffinity register failed\n");
		goto err_a53_cluster0_irqaffinity_register;
	}

	irq_num = irq_of_parse_and_map(node, 1);
	ret = request_irq(irq_num, irq_handler_axierr_A53,
			IRQF_TRIGGER_RISING, "axierr_A53_cluster1",NULL);
	if (ret) {
		pr_err("%s: cannot register IRQ %d for A53. Line: %d\n",
				__func__, irq_num, __LINE__);
		goto err_a53_cluster0_irqaffinity_register;
	}

	ret = hisi_irqaffinity_register(irq_num, AFFINITY_A53_cluster1);
	if (ret < 0) {
		pr_err("hisi axi init failed: A15 irqaffinity register failed\n");
		goto err_a53_cluster1_irqaffinity_register;
	}
	pr_info("hisi axi init finished.\n");
	return ret;

err_a53_cluster1_irqaffinity_register:
	irq_num = irq_of_parse_and_map(node, 1);
	free_irq(irq_num, NULL);

err_a53_cluster0_irqaffinity_register:
	irq_num = irq_of_parse_and_map(node, 0);
	free_irq(irq_num, NULL);

err:
	pr_err("hisi axi init failed!\n");
	return ret;
}

early_initcall(hisi_axi_init);
