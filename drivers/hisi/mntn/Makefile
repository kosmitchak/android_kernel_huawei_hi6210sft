ccflags-y += -I$(srctree)/arch/arm/mach-hi6210-legacy/include
ccflags-y += -I$(srctree)/arch/arm/include/
ccflags-y += -I$(srctree)/drivers/hisi/secure_boot/inc
ccflags-y += -I$(srctree)/drivers/hisi/modem_hi6xxx/rfile

obj-$(CONFIG_HISILICON_PLATFORM_MAINTAIN)	+= hisilicon_platform_mntn.o
ifneq ($(strip $(CFG_HISI_OPENSOURCE)), true)
obj-y += reset_secure.o
endif

ifndef CONFIG_ARM64

hisilicon_platform_mntn-y := \
	excDrv.o icc_mntn.o dump_info.o dev_ram_console.o queue.o util.o dump.o mntn_stub.o debug_func.o \
	mntn.o reset.o reset_sub.o reset_ccoreloader.o reset_hifiloader.o watchpoint_acore.o \
	watchpoint_common.o watchpoint_test.o hisi_dmesg.o fastbootlog.o

ifeq ($(strip $(CONFIG_ARCH_HI6XXX)), y)
ifeq ($(strip $(CONFIG_DEBUG_FS)), y)
hisilicon_platform_mntn-y += vstool_if.o
endif
endif

else
hisilicon_platform_mntn-y := \
	excDrv.o icc_mntn.o dump_info.o dev_ram_console.o queue.o util.o dump.o mntn_stub.o debug_func.o \
	mntn.o reset.o reset_sub.o reset_ccoreloader.o reset_hifiloader.o 
endif
