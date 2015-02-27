include ../project.mk

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


# define this variable here to keep from getting out of sync
SERIAL_NUMBER_LENGTH = 12

SERIAL_NUMBER = \
    $(shell head -c 500 /dev/urandom | base64 | tr -dc 'a-f0-9' | head -c $(SERIAL_NUMBER_LENGTH))

$(warning SN=$(SERIAL_NUMBER))

CPPFLAGS += \
    -D USB_SERIAL_NUMBER=\"$(SERIAL_NUMBER)\" \
    -D USB_DEVICE_GET_SERIAL_NAME_LENGTH=$(SERIAL_NUMBER_LENGTH)
