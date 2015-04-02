
TARGET = main.elf

CSRCS += \
    main.c \
	xmega/drivers/tc/tc.c \
	system_timer.c

INC_PATH += \
    common/services/ioport/xmega \

USER_INC_PTH += \
