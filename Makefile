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
PART=ATmega328P
PROTOCOL=arduino
CFLAGS=-Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Ilua -I. -fdata-sections -ffunction-sections
LDFLAGS=-Wl,--gc-sections

all:
	$(CC) $(CFLAGS) -o luaarduino.elf $(LDFLAGS) luaarduino.c
	$(OBJCOPY) -O $(BIN_FORMAT) -R .eeprom luaarduino.elf luaarduino.hex

clean:
	$(RM) luaarduino.elf luaarduino.hex main.lua.o

upload:
	$(AVRDUDE) -c $(PROTOCOL) -p $(PART) -P $(PORT) -U flash:w:luaarduino.hex

upload-lua:
	$(OBJCOPY) -O $(BIN_FORMAT) main.lua main.lua.hex
	$(AVRDUDE) -c $(PROTOCOL) -p $(PART) -P $(PORT) -U eeprom:w:main.lua.hex

.PHONY: all clean upload
