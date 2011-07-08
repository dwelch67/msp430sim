
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 


AOPS = --warn --fatal-warnings

OOPS = -std-compile-opts -strip-debug -disable-simplify-libcalls

LOPS = -Wall -m32 -emit-llvm 

HLOPS = -Wall -emit-llvm 

LLCOPS = -march=msp430


all : decomp lzfx_decomp_test.list llzfx_decomp_test.list

clean :
	rm -f mspdiss
	rm -f mulator
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.bc
	rm -f *.hex
	rm -f llzfx*
	rm -f luart*
	rm -f decomp

#host test program

decomp : decomp.c lzfx_decomp.c lzfx_decomp_test.c
	gcc -O2 -o decomp decomp.c lzfx_decomp.c lzfx_decomp_test.c


#shared between gnu and llvm

startup.o : startup.s
	msp430-as startup.s -o startup.o

reset.o : reset.s
	msp430-as reset.s -o reset.o

#using the gnu gcc/binutils tools

uart.o : uart.c
	msp430-gcc $(COPS) -c uart.c -o uart.o

lzfx_decomp.o : lzfx_decomp.c
	msp430-gcc $(COPS) -c lzfx_decomp.c -o lzfx_decomp.o

lzfx_decomp_test.o : lzfx_decomp_test.c
	msp430-gcc $(COPS) -c lzfx_decomp_test.c -o lzfx_decomp_test.o


lzfx_decomp_test.elf : memmap startup.o reset.o uart.o lzfx_decomp_test.o lzfx_decomp.o
	msp430-ld -T memmap reset.o uart.o lzfx_decomp_test.o lzfx_decomp.o startup.o -o lzfx_decomp_test.elf

lzfx_decomp_test.list : lzfx_decomp_test.elf
	msp430-objdump -D lzfx_decomp_test.elf > lzfx_decomp_test.list
	msp430-objcopy -O ihex lzfx_decomp_test.elf lzfx_decomp_test.hex

#using llvm tools

luart.bc : uart.c
	clang $(LOPS) -c uart.c -o luart.bc

llzfx_decomp.bc : lzfx_decomp.c
	clang $(LOPS) -c lzfx_decomp.c -o llzfx_decomp.bc

llzfx_decomp_test.bc : lzfx_decomp_test.c
	clang $(LOPS) -c lzfx_decomp_test.c -o llzfx_decomp_test.bc

llzfx_decomp_test.o : luart.bc llzfx_decomp.bc llzfx_decomp_test.bc
	llvm-link -o=llzfx_decomp_test.linked.bc luart.bc llzfx_decomp.bc llzfx_decomp_test.bc
	opt $(OOPS) llzfx_decomp_test.linked.bc -f -o llzfx_decomp_test.bc
	llc $(LLCOPS) llzfx_decomp_test.bc -o llzfx_decomp_test.s
	msp430-as llzfx_decomp_test.s -o llzfx_decomp_test.o	

llzfx_decomp_test.elf : memmap startup.o reset.o llzfx_decomp_test.o
	msp430-ld -T memmap reset.o llzfx_decomp_test.o startup.o -o llzfx_decomp_test.elf

llzfx_decomp_test.list : llzfx_decomp_test.elf
	msp430-objdump -D llzfx_decomp_test.elf > llzfx_decomp_test.list
	msp430-objcopy -O ihex llzfx_decomp_test.elf llzfx_decomp_test.hex



