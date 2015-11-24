# This is a Makefile to build all the examples in the library
#
# You don't need this file at all if you're using the Arduino IDE - just open any one of the
# example sketch files (.ino files) in the Arduino IDE.
#
#
EXAMPLES = all_drawing_operations countdown fonts_demo moodlight flames loremipsum stripchart

all: TARG=all
clean: TARG=clean
all clean: $(EXAMPLES)

$(EXAMPLES):
	$(MAKE) -C $@ $(TARG)

.PHONY: clean all $(EXAMPLES)
