#Copyright Huawei Technologies Co., Ltd. 1998-2011. All rights reserved.
#This file is Auto Generated 

dtb-y += hi6210sft/CHERRY_PLUS_HI1101_TL00_config.dtb
dtb-y += hi6210sft/hi6210sft_udp_ddr3_config.dtb
dtb-y += hi6210sft/hi6210sft_udp_ddr2_config.dtb
dtb-y += hi6210sft/hi6210sft_sft_config.dtb
dtb-y += hi6210sft/HUAWEI_G760S_TL00_VA_config.dtb
dtb-y += hi6210sft/HUAWEI_G760S_UL00_VA_config.dtb
dtb-y += hi6210sft/CHERRY_PLUS_TL00_VA_config.dtb
dtb-y += hi6210sft/CHERRY_PLUS_UL00_VA_config.dtb

targets += hi6210sft_dtb
targets += $(dtb-y)

# *.dtb used to be generated in the directory above. Clean out the
# old build results so people don't accidentally use them.
hi6210sft_dtb: $(addprefix $(obj)/, $(dtb-y))
	$(Q)rm -f $(obj)/../*.dtb

clean-files := *.dtb

#end of file
