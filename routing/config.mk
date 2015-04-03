
TARGET = main.elf

CSRCS += \
    main.c \
	routing.c \

INC_PATH += \
    common/services/ioport/xmega \
    xmega/drivers/crc/

USER_INC_PATH += \
