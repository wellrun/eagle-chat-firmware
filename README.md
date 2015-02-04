Eagle Chat Firmware
==========================

Firmware for the Eagle Chat secure handheld text messaging hardware peripheral.

This project uses the Atmel Software Framework and the AVR GCC toolchain, and is designed to target the ATxmega192A3u microcontroller, although other Atmel Xmega products should be trivial to support.

### Structure ###
Code is organized into folders representing individual features, which have a .c and .h file to allow reuse by other modules, as well as a main.c or similar that acts as a demonstration of this module and its features.

### Developer set up ###
As mentioned previously, the Atmel Software Framework is used by this project and must be available to build. To let the project know about your copy of the ASF's location:

1. Make a copy of `conf.mk.bak` in the root project folder and name it `conf.mk`
2. Edit the variable `ASF_PATH` in `conf.mk` to point to the top level folder of the ASF 
   (__NOTE__: absolute paths are recommended)

#### Developing new features ####
The easiest way to get started developing a new code module is to make a copy of the `reference/` folder, which contains a sample config.mk and code files. Use these as a starting point, updating config.mk and asf.h as necessary.

Certain features of the project are shared amongst sub-modules to ease future integration. These global configurations are stored in `config/`. For example, pin definitions should be placed in `user_board.h`. This prevents conflict between submodules when assigning external interfaces.

