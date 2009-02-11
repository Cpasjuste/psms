# SMS Plus - Sega Master System / Game Gear emulator
# (c) 1999, 2000, 2001, 2002, 2003  Charles MacDonald

# -DLSB_FIRST   - Leave undefined for big-endian processors.
# -DALIGN_DWORD - Align 32-bit memory transfers

CC	=	gcc
AS	=	nasm -f coff
LDFLAGS	=	
FLAGS	=	-I. -Icpu -Idos -Isound \
		-Werror -Wall \
		-O6 -mcpu=pentium -fomit-frame-pointer \
		-DLSB_FIRST -DX86_ASM

LIBS	=	-lalleg -laudio -lz

OBJ	=       obj/z80.oa	\
		obj/sms.o	\
		obj/render.o	\
		obj/vdp.o 	\
		obj/system.o    \
		obj/unzip.o     \
		obj/loadzip.o 	\
	
OBJ	+=      obj/sn76496.o	\
		obj/emu2413.o
		
OBJ	+=	obj/dos.o	\
		obj/sealintf.o	\
		obj/config.o	\
		obj/expand.o	\
		obj/blur.o	\
		obj/blit.o

EXE	=	sp.exe

all	:	$(EXE)

$(EXE)	:	$(OBJ)
		$(CC) -o $(EXE) $(OBJ) $(LIBS) $(LDFLAGS)
	        
obj/%.oa :	cpu/%.c cpu/%.h
		$(CC) -c $< -o $@ $(FLAGS)

obj/%.o : 	%.c %.h
		$(CC) -c $< -o $@ $(FLAGS)
	        
obj/%.o :	dos/%.s
		$(AS) $< -o $@
	        
obj/%.o :	sound/%.c sound/%.h	        
		$(CC) -c $< -o $@ $(FLAGS)
	        
obj/%.o :	cpu/%.c cpu/%.h	        
		$(CC) -c $< -o $@ $(FLAGS)

obj/%.o :	dos/%.c dos/%.h	        
		$(CC) -c $< -o $@ $(FLAGS)
	        
pack	:
		strip $(EXE)
		upx -1 $(EXE)	        

clean	:	        
		rm -f obj/*.o
		rm -f *.bak
		rm -f *.exe
		rm -f *.log
		rm -f *.wav
		rm -f *.zip
cleancpu :		
		rm -f obj/z80.oa

makedir :
		mkdir obj
	        
archive:	        
		pk -dir -add -max \
		-excl=rom -excl=src -excl=test -excl=zip \
		-excl=obj -excl=doc -excl=bak -excl=out \
		mdsrc.zip *.*
	        
#
# end of makefile
#
