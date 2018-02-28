#include "test4.h"
#include <limits.h>
#include <string.h>
#include <errno.h>


static int proc_args(int argc, char **argv);
static unsigned short parse_ushort(char *str, int base);
static short parse_short (char *str , int base);
static unsigned long parse_ulong (char *str , int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"packet <decimal no. - cnt>\"\n"
			"\t service run %s -args \"async <decimal no. - time>\"\n"
			"\t service run %s -args \"remote <decimal no. - period> <decimal no. - cnt>\"\n"
			"\t service run %s -args \"gesture <decimal no. - length>\"\n",
			argv[0], argv[0], argv[0] , argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short cnt, time ;
	unsigned long period;
	long length;
	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if (argc != 3) {
			printf("mouse_test: wrong no. of arguments for mouse_test_packet()\n");
			return 1;
		}
		cnt = parse_ushort(argv[2], 10);
		if (cnt == USHRT_MAX)
			return 1;
		printf("mouse_test::mouse_test_packet(%d)\n", cnt);
		return mouse_test_packet(cnt);
	}
	else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if (argc != 3) {
			printf("mouse_test: wrong no. of arguments for mouse_test_async()\n");
			return 1;
		}
		time = parse_ushort(argv[2], 10);
		if (time == USHRT_MAX)
			return 1;
		printf("mouse_test::mouse_test_async(%d)\n",time);
		return mouse_test_async(time);
	}
	else if (strncmp(argv[1], "remote", strlen("remote")) == 0) {
		if (argc != 4) {
			printf("mouse_test: wrong no of arguments for mouse_test_remote()\n");
			return 1;
		}
		period = parse_ulong(argv[2], 10);
		if (period == ULONG_MAX)
			return 1;
		cnt = parse_ushort(argv[3], 10);
		if (cnt == USHRT_MAX)
			return 1;
		printf("mouse_test::mouse_test_remote(%d, %d)\n", period, cnt);
		return mouse_test_remote(period , cnt);
	}
	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
		if (argc != 3) {
			printf("mouse_test: wrong no. of arguments for mouse_test_gesture()\n");
			return 1;
		}
		length = parse_short(argv[2], 10);
		if (length == SHRT_MAX)
			return 1;
		printf("mouse_test::mouse_test_gesture(%d)\n", length);
		return mouse_test_gesture(length);
	}
	else {
		printf("kbd_test: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned short parse_ushort(char *str, int base)
{
	char *endptr;
	unsigned short val;

	/* Convert string to unsigned short */
	val = (unsigned short)strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == USHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("mouse_test: parse_ushort: no digits were found in %s\n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("mouse_test: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static short parse_short(char *str, int base) {
	char *endptr;
	short val;

	/* Convert string to short */
	val = (short) strtol(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == SHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtol");
		return SHRT_MAX;
	}

	if (endptr == str) {
		printf("mouse_test: parse_short: no digits were found in %s\n", str);
		return SHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

