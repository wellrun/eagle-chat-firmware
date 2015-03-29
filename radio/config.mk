
TARGET = main.elf

CSRCS += \
    main.c \
    radio.cpp \
    RFM69.cpp \
    fifo.c \
    xmega/drivers/crc/crc.c

INC_PATH += \
    common/services/ioport/xmega \
    xmega/drivers/crc/

USER_INC_PTH += \
