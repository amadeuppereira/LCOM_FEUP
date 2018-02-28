#ifndef _RTC_H
#define _RTC_H

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/driver.h>

#define BIT(n) (0x01<<(n))


/** @defgroup RTC RTC
 * @{
 *
 * Constants for programming the rtc
 */
#define RTC_ADDR_REG  0x70
#define RTC_DATA_REG  0x71

#define RTC_IRQ 		 8	///< rtc IRQ line


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

/**@}*/

/**
 * @brief Get the time of the day
 *
 * @param hours address to the hour
 * @param minutes address to the minutes
 * @param seconds address to the seconds
 *
 * @return Return 1 if fail , 0 otherwise
 */
int rtc_get_time(unsigned long *seconds , unsigned long *minutes , unsigned long * hours);

/**
 * @brief Uses inline assembly to disable interrupts (CLI)
 */

void rtc_dis_int();

/**
 * @brief Uses inline assembly to enable interrupts (STI)
 */

void rtc_ena_int();

/**
 * @brief Checks if UIP is set
 *
 * @return Return 1 if UIP is set , 0 if not
 */
int update_in_progress();

/**
 * @brief Checks if time, alarm and date registers are in BCD
 *
 * @return Return 1 if BCD , 0 if Binary
 */
int is_bcd();
/**
 * @brief Get the date of the day
 *
 * @param ano address to the year
 * @param mes address to the month
 * @param dia address to the day
 *
 * @return Return 1 if fail , 0 otherwise
 */

int rtc_get_date(unsigned long *ano, unsigned long *mes, unsigned long *dia);

#endif
