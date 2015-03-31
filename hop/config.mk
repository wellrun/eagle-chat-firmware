
TARGET = main.elf

CSRCS += \
    main.c \
    radio/radio.cpp \
    radio/RFM69.cpp \
    radio/fifo.c \
    xmega/drivers/crc/crc.c

INC_PATH += \
    common/services/ioport/xmega \
    xmega/drivers/crc/

USER_INC_PATH += \
	radio 
