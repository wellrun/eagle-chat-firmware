
TARGET = main.elf

CSRCS += \
    main.c \
	routing.c \
	radio/radio.cpp \
    radio/RFM69.cpp \
    radio/fifo.c \
	packet_fifo.c

INC_PATH += \

USER_INC_PATH += \

ifdef FORCE_HOPS
CPPFLAGS += \
	-D FORCE_HOPS=$(FORCE_HOPS)
endif
