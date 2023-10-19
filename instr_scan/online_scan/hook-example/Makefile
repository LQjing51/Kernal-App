SHELL := /bin/bash
SRC_DIR := src
TARGET := example


#########################################
### DO NOT MODIFY THE FOLLOWING LINES ###
#########################################

KERNELDIR=/lib/modules/$(shell uname -r)/build
PWD=$(shell pwd)

#Variables expanded in the first make
H_FILES := $(foreach d,${SRC_DIR},$(wildcard ${d}/*.h))
C_FILES := $(foreach d,${SRC_DIR},$(wildcard ${d}/*.c))
EDIT_FILES := $(H_FILES) $(C_FILES)

OS_VERSION = $(shell uname -r)                                                                                                                             
VM_VERSION = 3.13.0-32-generic
C10_VERSION = 3.2.0-24-generic

ifneq (, $(findstring ${VM_VERSION}, ${OS_VERSION}))
  DOV = -D_VM_PLATFORM
else
  ifneq (, $(findstring ${C10_VERSION}, ${OS_VERSION}))
    DOV = -D_C10_PLATFORM
  else
    DOV = -D_UNK_PLATFORM
  endif
endif

#Variables expanded in the second make
OBJ += $(foreach d,$M${SRC_DIR},$(addprefix ${SRC_DIR}/,$(notdir $(patsubst %.c,%.o,$(wildcard ${d}/*.c)))))
OBJ += $(foreach d,$M${SRC_DIR},$(addprefix ${SRC_DIR}/,$(notdir $(patsubst %.S,%.o,$(wildcard ${d}/*.S)))))

obj-m += $(TARGET).o
$(TARGET)-objs += $(OBJ)


all: lines Makefile
	@if \
        make -s -C $(KERNELDIR) M=$(PWD)/ LDDINC=$(PWD)/../include EXTRA_CFLAGS=$(DOV) modules; \
        then echo -e "[\e[34;1mMAKE\e[m] \e[33mCompile loadable kernel module \e[m \e[36m->\e[m \e[32;1m$(TARGET).ko\e[m"; \
        else echo -e "[\e[31mFAIL\e[m] \e[33mCompile loadable kernel module \e[m \e[36m->\e[m \e[32;1m$(TARGET).ko\e[m"; exit -1; fi;
	@echo ${OBJ}

clean:
	@if \
        make -s -C $(KERNELDIR) M=$(PWD)/ clean; \
        then echo -e "[\e[34;1mCLEAN\e[m] \e[33m loadable kernel module \e[m"; \
        else "[\e[31mFAIL\e[m] \e[33mclean loadable kernel module\e[m "; exit -1; fi;

lines:
	@echo -e "[\e[34;1mECHO\e[m] \e[32;1mUntil\e[m \e[37;1m`date`\e[m\e[32;1m, you have already edited\e[m \e[37;1m`cat ${EDIT_FILES} | wc -l`\e[m \e[32;1mlines!\e[m"

