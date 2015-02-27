include ../project.mk

# Path to top level ASF directory relative to this project directory.
PRJ_PATH = $(ASF_PATH)

TARGET = main.elf

CSRCS += \
	board.c \
	sram.c \
    main.c \

USER_INC_PATH += \
