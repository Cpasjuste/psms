
# Makefile for sms+sdl
#
# Copyright (C) 1998, 1999, 2000  Charles Mac Donald
# SDL version by Gregory Montoir <cyx@frenchkiss.net>
#
# Defines :
# LSB_FIRST : for little endian systems.
# X86_ASM   : enable x86 assembly optimizations
# USE_ZLIB  : enable ZIP file support

NAME	  = sms_sdl

CC        = arm-none-linux-gnueabi-gcc
CFLAGS    = `arm-none-linux-gnueabi-sdl-config --cflags` -O2 -mcpu=cortex-a8 -mtune=cortex-a8 -mfloat-abi=softfp -mfpu=neon -DPANDORA
DEFINES   = -DLSB_FIRST
INCLUDES  = -I. -I.. -I../cpu -I../sound -I$(PNDSDK)/include -I$(PNDSDK)/usr/include
LIBS	  = `arm-none-linux-gnueabi-sdl-config --libs` -L$(PNDSDK)/usr/lib

OBJECTS   = main.o saves.o sdlsms.o filters.o \
            ../render.o ../sms.o ../system.o ../vdp.o \
            ../cpu/z80.o ../sound/emu2413.o ../sound/sn76496.o

# (un)comment to enable ZIP support
DEFINES  += -DUSE_ZLIB
LIBS     += -lz
OBJECTS  += unzip.o


all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

clean:
	rm -f $(OBJECTS) $(NAME)
