#ifndef __TEST5_H
#define __TEST5_H


#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include <minix/sysutil.h>



int rtc_test_conf(void);
int rtc_test_date(void);
int rtc_test_int(/* to be defined in class */);

#endif
