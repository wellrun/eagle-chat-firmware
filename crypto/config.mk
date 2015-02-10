# Path to top level ASF directory relative to this project directory.
PRJ_PATH = $(ASF_PATH)

TARGET = main.elf

CSRCS += \
        main.c

# Libraries to link with the project
LIBS += \
	nacl
