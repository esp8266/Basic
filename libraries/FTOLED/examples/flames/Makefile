BOARD_TAG    = leonardo
ARDUINO_PORT = /dev/ttyACM3
ARDUINO_LIBS = SPI FTOLED SD digitalWriteFast SD/utility

ARDUINO_DIR ?= /usr/share/arduino
ARDMK_DIR     ?= /usr
AVR_TOOLS_DIR ?= /usr

include ${ARDUINO_DIR}/Arduino.mk
CPPFLAGS += -Wall -Werror
RESET_CMD = /bin/stty -F ${ARDUINO_PORT} 1200; sleep 1.0; echo
