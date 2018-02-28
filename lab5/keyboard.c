#include "keyboard.h"
#include "video_gr.h"


int hookID_kbd = INTERRUPT_BIT_KBD;


int kbd_subscribe_int(void){

	if (sys_irqsetpolicy( KBD_IRQ , IRQ_REENABLE | IRQ_EXCLUSIVE , &hookID_kbd ) != 0)
	{
		printf("Error setting keyboard interrupts ");
		return -1;
	}

	if (sys_irqenable(&hookID_kbd) !=0)
	{
		printf ("Error calling sys_irqenable");
		return -1;
	}

	return INTERRUPT_BIT_KBD;

}

int kbd_unsubscribe_int (void){

	if (sys_irqdisable(&hookID_kbd) !=0)
	{
		printf("Error calling sys_irqdisable");
		return 1;
	}

	if (sys_irqrmpolicy(&hookID_kbd) !=0)
	{
		printf ("Error removing policy");
		return 1;
	}

	return 0;
}

int kbd_read_scancode(unsigned long *scancode)
{
	unsigned long status, counter = 0;

	while(counter < 20){                    // while out buff not set
		if(kbd_status(&status) != 0) {  	// reads status register
			return 1;
		}

		if(status & OUT_BUF_FULL)  // checks if  bit 0 is set
			break;

		counter++;
	}


	if (sys_inb(KBD_OUT_BUF ,scancode) != 0){  // reads the scancode from out buffer
		printf("Error reading from buffer");
		return 1;
	}
	return 0;
}

int isBreakCode(unsigned long scancode)
{
	if ((scancode & BIT(7)) == BREAK_CODE)
		return 1;
	return 0;

}

int kbd_read_command(unsigned long *code) {

	unsigned long status;

	if (kbd_status(&status) != 0) {
		printf("Error reading status register.");
		return 1;
	}

	if ((status & IN_BUF_FULL) == IN_BUF_FULL) { // checks if in buff is set
		printf("Error , IBF is set.");
		return 1;
	}

	if (sys_outb(KBD_IN_BUF, READ_KBD_COMMAND) != 0) {  // writes command (0x20) in kbd in buffer (0x64)
		printf("Error calling sys_outb!\n");
		return 1;
	}

	if (sys_inb(KBD_OUT_BUF, code) != 0) {  // reads kbd out buffer (0x60)
		printf("Error calling sys_inb!\n");
		return 1;
	}

	return 0;
}

int kbd_wrt_command(unsigned long code) {

	unsigned long status;

	if (kbd_status(&status) != 0){
		printf ("Error reading status register.");
		return 1;
	}

	if((status & IN_BUF_FULL) == IN_BUF_FULL ){  // checks if in buff is set
		printf ("Error , IBF is set.");
		return 1;
	}

	if (sys_outb(KBD_IN_BUF, WRITE_KBD_COMMAND) != 0){ // writes command (0x60) in kbd in buffer (0x64)
		printf("Error calling sys_outb!\n");
		return 1;
	}

	if (sys_outb(KBD_OUT_BUF, code) != 0) {  // writes code in  0x60 port ;;
		printf("Error calling sys_out!\n");
		return 1;
	}

	return 0;
}

int kbd_status(unsigned long *status) {

	if(sys_inb(STATUS_PORT, status) != 0) { // reads status register
		printf("Error reading status!\n");
		return 1;
	}

	if(((*status & PARITY_EVEN) == PARITY_EVEN) || ((*status & RCV_TIMEOUT) == RCV_TIMEOUT)) { // checks if bit 7 (parity) and bit 6 (timeout) are set
		printf("Invalid data!\n");
		return 1;
	}

	return 0;
}

int exit_with_esc() {

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

					if (kbd_read_scancode(&scancode) != 0) {
						printf("Error reading from out buffer!\n");
						return 1;
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

	if (vg_exit()) {
		printf("Error exiting from vg\n");
		return 1;
	}

	return 0;
}
