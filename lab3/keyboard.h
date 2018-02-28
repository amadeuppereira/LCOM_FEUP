#ifndef _KBD_H
#define _KBD_H

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include "i8042.h"
#include "i8254.h"


#define INTERRUPT_BIT_KBD  1
#define DELAY_US 		10000


/**
 *  Subscribes and enables Keyboard interrupts
 *
 *  Returns bit order in interrupt mask; negative value on failure
 */

int kbd_subscribe_int();

/**
 *  Unsubscribes  Keyboard interrupts
 *
 *  Returns 0 if success and non-zero otherwise
 */

int kbd_unsubscribe_int();

/**
 *
 *  Reads scancode from kbd out buffer
 *
 *  Returns 0 if success and non-zero otherwise
 */
int kbd_read_scancode(unsigned long *scancode);

/**
 *
 * checks if the scancode is the makecode or the breakcode
 *
 *  Returns 0 if makecode and 1 if breakcode
 */
int isBreakCode(unsigned long scancode);

/**
 *	reads command byte and puts it at @param scancode
 *
 *  Returns 0 if success and non-zero otherwise
 */
int kbd_read_command(unsigned long *code);

/**
 *	writes command byte
 *
 *  Returns 0 if success and non-zero otherwise
 */

int kbd_wrt_command(unsigned long code);

int kbd_status(unsigned long *status);

void print_counter();


#endif
