#ifndef __HI6xxx_NOC_BUS_H
#define __HI6xxx_NOC_BUS_H

#define SYSNOC_INITFLOW_ARRAY_SIZE		15
#define SYSNOC_TARGETFLOW_ARRAY_SIZE	15
#define MEDIA_INITFLOW_ARRAY_SIZE		7
#define MEDIA_TARGETFLOW_ARRAY_SIZE		12

#define MAX_NOC_BUSES_NR	2

/* keep target route id, initiator flow id etc*/
struct noc_bus_info {
	char *name;
	unsigned int initflow_mask;
	unsigned int initflow_shift;

	unsigned int targetflow_mask;
	unsigned int targetflow_shift;

	unsigned int targ_subrange_mask;
	unsigned int seq_id_mask;

	char **initflow_array;
	unsigned int initflow_array_size;

	char **targetflow_array;
	unsigned int targetflow_array_size;
};

extern char *sysnoc_initflow_array[SYSNOC_INITFLOW_ARRAY_SIZE];
extern char *sysnoc_targetflow_array[SYSNOC_TARGETFLOW_ARRAY_SIZE];
extern char *media_initflow_array[MEDIA_INITFLOW_ARRAY_SIZE];
extern char *media_targetflow_array[MEDIA_TARGETFLOW_ARRAY_SIZE];
extern struct noc_bus_info noc_buses_info[MAX_NOC_BUSES_NR];
#endif
