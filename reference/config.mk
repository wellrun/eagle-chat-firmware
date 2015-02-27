# Path to top level ASF directory relative to this project directory.
PRJ_PATH = $(ASF_PATH)

TARGET = main.elf

CSRCS += \
    main.c \
    xmega/drivers/usb/usb_device.c \
    common/services/usb/udc/udc.c \
    common/services/usb/class/cdc/device/udi_cdc.c \
    common/services/usb/class/cdc/device/udi_cdc_desc.c \
    usb/cdc.c

USER_INC_PATH += \
	usb \

CWD = $(shell greadlink -f ..)

USER_INC_PATH = \
    config \
    usb

CPPFLAGS += $(foreach INC,$(addprefix $(CWD)/,$(USER_INC_PATH)),-I$(INC))
