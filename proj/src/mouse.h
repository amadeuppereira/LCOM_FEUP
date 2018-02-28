#ifndef _MOUSE_H
#define _MOUSE_H

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include "i8042.h"

#define INTERRUPT_BIT_MOUSE  2
#define DELAY_US 		10000

/**
 *  @brief Subscribes and enables mouse interrupts
 *
 *  @return Returns bit order in interrupt mask; negative value on failure
 */
int mouse_subscribe_int();

/**
 *  @brief Unsubscribes  Keyboard interrupts
 *
 *  @return Returns 0 if success and non-zero otherwise
 */
int mouse_unsubscribe_int();

/**
 *  @brief Reads mouse out buffer, changing the given variable
 *	@param outbuf variable to change
 *  @return Returns 0 if success and non-zero otherwise
 */
int read_outbuf(unsigned long *outbuf);

/**
 *  @brief Reads mouse status, changing the given variable
 *	@param status variable to change
 *  @return Returns 0 if success and non-zero otherwise
 */
int read_status(unsigned long *status);

/**
 * @brief Enables mouse data
 * @return Returns 0 if success and non-zero otherwise
 */
int mouse_enable_data();

/**
 * @brief Disables mouse data
 * @return Returns 0 if success and non-zero otherwise
 */
int mouse_disable_data();

/**
 * @brief Sets mouse stream mode
 * @return Returns 0 if success and non-zero otherwise
 */
int mouse_set_stream_mode();

/**
 * @brief Sets mouse remote mode
 * @return Returns 0 if success and non-zero otherwise
 */
int mouse_set_remote_mode();

/**
 * @brief Waits until mouse in buf full flag is set to 0
 * @return Returns 0 if success and non-zero otherwise
 */
int wait_inbf();

/**
 * @brief Waits until mouse out buf full flag is set to 1
 * @return Returns 0 if success and non-zero otherwise
 */
int wait_outb();

/**
 * @brief Sends a command to the mouse
 * @param cmd command to send
 * @return Returns 0 if success and non-zero otherwise
 */
int mouse_write(unsigned long cmd);

/**
 * @brief Reads a command from the kbc, changing the given variable
 * @param code variable to change
 * @return Returns 0 if success and non-zero otherwise
 */
int kbc_read_command(unsigned long *code);

/**
 * @brief Sends a command to the kbc
 * @param code command to send
 * @return Returns 0 if success and non-zero otherwise
 */
int kbc_write_command(unsigned long code);

/**
 * @brief Requests a packet form the mouse
 * @return Returns 0 if success and non-zero otherwise
 */
int mouse_request_packet();

/**
 * @brief Reads a packet from the mouse and prints it
 * @return Returns 0 if success and non-zero otherwise
 */
int read_packet();

/**
 * @brief Print a packet from the mouse when called 3 times with the three packet bytes
 * @param byte byte from packet
 * @return Returns 1 if successfully printed packet, 0 otherwise
 */
int packet_print(unsigned long byte);

#endif
