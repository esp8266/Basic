BOARD_TAG    = leonardo
ARDUINO_PORT = /dev/ttyUSB0
ARDUINO_LIBS = SPI FTOLED SD SD/utility

ARDMK_DIR ?= /usr/share/arduino
AVR_TOOLS_DIR ?= /usr

include ${ARDMK_DIR}/Arduino.mk
#CPPFLAGS += -Wall -Werror

# Arduino Leonardo custom reset command (NB: the echo on the end is important, due to parent makefile)
RESET_CMD = /bin/stty -F ${ARDUINO_PORT} 1200; sleep 0.5; echo
