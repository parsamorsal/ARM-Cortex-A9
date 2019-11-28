
export ARCH:=arm
export CROSS_COMPILE:=arm-xilinx-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc

obj-m += main.o

KDIR  := ~/Desktop/kernel/706/linux-xlnx-master/
PWD		:= $(shell pwd)

default:
	${MAKE} -C ${KDIR} M=${PWD} modules

clean:
	${MAKE} -C ${KDIR} M=${PWD} clean
