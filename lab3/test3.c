#include "test3.h"
#include "keyboard.h"
#include "timer.h"
#include <stdio.h>

extern int outB;

int kbd_test_scan(unsigned short assembly) {


	unsigned long scancode;

	int ipc_status;
	message msg;
	int irq_set = BIT(INTERRUPT_BIT_KBD), r;

	if (kbd_subscribe_int() != INTERRUPT_BIT_KBD) {
		printf("Subscribe error! ");
		return 1;
	}

	while (scancode != ESC_BREAK) { /* You may want to use a different condition */
		/* Get a reqest message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					if(assembly == 0) { // c
						if(kbd_read_scancode(&scancode) != 0) {
							printf("Error reading from out buffer!\n");
							return 1;
						}
						kbd_print(scancode);
					}

					else {  // assembly
						kbd_read_scancode_asm(); // calls assembly function
						scancode = outB;
						kbd_print(scancode);  // prints scancode
					}

				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}



	if (kbd_unsubscribe_int() != 0) {
		printf("KBD failed unsubscribe\n");
		return 1;
	}

	print_counter();

	return 0;

}

int kbd_test_poll() {     // after first enter is displaying garbage

	unsigned long command = 0, scancode = 0, status = 0;

	if (kbd_read_command(&command) != 0){
		printf("Error reading command.\n");
		return 1;
	}

	if(kbd_wrt_command(command & 0xfe) != 0){         // setting command bit 0 to 0
		printf("Error calling kbd_wrt_command. \n");
		return 1;
	}

	while(scancode != ESC_BREAK) {
		if(kbd_status(&status) != 0) {
			printf("Error reading status.\n");
			return 1;
		}
		if(((status & OUT_BUF_FULL) == OUT_BUF_FULL) && ((status & TRANS_TIMEOUT) != TRANS_TIMEOUT)) {
			if(kbd_read_scancode(&scancode) != 0) {
				printf("Error reading scancode.\n");
				return 1;
			}
			kbd_print(scancode);
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}

	if(kbd_wrt_command((command | 0x01))!= 0){         // setting command bit 0 to 1
			printf("Error calling kbd_wrt_command. \n");
			return 1;
		}

	print_counter();

	return 0;

}

int kbd_test_timed_scan(unsigned short n) {

	int counter = n*60;
	unsigned long scancode;

	int ipc_status;
	message msg;
	int irq_set = BIT(INTERRUPT_BIT_KBD), r;
	int irq_set_timer = BIT(INTERRUPT_BIT);

	if (kbd_subscribe_int() != INTERRUPT_BIT_KBD) {
		printf("KBD subscribe error!\n");
		return 1;
	}

	if (timer_subscribe_int() != INTERRUPT_BIT) {
		printf("TIMER subscribe error!\n");
		return 1;
	}


	while ((counter > 0) && (scancode != ESC_BREAK)) { /* You may want to use a different condition */
		/* Get a reqest message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					if (kbd_read_scancode(&scancode) != 0) {
						printf("Error reading from out buffer!\n");
						return 1;
					}
					kbd_print(scancode);
					counter = n*60;

				}

				if (msg.NOTIFY_ARG & irq_set_timer) { /* subscribed interrupt */

					counter--;

				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	if(counter == 0)
		printf("Time's out!\n");

	if (kbd_unsubscribe_int() != 0) {
		printf("KBD failed unsubscribe\n");
		return 1;
	}

	if (timer_unsubscribe_int() != 0) {
		printf("TIMER failed unsubscribe\n");
		return 1;
	}

	return 0;
}

int kbd_print(unsigned long scancode) {

	static int isTwoByte = 0;

	if (scancode == TWOBYTE_SCANCODE) {
		isTwoByte = 1;
		return 0;
	}

	if (isTwoByte) {
		isTwoByte = 0;
		if (isBreakCode(scancode)) {
			printf("breakcode : 0xe0 0x%02x\n", scancode);

		}
		else {
			printf("makecode  : 0xe0 0x%02x\n", scancode);
		}
	} else {
		if (isBreakCode(scancode)) {
			printf("breakcode : 0x%02x\n", scancode);
		} else {
			printf("makecode  : 0x%02x\n", scancode);
		}
	}

	return 0;
}
