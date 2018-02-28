#include <minix/syslib.h>
#include <minix/drivers.h>
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



