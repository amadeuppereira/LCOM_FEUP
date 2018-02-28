#include "rtc.h"
#include <limits.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv) {

	sef_startup();

	/* Enable IO-sensitive operations for ourselves */
	sys_enable_iop(SELF);

	unsigned long sec, min, hour, dia,mes,ano;

	if (rtc_get_time(&sec, &min, &hour) != 0) {
		printf("Error , couldn't get the time !");
		return 1;
	}

	if (rtc_get_date(&ano, &mes, &dia) != 0) {
		printf("Error , couldn't get date !");
		return 1;
	}

	printf("TIME : %d:%d:%d \n", hour, min, sec);
	printf("DATE : %d-%d-%d \n" , ano ,mes ,dia);

	return 0;

}

