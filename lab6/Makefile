#Makefile for lab6

COMPILER_TYPE= gnu

CC= gcc

PROG= lab6
SRCS= test6.c lab6.c rtc.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>