###
# This makefile include sets basic parameters to let the build system know about your local environment.
#

include ../conf.mk

# Path to top level ASF directory relative to this project directory.
PRJ_PATH = $(ASF_PATH)

# Microcontroller: atxmega128a1, atmega128, attiny261, etc.
MCU = atxmega192a3u

BUILD_DIR = build

CWD = $(abspath ..)

# define this variable here to keep from getting out of sync
SERIAL_NUMBER_LENGTH = 12

SERIAL_NUMBER = \
    $(shell head -c 500 /dev/urandom | base64 | tr -dc 'a-f0-9' | head -c $(SERIAL_NUMBER_LENGTH))

CPPFLAGS = \
    -D USB_SERIAL_NUMBER=\"$(SERIAL_NUMBER)\" \
    -D USB_DEVICE_GET_SERIAL_NAME_LENGTH=$(SERIAL_NUMBER_LENGTH)

USER_INC_PATH = \
    config \
    cdc \

CPPFLAGS += $(foreach INC,$(addprefix $(CWD)/,$(USER_INC_PATH)),-I$(INC))

# C source files located from the top-level source directory
CSRCS = \
	common/services/clock/xmega/sysclk.c \
	xmega/drivers/nvm/nvm.c \
	common/services/sleepmgr/xmega/sleepmgr.c \
    cdc/cdc.c \
    xmega/drivers/usb/usb_device.c \
    common/services/usb/udc/udc.c \
    common/services/usb/class/cdc/device/udi_cdc.c \
    common/services/usb/class/cdc/device/udi_cdc_desc.c \
    common/services/spi/xmega_spi/spi_master.c \
	common/services/ioport/xmega/ioport_compat.c\
	xmega/drivers/spi/spi.c


 # common/components/display/st7565r/st7565r.c \
 # common/services/calendar/calendar.c \
 # common/services/gfx_mono/gfx_mono_c12832_a1z.c \
 # common/services/gfx_mono/gfx_mono_framebuffer.c \
 # common/services/gfx_mono/gfx_mono_generic.c \
 # common/services/gfx_mono/gfx_mono_menu.c \
 # common/services/gfx_mono/gfx_mono_spinctrl.c \
 # common/services/gfx_mono/gfx_mono_text.c \
 # common/services/gfx_mono/sysfont.c \
 # common/services/ioport/xmega/ioport_compat.c \
 # common/services/spi/xmega_spi/spi_master.c \
 # common/services/spi/xmega_usart_spi/usart_spi.c \
 
 # xmega/applications/xmega_a3bu_xplained_demo/adc_sensors.c \
 # xmega/applications/xmega_a3bu_xplained_demo/bitmaps.c \
 # xmega/applications/xmega_a3bu_xplained_demo/cdc.c \
 # xmega/applications/xmega_a3bu_xplained_demo/date_time.c \
 # xmega/applications/xmega_a3bu_xplained_demo/keyboard.c \
 # xmega/applications/xmega_a3bu_xplained_demo/lightsensor.c \
 # xmega/applications/xmega_a3bu_xplained_demo/main.c \
 # xmega/applications/xmega_a3bu_xplained_demo/ntc_sensor.c \
 # xmega/applications/xmega_a3bu_xplained_demo/production_date.c \
 # xmega/applications/xmega_a3bu_xplained_demo/qtouch/touch.c \
 # xmega/applications/xmega_a3bu_xplained_demo/timezone.c \
 # xmega/boards/xmega_a3bu_xplained/init.c \
 # xmega/drivers/adc/adc.c \
 # xmega/drivers/adc/xmega_aau/adc_aau.c \
 # xmega/drivers/rtc32/rtc32.c \
 # xmega/drivers/tc/tc.c \
 # xmega/drivers/usart/usart.c \

# Assembler source files located from the top-level source directory
ASSRCS = \
	xmega/drivers/cpu/ccp.s \
	xmega/drivers/nvm/nvm_asm.s
# xmega/applications/xmega_a3bu_xplained_demo/qtouch/qt_asm_xmega.s \

# Include path located from the top-level source directory
INC_PATH = \
	common/boards \
	common/utils \
	xmega/boards \
	common/services/clock \
	xmega/drivers/cpu \
	xmega/utils \
	xmega/utils/preprocessor \
	xmega/drivers/nvm \
	xmega/drivers/usb \
	common/services/usb \
	common/services/gpio \
	common/services/ioport \
	xmega/drivers/pmic \
	xmega/drivers/tc \
	common/services/sleepmgr \
	xmega/drivers/sleep \
	xmega/drivers/usart \
	common/services/usb/class/cdc \
	common/services/usb/class/cdc/device \
	common/services/usb/udc \
	common/services/spi \
	xmega/drivers/spi 

# common/components/display/st7565r \
# common/services/calendar \
# common/services/delay \
# common/services/gfx_mono \
# xmega/applications/xmega_a3bu_xplained_demo \
# xmega/applications/xmega_a3bu_xplained_demo/atxmega256a3bu_xmega_a3bu_xplained \
# xmega/applications/xmega_a3bu_xplained_demo/qtouch \
# xmega/boards/xmega_a3bu_xplained \
# xmega/drivers/adc \
# xmega/applications/xmega_a3bu_xplained_demo/atxmega256a3bu_xmega_a3bu_xplained/gcc

# Library paths from the top-level source directory
LIB_PATH = \
	
# xmega/applications/xmega_a3bu_xplained_demo/qtouch

# Libraries to link with the project
LIBS = \
	m \
# avrxmega6g1-4qt-k-0rs \
	 

# Additional options for debugging. By default the common Makefile.in will
# add -gdwarf-2.
DBGFLAGS = 

# Optimization settings
OPTIMIZATION = -Os

# Extra flags used when creating an EEPROM Intel HEX file. By default the
# common Makefile.in will add -j .eeprom
# --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0.
EEPROMFLAGS = 

# Extra flags used when creating an Intel HEX file. By default the common
# Makefile.in will add -R .eeprom -R .usb_descriptor_table.
FLASHFLAGS = 

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS = 

# Extra flags to use when compiling.
#CFLAGS = -I../config/ -I.
#CFLAGS += $(foreach user_inc_path,$(USER_INC_PATH),-I$(user_inc_path))

#$(warning CFLAGS=$(CFLAGS))

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
# To add a definition use the format "-D name[=definition]".
# To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
# BOARD Target board in use, see boards/board.h for a list.
# EXT_BOARD Optional extension board in use, see boards/board.h for a list.
CPPFLAGS += \
	-D BOARD=USER_BOARD	 \
	-D IOPORT_XMEGA_COMPAT \

# -D CONFIG_NVM_IGNORE_XMEGA_A3_D3_REVB_ERRATA \
# -D GFX_MONO_C12832_A1Z=1 \
# -D NUMBER_OF_PORTS=1 \
# -D QTOUCH_STUDIO_MASKS=1 \
# -D QT_DELAY_CYCLES=1 \
# -D QT_NUM_CHANNELS=4 \
# -D SNS=F \
# -D SNSK=F \
# -D _QTOUCH_ \
# -D _SNS1_SNSK1_SAME_PORT_


# Use this to add library search paths relative to the current working directory
USER_LIB_PATH := \
    libs

LDFLAGS += \
	$(foreach INC,$(addprefix $(CWD)/,$(USER_LIB_PATH)),-L$(INC)) 

# Extra flags to use when linking
LDFLAGS += \
	-Wl,--section-start=.BOOT=0x30000 

# Pre- and post-build commands
PREBUILD_CMD = 
POSTBUILD_CMD = 
