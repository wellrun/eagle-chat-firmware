
TARGET = main.elf

CSRCS += \
    main.c \
	system_timer.c \
	host_rx.c \
	host_tx.c \
	host_msg_fifo.c \
	radio/RFM69.cpp \
	radio/radio.cpp \
	radio/fifo.c \
	routing/packet_fifo.c \
	routing/routing.c \
	crypto/crypto.c \
	crypto/randombytes.c \
	keys/keys.c \
	common/drivers/nvm/xmega/xmega_nvm.c \
	sha204/sha204.c \
	common/components/crypto/sha204/sha204_i2c.c \
    common/components/crypto/sha204/sha204_comm.c \
    common/components/crypto/sha204/sha204_command_marshaling.c \
    common/components/crypto/sha204/xmega_sha204/sha204_timer.c \
    xmega/drivers/twi/twim.c \
    xmega/drivers/tc/tc.c \

INC_PATH += \
    common/services/ioport/xmega \
    common/services/twi \
	xmega/drivers/twi \
	common/components/crypto/sha204 \
	common/components/crypto/sha204/xmega_sha204 \
	common/services/delay \
	common/drivers/nvm/xmega \
	xmega/drivers/nvm \
	common/drivers/nvm \

USER_INC_PTH += \

# Libraries to link with the project
LIBS += \
	nacl


ifdef FORCE_HOPS
CPPFLAGS += \
	-D FORCE_HOPS=$(FORCE_HOPS)
endif
