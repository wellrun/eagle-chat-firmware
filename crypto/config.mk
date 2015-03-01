
TARGET = main.elf

CSRCS += \
    main.c \
    randombytes.c \
	crypto.c \

# Libraries to link with the project
LIBS += \
	nacl

# Extra header paths to include, relative to this directory
USER_INC_PATH += \

USER_LIB_PATH += \
	# add extra static libraries here
