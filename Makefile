# Set up cross tools
AVRDUDE=avrdude -F -V
OBJCOPY=avr-objcopy
CC=avr-gcc

RM=rm -f

# Params
MCU=atmega328p
F_CPU=16000000UL
BIN_FORMAT=ihex
PORT=/dev/ttyACM0
BAUD=115200
PROTOCOL=arduino
PART=ATMEGA3290
CFLAGS=-Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Ilua -I. -fdata-sections -ffunction-sections
LDFLAGS=-Wl,--gc-sections

all:
	$(CC) -c $(CFLAGS) -o luaarduino.o $(LDFLAGS) luaarduino.c
	$(OBJCOPY) --input binary -O elf32-avr main.lua main.lua.o
	$(CC) $(CFLAGS) -o luaarduino.elf luaarduino.o main.lua.o
	$(OBJCOPY) -O $(BIN_FORMAT) -j .eeprom luaarduino.elf luaarduino.hex

clean:
	$(RM) luaarduino.elf luaarduino.hex main.lua.o

upload: all
	$(AVRDUDE) -c $(PROTOCOL) -p $(PART) -P $(PORT) -b $(BAUD) -U flash:w:luaarduino.hex

.PHONY: all clean upload
