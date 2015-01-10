# makefile for Project

# clock of the arduino micro
CLOCK=16000000UL

# the arduino micro 
CPU=atmega32u4
DUDECPU=ATMEGA32U4

TTY=/dev/ttyACM0

CC=avr-gcc
OBJCOPY=avr-objcopy

FLASHTOOL=avrdude -f -V -c arduino -p $(DUDECPU) -P $(TTY) -b 115200 -U flash:w:

OBJFILES=main.o
TARGET=main.hex


all: $(TARGET)

flash: $(TARGET)
	$(FLASHTOOL)$<

clean: $(TARGET) $(OBJFILES)
	rm -f $<


# general methods
%.o: %.c
	$(CC)  -Os -DF_CPU=$(CLOCK) --mmcu=$(CPU) -c -o $@ $<

# link together every file
%: $(OBJFILES)
	$(CC) --mmcu=$(CPU) -o $@ $<

%.hex: %
	$(OBJCPY) -O ihex -R .eeprom $< $@


