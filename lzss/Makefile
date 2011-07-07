
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 


AOPS = --warn --fatal-warnings

OOPS = -std-compile-opts -strip-debug -disable-simplify-libcalls

LOPS = -Wall -m32 -emit-llvm 

LLCOPS = -march=msp430


all : def lzss_decode_test.list llzss_decode_test.list

clean :
	rm -f mspdiss
	rm -f mulator
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.bc
	rm -f *.hex
	rm -f llzss*
	rm -f luart*
	rm -f def

#host test program

def : def lzss_decode.c lzss_decode_test.c
	gcc -O2 -o def def.c lzss_decode.c lzss_decode_test.c


#shared between gnu and llvm

startup.o : startup.s
	msp430-as startup.s -o startup.o

reset.o : reset.s
	msp430-as reset.s -o reset.o

#using the gnu gcc/binutils tools

uart.o : uart.c
	msp430-gcc $(COPS) -c uart.c -o uart.o

lzss_decode.o : lzss_decode.c
	msp430-gcc $(COPS) -c lzss_decode.c -o lzss_decode.o

lzss_decode_test.o : lzss_decode_test.c
	msp430-gcc $(COPS) -c lzss_decode_test.c -o lzss_decode_test.o


lzss_decode_test.elf : memmap startup.o reset.o uart.o lzss_decode_test.o lzss_decode.o
	msp430-ld -T memmap reset.o uart.o lzss_decode_test.o lzss_decode.o startup.o -o lzss_decode_test.elf

lzss_decode_test.list : lzss_decode_test.elf
	msp430-objdump -D lzss_decode_test.elf > lzss_decode_test.list
	msp430-objcopy -O ihex lzss_decode_test.elf lzss_decode_test.hex

#using llvm tools

luart.bc : uart.c
	clang $(LOPS) -c uart.c -o luart.bc

llzss_decode.bc : lzss_decode.c
	clang $(LOPS) -c lzss_decode.c -o llzss_decode.bc

llzss_decode_test.bc : lzss_decode_test.c
	clang $(LOPS) -c lzss_decode_test.c -o llzss_decode_test.bc

llzss_decode_test.o : luart.bc llzss_decode.bc llzss_decode_test.bc
	llvm-link -o=llzss_decode_test.linked.bc luart.bc llzss_decode.bc llzss_decode_test.bc
	opt $(OOPS) llzss_decode_test.linked.bc -f -o llzss_decode_test.bc
	llc $(LLCOPS) llzss_decode_test.bc -o llzss_decode_test.s
	msp430-as llzss_decode_test.s -o llzss_decode_test.o	

llzss_decode_test.elf : memmap startup.o reset.o llzss_decode_test.o
	msp430-ld -T memmap reset.o llzss_decode_test.o startup.o -o llzss_decode_test.elf

llzss_decode_test.list : llzss_decode_test.elf
	msp430-objdump -D llzss_decode_test.elf > llzss_decode_test.list
	msp430-objcopy -O ihex llzss_decode_test.elf llzss_decode_test.hex

