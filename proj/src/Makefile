#Makefile for proj

COMPILER_TYPE= gnu

CC= gcc

PROG= proj
SRCS= proj.c bitmap.c highscores.c rtc.c input.c keyboard.c mouse.c timer.c vbe.c video_gr.c xpm.c objects.c collisions.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>