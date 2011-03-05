
COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 


AOPS = --warn --fatal-warnings

OOPS = -std-compile-opts -strip-debug

LOPS = -Wall -m32 -emit-llvm 

LLCOPS = -march=msp430



all : mspdiss mulator test.list xdiss.list utest.list utest_not.list lutest.list lutest_not.list host 

mspdiss : mspdiss.c
	clang mspdiss.c -o mspdiss

mulator : mulator.c
	clang mulator.c -o mulator

clean :
	rm -f mspdiss
	rm -f mulator
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.bc
	rm -f luart*
	rm -f lutest*
	rm -f host
	rm -f *.hex

startup.o : startup.s
	msp430-as startup.s -o startup.o

reset.o : reset.s
	msp430-as reset.s -o reset.o

test.o : test.s
	msp430-as test.s -o test.o

test.elf : memmap startup.o test.o
	msp430-ld -T memmap test.o startup.o -o test.elf

test.list : test.elf
	msp430-objdump -D test.elf > test.list
	msp430-objcopy -O ihex test.elf test.hex


xvect.o : xvect.s
	msp430-as xvect.s -o xvect.o

xdiss.o : xdiss.s
	msp430-as xdiss.s -o xdiss.o

xdiss.elf : xmemmap xvect.o xdiss.o
	msp430-ld -T xmemmap xdiss.o xvect.o -o xdiss.elf

xdiss.list : xdiss.elf
	msp430-objdump -D xdiss.elf > xdiss.list
	msp430-objcopy -O ihex xdiss.elf xdiss.hex


uart.o : uart.c
	msp430-gcc $(COPS) -c uart.c -o uart.o

utest.o : utest.c
	msp430-gcc $(COPS) -c utest.c -o utest.o

utest.elf : memmap startup.o reset.o uart.o utest.o
	msp430-ld -T memmap reset.o uart.o utest.o startup.o -o utest.elf

utest.list : utest.elf
	msp430-objdump -D utest.elf > utest.list
	msp430-objcopy -O ihex utest.elf utest.hex

notuart.o : notuart.c
	msp430-gcc $(COPS) -c notuart.c -o notuart.o

utest_not.elf : memmap startup.o reset.o notuart.o utest.o
	msp430-ld -T memmap reset.o notuart.o utest.o startup.o -o utest_not.elf

utest_not.list : utest_not.elf
	msp430-objdump -D utest_not.elf > utest_not.list
	msp430-objcopy -O ihex utest_not.elf utest_not.hex









luart.o : uart.c 
	clang $(LOPS) -c uart.c -o luart.raw.bc
	opt $(OOPS) luart.raw.bc -f -o luart.bc
	llc $(LLCOPS) luart.bc -o luart.s
	msp430-as luart.s -o luart.o	

lutest.o : utest.c 
	clang $(LOPS) -c utest.c -o lutest.raw.bc
	opt $(OOPS) lutest.raw.bc -f -o lutest.bc
	llc $(LLCOPS) lutest.bc -o lutest.s
	msp430-as lutest.s -o lutest.o	

lutest.elf : memmap startup.o reset.o luart.o lutest.o
	msp430-ld -T memmap reset.o luart.o lutest.o startup.o -o lutest.elf
	msp430-objcopy -O ihex lutest.elf lutest.hex

lutest.list : lutest.elf
	msp430-objdump -D lutest.elf > lutest.list

lnotuart.o : notuart.c 
	clang $(LOPS) -c notuart.c -o lnotuart.raw.bc
	opt $(OOPS) lnotuart.raw.bc -f -o lnotuart.bc
	llc $(LLCOPS) lnotuart.bc -o lnotuart.s
	msp430-as lnotuart.s -o lnotuart.o	

lutest_not.elf : memmap startup.o reset.o lnotuart.o lutest.o
	msp430-ld -T memmap reset.o lnotuart.o lutest.o startup.o -o lutest_not.elf
	msp430-objcopy -O ihex lutest_not.elf lutest_not.hex

lutest_not.list : lutest_not.elf
	msp430-objdump -D lutest_not.elf > lutest_not.list















host : host.c utest.c
	clang host.c utest.c -o host




