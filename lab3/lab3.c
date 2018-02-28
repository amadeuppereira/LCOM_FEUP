#include "test3.h"
#include <limits.h>
#include <string.h>
#include <errno.h>


static int proc_args(int argc, char **argv);
static unsigned short parse_ushort(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();
	/* Enable IO-sensitive operations for ourselves */
	 sys_enable_iop(SELF);
	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <if 0, C, else, assembly>\"\n"
			"\t service run %s -args \"poll\"\n"
			"\t service run %s -args \"timedscan <decimal no. - time>\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short assembly, time;
	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if (argc != 3) {
			printf("kbd_test: wrong no. of arguments for kbd_test_scan()\n");
			return 1;
		}
		assembly = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (assembly == USHRT_MAX)
			return 1;
		printf("kbd_test::kbd_test_scan(%d)\n", assembly);
		return kbd_test_scan(assembly);
	}
	else if (strncmp(argv[1], "poll", strlen("poll")) == 0) {
		if (argc != 2) {
			printf("kbd_test: wrong no. of arguments for kbd_test_poll()\n");
			return 1;
		}
		printf("kbd_test::kbd_test_poll()\n");
		return kbd_test_poll();
	}
	else if (strncmp(argv[1], "timedscan", strlen("timedscan")) == 0) {
		if (argc != 3) {
			printf("kbd_test: wrong no of arguments for kbd_test_timed_scan()\n");
			return 1;
		}
		time = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (time == USHRT_MAX)
			return 1;
		printf("kbd_test::kbd_test_timed_scan(%d)\n", time);
		return kbd_test_timed_scan(time);
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
		printf("kbd_test: parse_ushort: no digits were found in %s\n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}
