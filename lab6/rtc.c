#include "rtc.h"


void rtc_dis_int(){

	asm("cli");                //  CLI(disable interrupts)

}

void rtc_ena_int(){

	asm("sti");                 // STI(enable interrupts)
}



int update_in_progress(){

	unsigned long aux;

	if(sys_outb(RTC_ADDR_REG,RTC_REGISTER_B ) != 0) return 1;
	if(sys_inb(RTC_DATA_REG , &aux ) != 0) return 1;

	if (aux & 0x80) return 1;   // checks if UIP is set (1 while update or 0 if update is done)
	else return 0;


}

int is_bcd(){

	unsigned long mes;

	if(sys_outb(RTC_ADDR_REG,RTC_REGISTER_B ) != 0) return 1;
	if(sys_inb(RTC_DATA_REG , &mes ) != 0) return 1;

	if(!(mes & 0x04)) return 1;      // checks if time, alarm and date registers are in binary
	else return 0;					// or  bcd (1 binary , 0 in bcd)
}



int rtc_get_time(unsigned long *seconds , unsigned long *minutes , unsigned long * hours){

	rtc_dis_int();

	do {
		if (update_in_progress() == 0)
			break;
	} while (1);

	if (sys_outb(RTC_ADDR_REG, RTC_SECONDS) != 0) {
		return 1;
	}
	if (sys_inb(RTC_DATA_REG, seconds) != 0) {
		return 1;
	}

	do {
		if (update_in_progress() == 0)
			break;
	} while (1);

	if (sys_outb(RTC_ADDR_REG, RTC_MINUTES) != 0) {
		return 1;
	}
	if (sys_inb(RTC_DATA_REG, minutes) != 0) {
		return 1;
	}

	do {
		if (update_in_progress() == 0)
			break;
	} while (1);

	if (sys_outb(RTC_ADDR_REG, RTC_HOURS) != 0) {
		return 1;
	}
	if (sys_inb(RTC_DATA_REG, hours) != 0) {
		return 1;
	}

	if (is_bcd()){
		* seconds = (((* seconds & 0xF0)  >> 4) * 10) + (* seconds & 0x0F);
		* minutes = (((* minutes & 0xF0) >> 4) * 10) + (* minutes & 0x0F);
		* hours = (((* hours & 0xF0) >> 4) * 10) + (* hours & 0x0F);
	}

	rtc_ena_int();

	return 0;
}

int rtc_get_date(unsigned long *ano, unsigned long *mes, unsigned long *dia) {

	rtc_dis_int();

	do {
		if (update_in_progress() == 0)
			break;
	} while (1);

	if (sys_outb(RTC_ADDR_REG, RTC_YEAR) != 0) {
		return 1;
	}
	if (sys_inb(RTC_DATA_REG, ano) != 0) {
		return 1;
	}

	do {
		if (update_in_progress() == 0)
			break;
	} while (1);

	if (sys_outb(RTC_ADDR_REG, RTC_MONTH) != 0) {
		return 1;
	}
	if (sys_inb(RTC_DATA_REG, mes) != 0) {
		return 1;
	}

	do {
		if (update_in_progress() == 0)
			break;
	} while (1);

	if (sys_outb(RTC_ADDR_REG, RTC_DAY) != 0) {
		return 1;
	}
	if (sys_inb(RTC_DATA_REG, dia) != 0) {
		return 1;
	}

	if (is_bcd()) {
		*ano = (((*ano & 0xF0) >> 4) * 10) + (*ano & 0x0F);
		*mes = (((*mes & 0xF0) >> 4) * 10) + (*mes & 0x0F);
		*dia = (((*dia & 0xF0) >> 4) * 10) + (*dia & 0x0F);
	}

	rtc_ena_int();

	return 0;

}












