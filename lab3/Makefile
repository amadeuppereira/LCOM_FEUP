#Makefile for lab3

COMPILER_TYPE= gnu

CC= gcc

PROG= lab3
SRCS= kbd_assembly.S lab3.c test3.c keyboard.c timer.c

CFLAGS= -Wall
CPPFLAGS += -D LAB3
DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>