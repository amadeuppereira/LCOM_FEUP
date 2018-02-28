#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include "timer.h"
#include "i8254.h"

int handler_counter = 0;
int hookID = INTERRUPT_BIT;


int timer_set_frequency(unsigned char timer, unsigned long freq) {

	if (timer < 0 || timer > 2){
		printf("Timer out of range");
		return 1;
	}
	unsigned long valorLSB = (long) TIMER_FREQ / freq; // valor de 16 bits para alterar a frequencia
	unsigned long valorMSB = (valorLSB >> 8);

	unsigned char conf;
	if (timer_get_conf(timer, &conf) != 0){
		printf ("Error getting timer configuration!");
		return 1;
	}


	unsigned char mask = (BIT(3) | BIT(2) | BIT(1) | BIT(0)); // 00001111
	const unsigned char last4b = conf & mask; // ultimos 4 bits (que não podem ser alterados

	unsigned char ctrl = ((TIMER_0 + timer) | TIMER_LSB_MSB | last4b);

	if (sys_outb(TIMER_CTRL, ctrl) != 0){
		printf("Error making the call");
		return 1;
	}


	if (sys_outb(TIMER_0 + timer, valorLSB) != 0
			|| sys_outb(TIMER_0 + timer, valorMSB) != 0){
		printf("Error loading frequency");
		return 1;
	}
	printf("Frequency set!\n");
	return 0;
}

int timer_subscribe_int(void) {

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hookID) != 0){
		printf("Error setting policy");
		return -1;
	}
	if (sys_irqenable(&hookID) != 0){
		printf("Error irqenable");
		return -1;
	}
	return INTERRUPT_BIT;

}

int timer_unsubscribe_int() {


	if (sys_irqdisable(&hookID) != 0){
		printf("Error irqdisable\n");
		return 1;
	}
	if (sys_irqrmpolicy(&hookID) != 0){
		printf("Error removing policy!");
		return 1;
	}

	return 0;

}

int timer_get_conf(unsigned char timer, unsigned char *st) {

	if (timer < 0 || timer > 2){
		printf("Timer out of range");
		return 1;
	}
	int readbackcommand = sys_outb(TIMER_CTRL,
	TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
	if (readbackcommand != 0) {
		printf("Error writing read back command!\n");
		return 1;
	}
	unsigned long byte;
	readbackcommand = sys_inb(TIMER_0 + timer, &byte);
	*st = byte;
	if (readbackcommand != 0) {
		printf("Error reading from timer!\n");
		return 1;
	}

	return 0;
}

int wait_time (int delay){


	int counter = delay*60;

		int ipc_status;
		message msg;
		int irq_set_timer = BIT(INTERRUPT_BIT) ,r;


		if (timer_subscribe_int() != INTERRUPT_BIT) {
			printf("TIMER subscribe error!\n");
			return 1;
		}


		while (counter > 0) { /* You may want to use a different condition */
			/* Get a reqest message. */
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
				printf("driver_receive failed with: %d", r);
				continue;
			}

			if (is_ipc_notify(ipc_status)) { /* received notification */

				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */

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

		if (timer_unsubscribe_int() != 0) {
			printf("TIMER failed unsubscribe\n");
			return 1;
		}
		return 0;



}

