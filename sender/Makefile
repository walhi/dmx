# License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)

PROJECT = main
MCU = atmega8
MCU_FLASH = m8

CC = avr-gcc
SIZE = avr-size
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

PROGRAMMER ?= usbasp

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom
HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0

OBJ_PATH = obj

CFLAGS = -Wall -Os -I. -mmcu=$(MCU) -DF_CPU=12000000L #-DDEBUG_LEVEL=1

PROJECT_SRC = $(PROJECT).c uart.c adc.c led.c

OBJECTS = $(patsubst %.c, $(OBJ_PATH)/%.o, $(PROJECT_SRC))\
	$(filter %.o, $(patsubst %.c, $(OBJ_PATH)/%.o, $(VUSB_SRC)))\
	$(filter %.o, $(patsubst %.S, $(OBJ_PATH)/%.o, $(VUSB_SRC)))


# ------------------ Build --------------------#
all: $(PROJECT).elf size

$(OBJ_PATH)/%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_PATH)/%.o: %.S Makefile
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

$(PROJECT).elf:	$(OBJECTS)
	$(CC) $(CFLAGS) -o $(PROJECT).elf $(OBJECTS)

size: $(PROJECT).elf
	avr-size --mcu=$(MCU) --format=avr $<

%.hex:	$(PROJECT).elf
	$(OBJCOPY) -O ihex $(HEX_FLASH_FLAGS) $< $@

%.eep: $(PROJECT).elf
	$(OBJCOPY) $(HEX_EEPROM_FLAGS) -O ihex $< $@

# ------------------ Clean --------------------#
clean:
	rm -f *.hex *.lst *.obj *.cof *.list *.map *.eep *.bin
	rm -f ./$(OBJ_PATH)/*


# ------------------ Flash --------------------#
fuse:
	$(AVRDUDE) -p $(MCU_FLASH) -c $(PROGRAMMER) -e -B 5 -U lfuse:w:0x1f:m -U hfuse:w:0xd9:m

flash: $(PROJECT).hex
	$(AVRDUDE) -p $(MCU_FLASH) -c $(PROGRAMMER) -B 1 -U flash:w:$(PROJECT).hex

eeprom: $(PROJECT).eep
	$(AVRDUDE) -p $(MCU_FLASH) -c $(PROGRAMMER) -B 1 -U eeprom:w:$(PROJECT).eep

# ------------------ Tags ---------------------#
ETAGS?=etags
.PHONY: TAGS tags
TAGS:
	$(ETAGS) `ls *.[ch]`

tags:
	make TAGS ETAGS=ctags
