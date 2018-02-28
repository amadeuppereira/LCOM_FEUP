#ifndef _MOUSE_H
#define _MOUSE_H

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include "i8042.h"

#define INTERRUPT_BIT_MOUSE  2
#define DELAY_US 		10000


int mouse_subscribe_int();
int mouse_unsubscribe_int();
int read_outbuf(unsigned long *outbuf);
int read_status(unsigned long *status);
int mouse_enable_data();
int mouse_disable_data();
int mouse_set_stream_mode();
int mouse_set_remote_mode();
int wait_inbf();
int wait_outb();
int mouse_write(unsigned long cmd);
int kbc_read_command(unsigned long *code);
int kbc_write_command(unsigned long code);
int mouse_request_packet();
int read_packet();
int packet_print(unsigned long byte);

#endif
