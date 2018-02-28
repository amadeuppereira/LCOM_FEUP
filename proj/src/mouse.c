#include "mouse.h"

int hookID_mouse = INTERRUPT_BIT_MOUSE;


int mouse_subscribe_int() {

	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hookID_mouse)!= 0) {
		printf("Error setting mouse interrupts");
		return -1;
	}
	if (sys_irqenable(&hookID_mouse) != 0) {
		printf("Error calling sys_irqenable");
		return -1;
	}
	return INTERRUPT_BIT_MOUSE;
}

int mouse_unsubscribe_int() {

	if (sys_irqdisable(&hookID_mouse) != 0) {
		printf("Error calling sys_irqdisable");
		return 1;
	}

	if (sys_irqrmpolicy(&hookID_mouse) != 0) {
		printf("Error removing policy");
		return 1;
	}

	return 0;
}

int read_outbuf(unsigned long *outbuf) {

	if (sys_inb(KBC_OUT_BUF, outbuf) != 0) {  // reads the code from out buffer
		printf("Error reading from buffer");
		return 1;
	}
	return 0;
}

int read_status(unsigned long *status) {

	if (sys_inb(STATUS_PORT, status) != 0) { // reads status register
		printf("Error reading status!\n");
		return 1;
	}

	if (((*status & PARITY_EVEN) == PARITY_EVEN) || ((*status & RCV_TIMEOUT) == RCV_TIMEOUT)) { // checks if bit 7 (parity) and bit 6 (timeout) are set
		printf("Invalid data!\n");
		return 1;
	}

	return 0;
}

int wait_inbf(){

	int counter = 0;
	unsigned long status;

	while (counter < 20){

		if (read_status(&status) != 0)
			return 1;

		if (status & IN_BUF_FULL)
			counter ++;
		else break;
	}

	if (counter == 20)
		return 1;
	return 0;
}

int wait_outb() {
	int counter = 0;
	unsigned long status;

	while (counter < 20) {
		read_status(&status);
		if (status & OUT_BUF_FULL)
			break;
		counter++;
		tickdelay(micros_to_ticks(DELAY_US));
	}
	if(counter == 20)
		return 1;
	return 0;
}

int mouse_write( unsigned long cmd) {

	unsigned long response;

	if (wait_inbf() !=0 )
	{
		printf ("IN BUF FULL\n");
		return 1;
	}

	if (sys_outb(CNTRL_REG ,SEND_MOUSE_COMMAND) != 0)
	{
		return 1;
	}


	if (sys_outb(KBC_IN_BUF ,cmd) != 0)
	{
		return 1;
	}

	if (wait_outb() !=0 )
	{
		return 1;
	}


	if(read_outbuf(&response) != 0)
	{
		return 1;
	}

	if (response == NACK)
		return mouse_write(cmd);
	else if (response == ERROR)
		return 1;

	return 0;
}

int mouse_enable_data(){

	if (mouse_write(ENABLE_MOUSE) != 0)
	{
		printf("ENABLE DATA FAILED\n");
		return 1;
	}

	return 0;

}

int mouse_disable_data(){

	if (mouse_write(DISABLE_MOUSE) != 0)
		{
			printf("DISABLE DATA FAILED\n");
			return 1;
		}

	return 0;

}

int mouse_set_stream_mode(){

	if (mouse_write(MOUSE_SET_STREAM_MODE) != 0) {
		printf("SETTING STREAM MODE FAILED\n");
		return 1;
	}

	return 0;

}

int mouse_set_remote_mode(){

	if (mouse_write(MOUSE_REMOTE_MODE) != 0) {
		printf("SETTING REMOTE MODE FAILED\n");
		return 1;
	}

	return 0;

}

int kbc_read_command(unsigned long *code) {

	unsigned long status;

	if (read_status(&status) != 0) {
		printf("Error reading status register.");
		return 1;
	}

	if ((status & IN_BUF_FULL) == IN_BUF_FULL) { // checks if in buff is set
		printf("Error , IBF is set.");
		return 1;
	}

	if (sys_outb(KBC_CMD_BUF, READ_KBD_COMMAND) != 0) {  // writes command (0x20) in kbd in buffer (0x64)
		printf("Error calling sys_outb!\n");
		return 1;
	}

	if (sys_inb(KBC_OUT_BUF, code) != 0) {  // reads out buffer (0x60)
		printf("Error calling sys_inb_cnt!\n");
		return 1;
	}

	return 0;
}

int kbc_write_command(unsigned long code) {

	unsigned long status;

	if (read_status(&status) != 0){
		printf ("Error reading status register.");
		return 1;
	}

	if((status & IN_BUF_FULL) == IN_BUF_FULL ){  // checks if in buff is set
		printf ("Error , IBF is set.");
		return 1;
	}

	if (sys_outb(KBC_CMD_BUF, WRITE_KBD_COMMAND) != 0){ // writes command (0x60) in kbd in buffer (0x64)
		printf("Error calling sys_outb!\n");
		return 1;
	}

	if (sys_outb(KBC_OUT_BUF, code) != 0) {  // writes code in  0x60 port ;;
		printf("Error calling sys_out!\n");
		return 1;
	}

	return 0;
}

int mouse_request_packet() {

	if (mouse_write(READ_DATA) != 0) {
		printf("FAILED TO REQUEST PACKET\n");
		return 1;
	}

	return 0;
}

int read_packet() {

	unsigned long byte;


	if(read_outbuf(&byte) != 0)
		return 1;
	packet_print(byte);

	if (read_outbuf(&byte) != 0)
		return 1;
	packet_print(byte);

	if (read_outbuf(&byte) != 0)
		return 1;
	packet_print(byte);

	return 0;
}

int packet_print(unsigned long byte) {
	static unsigned long packet[3];
	static int counter = 0;

	if (((byte & BIT(3)) && (counter == 0)) || (counter > 0)) {
		packet[counter] = byte;
		counter++;
	}

	if (counter == 3) {
		counter = 0;

		 printf("B1=0x%02X ", packet[0]);
		 printf("B2=0x%02X ", packet[1]);
		 printf("B3=0x%02X ", packet[2]);
		 printf("LB=%u ", (packet[0] & BIT(0)));
		 printf("MB=%u ", (packet[0] & BIT(2)) >> 2);
		 printf("RB=%u ", (packet[0] & BIT(1)) >> 1);
		 printf("XOV=%u ", (packet[0] & BIT(6)) >> 6);
		 printf("YOV=%u ", (packet[0] & BIT(7)) >> 7);

		 //signed char xDelta = packet[1];
		 //signed char yDelta = packet[2];

		 if((packet[0] & BIT(4)))
			 //printf("X=%d ", xDelta);
			 printf("X=-%d ", 256 - packet[1]);
		 else
			 printf("X=%d ", packet[1]);

		 if((packet[0] & BIT(5)))
		 	 //printf("Y=%d \n", yDelta);
			 printf("Y=-%d \n", 256 - packet[2]);
		 else
		 	 printf("Y=%d \n", packet[2]);


		 return 1;

	}

	else return 0;

}

