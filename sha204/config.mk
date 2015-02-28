
TARGET = main.elf

CSRCS += \
    main.c \
    board.c \
    common/components/crypto/sha204/sha204_i2c.c \
    common/components/crypto/sha204/sha204_comm.c \
    common/components/crypto/sha204/sha204_command_marshaling.c \
    common/components/crypto/sha204/xmega_sha204/sha204_timer.c \
    xmega/drivers/twi/twim.c \
    xmega/drivers/tc/tc.c \


USER_INC_PATH += \
	
INC_PATH += \
	common/services/twi \
	xmega/drivers/twi \
	common/components/crypto/sha204 \
	common/components/crypto/sha204/xmega_sha204 \
