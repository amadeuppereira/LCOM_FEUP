#include "keyboard.h"


int hookID_kbd = INTERRUPT_BIT_KBD;
int counter_sys_call = 0;

#ifdef LAB3
int sys_inb_cnt(port_t port, unsigned long *byte) {
	counter_sys_call++;
	return sys_inb(port, byte);
}
#else
#define sys_inb_cnt(p,q) sys_inb(p,q)
#endif

void print_counter() {
	printf("sys_inb calls : %d\n", counter_sys_call);
}

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


	if (sys_inb_cnt(KBD_OUT_BUF ,scancode) != 0){  // reads the scancode from out buffer
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

	if (sys_inb_cnt(KBD_OUT_BUF, code) != 0) {  // reads kbd out buffer (0x60)
		printf("Error calling sys_inb_cnt!\n");
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

	if(sys_inb_cnt(STATUS_PORT, status) != 0) { // reads status register
		printf("Error reading status!\n");
		return 1;
	}

	if(((*status & PARITY_EVEN) == PARITY_EVEN) || ((*status & RCV_TIMEOUT) == RCV_TIMEOUT)) { // checks if bit 7 (parity) and bit 6 (timeout) are set
		printf("Invalid data!\n");
		return 1;
	}

	return 0;
}
