
TARGET = main.elf

CSRCS += \
    main.c \
	xmega/drivers/tc/tc.c \
	system_timer.c \
	host_rx.c \
	host_tx.c \
	host_msg_fifo.c \
	radio/RFM69.cpp \
	radio/radio.cpp \
	radio/fifo.c \
	routing/packet_fifo.c \
	routing/routing.c \

INC_PATH += \
    common/services/ioport/xmega \

USER_INC_PTH += \
