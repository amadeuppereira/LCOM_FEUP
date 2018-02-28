#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"

int handler_counter = 0;
int hookID = INTERRUPT_BIT;

int timer_get_conf(unsigned char timer, unsigned char *st);
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

	unsigned char ctrl = (TIMER_0 + timer | TIMER_LSB_MSB | last4b);

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

void timer_int_handler() {

	handler_counter++;

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

int timer_display_conf(unsigned char conf) {

	//conf display
	printf("\n");
	(conf & 0x80) ? printf("1") : printf("0");
	(conf & 0x40) ? printf("1") : printf("0");
	(conf & 0x20) ? printf("1") : printf("0");
	(conf & 0x10) ? printf("1") : printf("0");
	(conf & 0x08) ? printf("1") : printf("0");
	(conf & 0x04) ? printf("1") : printf("0");
	(conf & 0x02) ? printf("1") : printf("0");
	(conf & 0x01) ? printf("1") : printf("0");
	printf("\n");

	unsigned char mask;

	//output
	printf("\nOUTPUT: ");
	mask = BIT(7); //10000000
	if (conf & mask)
		printf("1\n");
	else
		printf("0\n");

	//null count
	printf("NULL COUNT: ");
	mask = BIT(6); //01000000
	if (conf & mask)
		printf("1\n");
	else
		printf("0\n");

	//type of access
	printf("TYPE OF ACCESS: ");
	mask = TIMER_LSB_MSB; //00110000
	if ((conf & mask) == TIMER_LSB)
		printf("LSB\n");                       //00010000
	else if ((conf & mask) == TIMER_MSB)
		printf("MSB\n");                  //00100000
	else if ((conf & mask) == TIMER_LSB_MSB)
		printf("LSB followed by MSB\n");  //00110000
	else
		printf("Timer not Configured!\n");

	//programmed mode
	printf("PROGRAMMED MODE: ");
	mask = TIMER_OP_MODE; //00001110
	if ((conf & mask) == INTERRUPT_TERMINAL_CNT)
		printf("Interrupt on Terminal Count (0)\n");                  //00000000
	else if ((conf & mask) == TIMER_ONESHOT)
		printf("Hardware Retriggerable One-Shot (1)\n");  //00000010
	else if ((conf & mask) == TIMER_RATE_GEN)
		printf("Rate Generator (2)\n");                  //00000100
	else if ((conf & mask) == TIMER_RATE_GEN_1)
		printf("Rate Generator (2)\n");                //00001100
	else if ((conf & mask) == TIMER_SQR_WAVE)
		printf("Square Wave Mode (3)\n");                //00000110
	else if ((conf & mask) == TIMER_SQR_WAVE_1)
		printf("Square Wave Mode (3)\n");              //00001110
	else if ((conf & mask) == SOFTWARE_STROBE)
		printf("Software Triggered Strobe (4)\n");      //00001000
	else if ((conf & mask) == HARDWARE_STROBE)
		printf("Hardware Triggered Strobe (Retriggerable) (5)\n"); //00001010
	else
		return 1;

	//counting mode
	printf("COUNTING MODE: ");
	mask = TIMER_BCD | TIMER_BIN;
	if ((conf & mask) == TIMER_BCD)
		printf("BCD (4 decades)\n\n");
	else
		printf("Binary (16 bits)\n\n");

	return 0;
}

int timer_test_time_base(unsigned long freq) {

	int tmp = timer_set_frequency(0, freq);

	if (tmp != 0){
		printf ("Error setting freq");
		return 1;
	}

	return 0;
}

int timer_test_int(unsigned long time) {

	int ipc_status;
	message msg;
	int irq_set = BIT(INTERRUPT_BIT), r;

	if (timer_subscribe_int() != INTERRUPT_BIT) {
		printf("Subscribe error! ");
		return 1;
	}

	while ((time * 60) > handler_counter) { /* You may want to use a different condition */
		/* Get a request message. */

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					timer_int_handler();

					if (handler_counter % 60 == 0)
						printf("%d SEC PASSED\n", handler_counter/60);
					/* process it */
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
		printf ("Unsubscribe error! ");
		return 1;
	}

	return 0;
}

int timer_test_config(unsigned char timer) {

	if (timer < 0 || timer > 2){
		printf("Timer out of range");
		return 1;
	}
	unsigned char st;
	int tmp = timer_get_conf(timer, &st);
	if (tmp != 0){
		printf("Error getting config");
		return 1;
	}
	tmp = timer_display_conf(st);
	if (tmp != 0){
		printf("Error on display");
		return 1;
	}
	return 0;
}
