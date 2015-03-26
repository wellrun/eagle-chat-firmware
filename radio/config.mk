
TARGET = main.elf

CSRCS += \
    main.cpp \
    radio.cpp \
    RFM69.cpp \
    fifo.c \

INC_PATH += \
    common/services/ioport/xmega \

USER_INC_PTH += \
