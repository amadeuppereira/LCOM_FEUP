#ifndef _KBD_H
#define _KBD_H

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/driver.h>

#include "i8042.h"
#include "i8254.h"

#define INTERRUPT_BIT_KBD  1
#define DELAY_US 		10000


/**
 *  @brief Subscribes and enables Keyboard interrupts
 *
 *  @return Returns bit order in interrupt mask; negative value on failure
 */

int kbd_subscribe_int();

/**
 *  @brief Unsubscribes  Keyboard interrupts
 *
 *  @return Returns 0 if success and non-zero otherwise
 */

int kbd_unsubscribe_int();

/**
 *  @brief Reads scancode from kbd out buffer, changing the given variable
 *	@param scancode variable to change
 *  @return Returns 0 if success and non-zero otherwise
 */
int kbd_read_scancode(unsigned long *scancode);

/**
 *  @brief Checks if the scancode is the makecode or the breakcode
 *	@param scancode scancode to analyze
 *  @return Returns 0 if makecode and 1 if breakcode
 */
int isBreakCode(unsigned long scancode);

/**
 *	@brief Reads command byte and puts it at, changing the given variable
 *	@param code variable to change
 *  @return Returns 0 if success and non-zero otherwise
 */
int kbd_read_command(unsigned long *code);

/**
 *	@brief Writes command byte
 *	@param code command to write
 *  @return Returns 0 if success and non-zero otherwise
 */
int kbd_wrt_command(unsigned long code);

/**
 *  @brief Reads the keyboard status, changing the given variable
 *  @param status variable to change
 *  @return Returns 0 if success and non-zero otherwise
 */
int kbd_status(unsigned long *status);

#endif
