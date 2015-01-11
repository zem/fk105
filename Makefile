# makefile for Project

# clock of the arduino micro
CLOCK=16000000UL

# the arduino micro 
MMCU=atmega32u4

DUDECPU=m32u4
DUDESPEED=115200
TTY=/dev/ttyACM0

CC=avr-gcc
OBJCPY=avr-objcopy

# note that avrdude seems not to work together with the bootloader
# this means that you need a second Micro first and flash it with 
# the ISP template I did not solve that problem by now. 
#FLASHTOOL=avrdude -F -p $(DUDECPU) -c arduino -P $(TTY) -U 
FLASHTOOL=avrdude -p $(DUDECPU) -c avr109 -P $(TTY) -vvv -U
#-b $(DUDESPEED)
#FLASHTOOL=avrdude -V -c arduino -p -P $(TTY) -b 115200 -U 

OBJFILES=main.o
TARGET=main.hex

all: $(TARGET)
	echo well done

flash: $(TARGET)
	$(FLASHTOOL) flash:w:$<

clean: 
	rm -f *.o *.hex *.bin

# general methods
%.o: %.c
	$(CC)  -Os -DF_CPU=$(CLOCK) -mmcu=$(MMCU) -c -o $@ $<

# link together every file
%.bin: $(OBJFILES)
	$(CC) -mmcu=$(MMCU) -o $@ $<

%.hex: %.bin
	$(OBJCPY) -O ihex -R .eeprom $< $@


