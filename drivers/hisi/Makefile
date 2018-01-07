
obj-$(CONFIG_HISILICON_PLATFORM) += adc/
obj-$(CONFIG_HISILICON_PLATFORM_NVE)	+= drvnve/
obj-$(CONFIG_HISILICON_PLATFORM_HIFI_DSP)	+= hifidsp/
obj-$(CONFIG_SND_HIFI_LPP_HI3XXX)	+= hifidsp_hi3xxx/
obj-$(CONFIG_HISILICON_PLATFORM_HI6XXX)	+= hi6xxx/
obj-$(CONFIG_HISILICON_PLATFORM_DDRC)	+= ddrc/
obj-y += efuse/
#obj-$(CONFIG_HISILICON_PLATFORM_KEYMASTER) += keymaster/
obj-$(CONFIG_HISILICON_PLATFORM_MAINTAIN)	+= mntn/
#obj-$(CONFIG_HISILICON_PLATFORM)					+= secure_boot/

#ifdef OBB_SEPARATE
#ifneq ($(OBB_SEPARATE),SEPARATE)
#obj-$(CONFIG_HISI_BALONG_MODEM_HI6XXX)	+= modem/
#endif
#endif
obj-$(CONFIG_HI6XXX_IPC_MULTICORE)	+= ipc_hi6xxx/
obj-$(CONFIG_HIFI_MAILBOX)					+= hifi_mailbox/
obj-$(CONFIG_HISILICON_PLATFORM)	+= modem_hi6xxx/
obj-$(CONFIG_HISILICON_PLATFORM)	+= mailbox/
obj-$(CONFIG_HISILICON_PLATFORM_MUTEX_SERVICE)	+= mutex_service/
obj-$(CONFIG_HISILICON_PLATFORM_NOC)	+= noc/
#obj-$(CONFIG_HISILICON_PLATFORM_NVIM)	+= nvim/
obj-$(CONFIG_HISILICON_PLATFORM_POWER_CONTROL)	+= pm/
obj-$(CONFIG_HISI_RDR) += rdr/
obj-$(CONFIG_HISI_AXI)+= axi/
obj-$(CONFIG_HISI_SOCP) += socp/
obj-$(CONFIG_HISILICON_PLATFORM_SUBVIC) += subvic/
obj-$(CONFIG_HISILICON_PLATFORM_SYNC)	+= sync/
obj-$(CONFIG_HISILICON_PLATFORM_TELE_MNTN) += tele_mntn/
obj-y += mcushell/
obj-$(CONFIG_HISILICON_PLATFORM_UDI)	+= udi/
obj-$(CONFIG_HISILICON_PLATFORM_USB)	+= usb/
obj-$(CONFIG_HISILICON_PLATFORM)	+= hi3xxx/
#obj-$(CONFIG_CONNECTIVITY_HI110X) += hi110x/
obj-$(CONFIG_HI6421_SPK) += hi6421_spk/
obj-$(CONFIG_HI6402_MBHC) += hi6402_mbhc/
obj-$(CONFIG_HI6402_HIFI_MISC) += hi6402dsp/
obj-$(CONFIG_SUSPEND) += pm_common/

ifneq ($(strip $(CFG_HISI_OPENSOURCE)), true)
obj-$(CONFIG_HISILICON_PLATFORM_KEYMASTER) += keymaster/
obj-$(CONFIG_HISILICON_PLATFORM)                                       += secure_boot/
ifdef OBB_SEPARATE
ifneq ($(OBB_SEPARATE),SEPARATE)
obj-$(CONFIG_HISI_BALONG_MODEM_HI6XXX) += modem/
endif
endif
obj-$(CONFIG_HISILICON_PLATFORM_NVIM)  += nvim/
endif
obj-$(CONFIG_HISI_BALONG_MODEM_HI6XXX) += modem/
