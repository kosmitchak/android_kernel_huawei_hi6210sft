#ifndef __HI6xxx_NOC_H
#define __HI6xxx_NOC_H


#define MODULE_NAME	"hi6xxx_noc"

#define MAX_NOC_NODE_NAME_LEN	20

#ifndef  DEFAULT_ENABLE_NOC_PROBE
#define DEFAULT_ENABLE_NOC_PROBE
#endif

/* NoC IRQs */
#define NOC_MAX_IRQ_NR   4

#define MAX_NODES_NR    4

#define TIMER_PERIOD_MAX 300000
#define TIMES_MAX 10000

#define SYS_ERR_INT      74
#define MEDIA_ERR_INT    75
#define SYS_PACKET_INT   81
#define MEDIA_PACKET_INT 83

enum NOC_IRQ_TPYE {
	NOC_ERR_PROBE_IRQ,
	NOC_PACKET_PROBE_IRQ
};

struct hisi_noc_irqd {
	enum NOC_IRQ_TPYE type;
	struct noc_node *node;
};

struct hisi_noc_device {
	struct device		*dev;
	void __iomem		*sys_base;
	void __iomem        *media_base;
	unsigned int		irq[4];
};


#ifdef CONFIG_HI6XXX_NOC_ERR_PROBE

struct	packet_configration {
	unsigned int		statperiod;
	unsigned int		statalarmmax;

	unsigned int		packet_counters_0_src;
	unsigned int		packet_counters_1_src;
	unsigned int        packet_counters_2_src;
	unsigned int        packet_counters_3_src;

	unsigned int		packet_counters_0_alarmmode;
	unsigned int		packet_counters_1_alarmmode;
    unsigned int		packet_counters_2_alarmmode;
    unsigned int		packet_counters_3_alarmmode;

	unsigned int		packet_filterlut;
	unsigned int		packet_f0_routeidbase;
	unsigned int		packet_f0_routeidmask;
	unsigned int		packet_f0_addrbase;
	unsigned int		packet_f0_windowsize;
	unsigned int		packet_f0_securitymask;
	unsigned int		packet_f0_opcode;
	unsigned int		packet_f0_status;
	unsigned int		packet_f0_length;
	unsigned int		packet_f0_urgency;
	unsigned int		packet_f0_usermask;

	unsigned int		packet_f1_routeidbase;
	unsigned int		packet_f1_routeidmask;
	unsigned int		packet_f1_addrbase;
	unsigned int		packet_f1_windowsize;
	unsigned int		packet_f1_securitymask;
	unsigned int		packet_f1_opcode;
	unsigned int		packet_f1_status;
	unsigned int		packet_f1_length;
	unsigned int		packet_f1_urgency;
	unsigned int		packet_f1_usermask;

};

#endif

struct noc_node {
	char			*name;
	void __iomem		*base;
	unsigned int		bus_id;
	unsigned int		interrupt_num;

	/* err probe */
#ifdef CONFIG_HI6XXX_NOC_ERR_PROBE
	unsigned int		eprobe_offset;
	bool			eprobe_autoenable;
	int			eprobe_hwirq;

	struct	packet_configration		packet_cfg;
#endif
};

struct noc_statstic_data {
    u64 time;
    u32 counter3;
    u32 counter2;
    u32 counter1;
    u32 counter0;
};
#ifdef CONFIG_HI6XXX_NOC_ERR_PROBE
void __iomem * get_errprobe_base(const char *name);
struct noc_node * get_probe_node(const char *name);
#endif

extern void __iomem *error_logger;
extern void __iomem *error_logger_media;
#endif
