# EXTRA_CFLAGS += -wformat

obj-$(CONFIG_HISILICON_PLATFORM_POWER_CONTROL)	+= hi6xxx-power-contorl.o

hi6xxx-power-contorl-y	:= hi6xxx-power-common.o hi6xxx-power-mca.o hi6xxx-power-dfs.o hi6xxx-power-debug.o
hi6xxx-power-contorl-y	+= hi6xxx-core-pm.o hi6xxx-hotplug.o hi6xxx-pm.o hi6xxx-pm-asm.o pwrctrl_power_state_manger.o

# EXTRA_CFLAGS -= -Wformat