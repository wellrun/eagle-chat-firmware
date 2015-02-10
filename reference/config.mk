# Path to top level ASF directory relative to this project directory.
PRJ_PATH = $(ASF_PATH)

TARGET = main.elf

CSRCS += \
    main.c \
    xmega/drivers/usb/usb_device.c \
	common/services/usb/udc/udc.c \
	common/services/usb/class/cdc/device/udi_cdc.c \
 	common/services/usb/class/cdc/device/udi_cdc_desc.c \
	cdc.c \
