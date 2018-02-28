#Makefile for lab5

COMPILER_TYPE= gnu

CC= gcc

PROG= lab5
SRCS= test5.c lab5.c vbe.c video_gr.c timer.c keyboard.c sprite.c xpm.c video_test.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>