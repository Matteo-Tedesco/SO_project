# set this to false to disable sonars in firmware
CC=avr-gcc
AS=avr-gcc
INCLUDE_DIRS=-I.
CC_OPTS=-Wall --std=gnu99 -DF_CPU=16000000UL -O3 -funsigned-char -funsigned-bitfields  -fshort-enums -Wall -Wstrict-prototypes -mmcu=atmega2560 $(INCLUDE_DIRS)  -D__AVR_3_BYTE_PC__
AS_OPTS=-x assembler-with-cpp $(CC_OPTS)

AVRDUDE=avrdude

# com1 = serial port. Use lpt1 to connect to parallel port.
AVRDUDE_PORT = /dev/ttyACM0    # programmer connected to serial device

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET):i
AVRDUDE_FLAGS = -p m2560 -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -b 115200
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)
AVRDUDE_FLAGS += -D -q -V -C /usr/share/arduino/hardware/tools/avr/../avrdude.conf
AVRDUDE_FLAGS += -c wiring


OBJS=uart.o\
     tcb.o\
     tcb_list.o\
     atomport_asm.o\
     timer.o\
     scheduler.o\
		 buffered_rw.o\
		 interrupt.o

HEADERS=uart.h\
        tcb.h.h\
	tcb_list.h\
	atomport_asm.h\
	timer.h\
        scheduler.h\
	buffered_rw.h\
	interrupt.h

BINS= main.elf client

.phony:	clean all

all:	$(BINS) 

#common objects
%.o:	%.c 
	$(CC) $(CC_OPTS) -c  $<

%.o:	%.s 
	$(AS) $(AS_OPTS) -c  $<

%.elf:	%.o $(OBJS)
	$(CC) $(CC_OPTS) -o $@ $< $(OBJS) $(LIBS)


%.hex:	%.elf
	avr-objcopy -O ihex -R .eeprom $< $@
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$@:i #$(AVRDUDE_WRITE_EEPROM) 

client:	client.c serial_linux.c 
	gcc --std=gnu99 -o build/$@ $^

clean:	
	rm -rf $(OBJS) $(BINS) *.hex *~ *.o && rm -rf ./build/*
