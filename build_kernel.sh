#!/bin/bash

LOCAL_DIR=`pwd`
export PATH=$PATH:$LOCAL_DIR/prebuilts/gcc/linux-x86/arm/gcc-linaro-arm-linux-gnueabihf-4.7/bin
export CROSS_COMPILE=arm-linux-gnueabihf-
mkdir ../out
make O=../out ARCH=arm hisi_hi6210sft_defconfig
make O=../out ARCH=arm zImage

