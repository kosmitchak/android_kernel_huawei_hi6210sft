/* noc buses info */
#include "hi6xxx_noc_bus.h"

char *sysnoc_initflow_array[SYSNOC_INITFLOW_ARRAY_SIZE] = {
	"noc_acpu",
	"noc_cssys",
	"noc_dap",
	"noc_dap_apb",
	"noc_dmac",
	"noc_hifi",
	"noc_ipf",
	"noc_mcu",
	"noc_mmc0",
	"noc_mmc1",
	"noc_mmc2",
	"noc_modem",
	"noc_secend",
	"noc_socp",
	"noc_usb"
};

char *sysnoc_targetflow_array[SYSNOC_TARGETFLOW_ARRAY_SIZE] = {
	"cfg_noc_target",
	"cssys_apb_target",
	"ddrc_target",
	"dmac_s_target",
	"hifi_s_target",
	"ipf_s_target",
	"mcu_s_target",
	"modem_target",
	"secend_s_target",
	"service_target",
	"service_ao_target",
	"service_dma_target",
	"socp_s_target",
	"tzma_target",
	"xg2ram0_target"
};


char *media_initflow_array[MEDIA_INITFLOW_ARRAY_SIZE] = {
	"noc_ade0",
	"noc_ade1",
	"noc_cfg",
	"noc_isp",
	"noc_jpu",
	"noc_vpu",
	"noc_vpu_sec"
};

char *media_targetflow_array[MEDIA_TARGETFLOW_ARRAY_SIZE] = {
	"apb_video_s_jsp_target",
	"apb_video_s_jpu_target",
	"apb_video_s_vpu_target",
	"ahb_isp_s_target",
	"apb_ade_s_target",
	"apb_g3d_s_target",
	"apb_media_sc_target",
	"apb_smmu_s_target",
	"axi_smmu_s_target",
	"axi_xg2ram_harq_target",
	"service_target",
	"service_video_target"
};

struct noc_bus_info noc_buses_info[MAX_NOC_BUSES_NR] = {
	[0] = {
		.name = "sys_noc_bus",
		.initflow_mask = ((0xf) << 14),
		.targetflow_mask = ((0xf) << 10),
		.targ_subrange_mask = 0,
		.seq_id_mask = 0,
		.initflow_array = sysnoc_initflow_array,
		.initflow_array_size = SYSNOC_INITFLOW_ARRAY_SIZE,
		.targetflow_array = sysnoc_targetflow_array,
		.targetflow_array_size = SYSNOC_TARGETFLOW_ARRAY_SIZE,
	},
	[1] = {
		.name = "media_noc_bus",
		.initflow_mask = ((0x7) << 14),
		.targetflow_mask = ((0xf) << 10),
		.targ_subrange_mask = 0,
		.seq_id_mask = 0 ,
		.initflow_array = media_initflow_array,
		.initflow_array_size = MEDIA_INITFLOW_ARRAY_SIZE,
		.targetflow_array = media_targetflow_array,
		.targetflow_array_size = MEDIA_TARGETFLOW_ARRAY_SIZE,
	}
};
