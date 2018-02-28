#include "i8042.h"

.globl _outB
.globl _kbd_read_scancode_asm

.data
	_outB:
		.long 0
	contador:
		.long 20

.text

_kbd_read_scancode_asm :

		xorl %eax , %eax
		mov contador, %ebx

status: inb $STATUS_PORT
		dec %ebx
		jz fim
		test $OUT_BUF_FULL, %al
		jz status

		xorl %eax , %eax
		inb $KBD_OUT_BUF, %al
		movb %al, _outB

fim:	ret
