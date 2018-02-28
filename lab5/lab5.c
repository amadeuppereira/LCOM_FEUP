#include "test5.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "pixmap.h"



static int proc_args(int argc, char **argv);
static unsigned short parse_ushort(char *str, int base);
static short parse_short(char *str, int base);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv) {
	sef_startup();

	if (argc == 1) { /* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	} else
		return proc_args(argc, argv);
}

static void print_usage(char **argv) {
	printf(
			"Usage: one of the following:\n"
					"\t service run %s -args \"init <mode> <delay>\"\n"
					"\t service run %s -args \"square <x> <y> <size> <color>\"\n"
					"\t service run %s -args \"line <xi> <yi> <xf> <yf> <color>\"\n"
					"\t service run %s -args \"xpm <xpm> <xi> <yi>\"\n"
					"\t service run %s -args \"move <xpm> <yi> <yi> <xf> <yf> <speed> <frame_rate>\"\n"
					"\t service run %s -args \"controller\"\n",
					argv[0],argv[0], argv[0],argv[0],argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {
	unsigned short mode, delay, x, y, size, xi, yi, xf, yf, frame_rate;
	short speed;
	unsigned long color;
	char **xpm;

	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if (argc != 4) {
			printf(
					"video_test: wrong no. of arguments for video_test_init()\n");
			return 1;
		}
		mode = parse_ushort(argv[2], 16);
		delay = parse_ushort(argv[3], 10);
		if (mode == USHRT_MAX || delay == USHRT_MAX)
			return 1;
		printf("video_test::video_test_init(%x, %d)\n", mode, delay);
		return video_test_init(mode, delay);
	} else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if (argc != 6) {
			printf(
					"video_test: wrong no. of arguments for video_test_square()\n");
			return 1;
		}
		x = parse_ushort(argv[2], 10);
		y = parse_ushort(argv[3], 10);
		size = parse_ushort(argv[4], 10);
		color = parse_ulong(argv[5], 10);
		xf = parse_ushort(argv[4], 10);
		yf = parse_ushort(argv[5], 10);
		if (x == USHRT_MAX || y == USHRT_MAX || size == USHRT_MAX
				|| color == ULONG_MAX)
			return 1;
		printf("video_test::video_test_square(%d, %d, %d, %d)\n", x, y, size,
				color);
		return video_test_square(x, y, size, color);
	}

	else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		if (argc != 7) {
			printf("video_test: wrong no of arguments for video_test_line()\n");
			return 1;
		}
		xi = parse_ushort(argv[2], 10);
		yi = parse_ushort(argv[3], 10);
		xf = parse_ushort(argv[4], 10);
		yf = parse_ushort(argv[5], 10);
		color = parse_ulong(argv[6], 10);
		if (xi == USHRT_MAX || yi == USHRT_MAX || xf == USHRT_MAX
				|| yf == USHRT_MAX || color == ULONG_MAX)
			return 1;
		printf("video_test::video_test_line(%d, %d, %d, %d, %d)\n", xi, yi, xf,
				yf, color);
		return video_test_line(xi, yi, xf, yf, color);
	}

	else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
		if (argc != 5) {
			printf("video_test: wrong no of arguments for test_xpm()\n");
			return 1;
		}

		if (strncmp(argv[2], "pic1", strlen("pic1")) == 0) {
			xpm = pic1;
		} else if (strncmp(argv[2], "pic2", strlen("pic2")) == 0) {
			xpm = pic2;
		} else if (strncmp(argv[2], "pic3", strlen("pic3")) == 0) {
			xpm = pic3;
		} else if (strncmp(argv[2], "cross", strlen("cross")) == 0) {
			xpm = cross;
		} else if (strncmp(argv[2], "penguin", strlen("penguin")) == 0) {
			xpm = penguin;
		} else if (strncmp(argv[2], "car", strlen("car")) == 0) {
			xpm = car;
		} else {
			printf(
					"\nThe one you chose does not exist! Try using pic1 pic2 pic3 cross car or penguin.\n");
			return 1;
		}

		xi = parse_ushort(argv[3], 10);
		yi = parse_ushort(argv[4], 10);
		if (xi == USHRT_MAX || yi == USHRT_MAX)
			return 1;
		printf("video_test::test_xpm(%s, %d, %d)\n", argv[2], xi, yi);
		return test_xpm(xpm, xi, yi);
	}

	else if (strncmp(argv[1], "move", strlen("move")) == 0) {
		if (argc != 9) {
			printf("video_test: wrong no of arguments for test_move()\n");
			return 1;
		}

		if (strncmp(argv[2], "pic1", strlen("pic1")) == 0) {
			xpm = pic1;
		} else if (strncmp(argv[2], "pic2", strlen("pic2")) == 0) {
			xpm = pic2;
		} else if (strncmp(argv[2], "pic3", strlen("pic3")) == 0) {
			xpm = pic3;
		} else if (strncmp(argv[2], "cross", strlen("cross")) == 0) {
			xpm = cross;
		} else if (strncmp(argv[2], "penguin", strlen("penguin")) == 0) {
			xpm = penguin;
		} else if (strncmp(argv[2], "car", strlen("car")) == 0) {
					xpm = car;
		} else {
			printf(
					"\nThe one you chose does not exist! Try using pic1 pic2 pic3 cross or penguin.\n");
			return 1;
		}

		xi = parse_ushort(argv[3], 10);
		yi = parse_ushort(argv[4], 10);
		xf = parse_ushort(argv[5], 10);
		yf = parse_ushort(argv[6], 10);
		speed = parse_short(argv[7], 10);
		frame_rate = parse_ushort(argv[8], 10);
		if (xi == USHRT_MAX || yi == USHRT_MAX || xf == USHRT_MAX
				|| yf == USHRT_MAX || speed == SHRT_MAX
				|| frame_rate == USHRT_MAX)
			return 1;
		printf("video_test::test_move(%s, %d, %d, %d, %d, %d, %d)\n", xpm, xi,
				yi, xf, yf, speed, frame_rate);
		return test_move(xpm, xi, yi, xf, yf, speed, frame_rate);
	}

	else if (strncmp(argv[1], "controller", strlen("controller")) == 0) {
		if (argc != 2) {
			printf("video_test: wrong no of arguments for test_controller()\n");
			return 1;
		}
		printf("video_test::test_controller()\n");
		return test_controller();
	}

	else {
		printf("video_test: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned short parse_ushort(char *str, int base) {
	char *endptr;
	unsigned short val;

	/* Convert string to unsigned short */
	val = (unsigned short) strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == USHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("video_test: parse_ushort: no digits were found in %s\n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

static short parse_short(char *str, int base) {
	char *endptr;
	short val;

	/* Convert string to unsigned short */
	val = (short) strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == SHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return SHRT_MAX;
	}

	if (endptr == str) {
		printf("video_test: parse_short: no digits were found in %s\n", str);
		return SHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

static unsigned long parse_ulong(char *str, int base) {
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
		printf("video_test: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

