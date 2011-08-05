
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 


AOPS = --warn --fatal-warnings

OOPS = -std-compile-opts -strip-debug -disable-simplify-libcalls

LOPS = -Wall -m32 -emit-llvm 

HLOPS = -Wall -emit-llvm 

LLCOPS = -march=msp430


all : teatest.list lteatest.list

clean :
	rm -f mspdiss
	rm -f mulator
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.bc
	rm -f *.hex
	rm -f lteatest*
	rm -f luart*
	


#shared between gnu and llvm

startup.o : startup.s
	msp430-as startup.s -o startup.o

reset.o : reset.s
	msp430-as reset.s -o reset.o

#using the gnu gcc/binutils tools

uart.o : uart.c
	msp430-gcc $(COPS) -c uart.c -o uart.o

teatest.o : teatest.c
	msp430-gcc $(COPS) -c teatest.c -o teatest.o

notmain.o : notmain.c
	msp430-gcc $(COPS) -c notmain.c -o notmain.o


teatest.elf : memmap startup.o reset.o uart.o teatest.o notmain.o
	msp430-ld -T memmap reset.o uart.o notmain.o teatest.o startup.o -o teatest.elf

teatest.list : teatest.elf
	msp430-objdump -D teatest.elf > teatest.list
	msp430-objcopy -O ihex teatest.elf teatest.hex

#using llvm tools

luart.bc : uart.c
	clang $(LOPS) -c uart.c -o luart.bc

lnotmain.bc : notmain.c
	clang $(LOPS) -c notmain.c -o lnotmain.bc

lteatest.bc : teatest.c
	clang $(LOPS) -c teatest.c -o lteatest.bc

lteatest.o : luart.bc lnotmain.bc lteatest.bc
	llvm-link -o=lteatest.linked.bc luart.bc lnotmain.bc lteatest.bc
	opt $(OOPS) lteatest.linked.bc -f -o lteatest.bc
	llc $(LLCOPS) lteatest.bc -o lteatest.s
	msp430-as lteatest.s -o lteatest.o	

lteatest.elf : memmap startup.o reset.o lteatest.o
	msp430-ld -T memmap reset.o lteatest.o startup.o -o lteatest.elf

lteatest.list : lteatest.elf
	msp430-objdump -D lteatest.elf > lteatest.list
	msp430-objcopy -O ihex lteatest.elf lteatest.hex



