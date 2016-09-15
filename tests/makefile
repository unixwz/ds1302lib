TARG=libtest
CC = avr-gcc
OBJCOPY = avr-objcopy
DUDE = avrdude
SOURCE = libtest.c ../src/ds1302lib.c
OBJS = $(SOURCE:.c=.o)
MCU=atmega8
DMCU=m8  
CFLAGS = -mmcu=$(MCU) -Wall -g -Os -Werror -std=c99
LDFLAGS = -mmcu=$(MCU)  -Wall -g -Os  -Werror 
 
all: $(TARG)
 
$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@.elf  $(OBJS)
	$(OBJCOPY) -j .text -j .data -O ihex $@.elf $@.hex 
	# $(DUDE) -p $(DMCU) -P /dev/ttyUSB0 -c avrisp -b 19200 -U flash:w:$@.hex
 
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
 
clean:
	rm -f *.elf *.bin *.hex  $(OBJS) *.map
