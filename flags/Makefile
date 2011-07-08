
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 


AOPS = --warn --fatal-warnings

OOPS = -std-compile-opts -strip-debug -disable-simplify-libcalls

LOPS = -Wall -m32 -emit-llvm 

LLCOPS = -march=msp430


all : host limit_test.list 

clean :
	rm -f mspdiss
	rm -f mulator
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.bc
	rm -f *.hex
	rm -f llimit*
	rm -f luart*
	rm -f host

#host test program

host : host.c limit_test.c limit_test.stuff.c
	gcc -O2 -o host host.c limit_test.c limit_test.stuff.c


#shared between gnu and llvm

startup.o : startup.s
	msp430-as startup.s -o startup.o

reset.o : reset.s
	msp430-as reset.s -o reset.o

#using the gnu gcc/binutils tools

uart.o : uart.c
	msp430-gcc $(COPS) -c uart.c -o uart.o

limit_test.o : limit_test.c
	msp430-gcc $(COPS) -c limit_test.c -o limit_test.o

limit_test.stuff.o : limit_test.stuff.c
	msp430-gcc $(COPS) -c limit_test.stuff.c -o limit_test.stuff.o

limit_test.elf : memmap startup.o reset.o uart.o limit_test.o limit_test.stuff.o
	msp430-ld -T memmap reset.o uart.o limit_test.o limit_test.stuff.o startup.o -o limit_test.elf

limit_test.list : limit_test.elf
	msp430-objdump -D limit_test.elf > limit_test.list
	msp430-objcopy -O ihex limit_test.elf limit_test.hex

#using llvm tools

luart.bc : uart.c
	clang $(LOPS) -c uart.c -o luart.bc

llzss_decode.bc : lzss_decode.c
	clang $(LOPS) -c lzss_decode.c -o llzss_decode.bc

llimit_test.bc : limit_test.c
	clang $(LOPS) -c limit_test.c -o llimit_test.bc

llimit_test.stuff.bc : limit_test.stuff.c
	clang $(LOPS) -c limit_test.stuff.c -o llimit_test.stuff.bc

llimit_test.o : luart.bc llimit_test.bc llimit_test.stuff.bc
	llvm-link -o=llimit_test.linked.bc luart.bc llimit_test.bc
	opt $(OOPS) llimit_test.linked.bc -f -o llimit_test.bc
	opt $(OOPS) llimit_test.stuff.bc -f -o llimit_test.stuff.opt.bc
	llvm-link -o=llimit_test.bc llimit_test.linked.bc llimit_test.stuff.opt.bc
	llc $(LLCOPS) llimit_test.bc -o llimit_test.s
	msp430-as llimit_test.s -o llimit_test.o	

llimit_test.elf : memmap startup.o reset.o llimit_test.o
	msp430-ld -T memmap reset.o llimit_test.o startup.o -o llimit_test.elf

llimit_test.list : llimit_test.elf
	msp430-objdump -D llimit_test.elf > llimit_test.list
	msp430-objcopy -O ihex llimit_test.elf llimit_test.hex

