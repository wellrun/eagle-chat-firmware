# Path to top level ASF directory relative to this project directory.
PRJ_PATH = $(ASF_PATH)

TARGET = main.elf

CSRCS += \
    main.c \
    randombytes.c \

# Libraries to link with the project
LIBS += \
	nacl

# Extra header paths to include, relative to this directory
USER_INC_PATH += \
	. \