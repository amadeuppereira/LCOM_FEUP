#ifndef _RTC_H
#define _RTC_H

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/driver.h>

#define BIT(n) (0x01<<(n))

#define RTC_ADDR_REG  0x70
#define RTC_DATA_REG  0x71

#define RTC_IRQ 		 8


#define RTC_SECONDS            0
#define RTC_SECONDS_ALARM      1
#define RTC_MINUTES            2
#define RTC_MINUTES_ALARM      3
#define RTC_HOURS 			   4
#define RTC_HOURS_ALARM        5
#define RTC_DAY_OF_WEEK        6
#define RTC_DAY                7
#define RTC_MONTH              8
#define RTC_YEAR 			   9

#define RTC_REGISTER_A		   10
#define RTC_REGISTER_B 		   11
#define RTC_REGISTER_C		   12
#define RTC_REGISTER_D         13


int rtc_get_time(unsigned long *seconds , unsigned long *minutes , unsigned long * hours);
void rtc_dis_int();
void rtc_ena_int();
int update_in_progress();
int is_bcd();
int rtc_get_date(unsigned long *ano, unsigned long *mes, unsigned long *dia);

#endif
