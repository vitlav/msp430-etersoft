GCC=msp430-gcc
CFLAGS=-mmcu=msp430g2553

main: main.c
	${GCC} ${CFLAGS} $< -o $@

pub: main
	mspdebug rf2500 "prog $<"
