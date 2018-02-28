#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "test5.h"
#include "timer.h"
#include "keyboard.h"
#include "vbe.h"
#include "lmlib.h"
#include "video_gr.h"
#include "video_test.h"
#include "xpm.h"
#include "sprite.h"
#include "i8254.h"
#include "i8042.h"

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int video_test_init(unsigned short mode, unsigned short delay) {

	if (vg_init(mode) == NULL) {
		return 1;
	}
	vbe_mode_info_t modeInfo;

	if (vbe_get_mode_info(mode, &modeInfo)) {
		return 1;
	}

	if (wait_time(delay)) {
		printf("Error!\n");
		return 1;
	}

	if (vg_exit()) {
		printf("Error exiting from vg\n");
		return 1;
	}

	return 0;

}

int video_test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	char *pointer;
	if ((pointer = vg_init(VBE_MODE_1024_768_256)) == NULL) {
		return 1;
	}

	vg_draw_square(x, y, size, color);

	video_dump_fb();

	if (exit_with_esc()) {
		return 1;
	}

	printf("Terminou !\n");

	return 0;

}

int video_test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {

	char *pointer;
	if ((pointer = vg_init(VBE_MODE_1024_768_256)) == NULL) {
		return 1;
	}

	vg_draw_line(xi, yi, xf, yf, color);

	video_dump_fb();

	if (exit_with_esc()) {
		return 1;
	}

	printf("Terminou !\n");

	return 0;

}

int test_xpm(char *xpm[], unsigned short xi, unsigned short yi) {

	char *pointer;
	if ((pointer = vg_init(VBE_MODE_1024_768_256)) == NULL) {
		return 1;
	}

	vbe_mode_info_t modeInfo;

	if (vbe_get_mode_info(VBE_MODE_1024_768_256, &modeInfo)) {
		return 1;
	}

	if (vg_draw_xpm(xpm, xi, yi)) {
		return 1;
	}

	video_dump_fb();
	if (exit_with_esc()) {
		return 1;
	}

	printf("Terminou !\n");

	return 0;

}

int test_move(char *xpm[], unsigned short xi, unsigned short yi,
		unsigned short xf, unsigned short yf, short s, unsigned short f) {
	char *pointer;
	if ((pointer = vg_init(VBE_MODE_1024_768_256)) == NULL) {
		return 1;
	}

	vbe_mode_info_t modeInfo;

	if (vbe_get_mode_info(VBE_MODE_1024_768_256, &modeInfo)) {
		return 1;
	}

	int flag = (s > 0) ? 1 : 0;

	int xspeed;
	int yspeed;
	int deltaX = xf - xi, deltaY = yf - yi;
	int inc, temp = abs(s);

	if (deltaX == 0) {
		xspeed = 0;
		yspeed = (deltaY > 0) ? temp : -temp;
		inc = (deltaY > 0) ? 1 : -1;
	}

	else if (deltaY == 0) {
		yspeed = 0;
		xspeed = (deltaX > 0) ? temp : -temp;
		inc = (deltaX > 0) ? 1 : -1;

	} else {
		printf("It needs to be an horizontal/vertical movement!\n");
		return 1;
	}

	if (deltaX == deltaY) {
		printf("Choose 2 different points!\n");
		return 1;
	}

	Sprite* sprite = create_sprite(xpm, xi, yi, xspeed, yspeed, modeInfo);

	if (sprite == NULL) {
		vg_exit();
		return 1;
	}

	int frames = 60 / f;
	int counter = 0;
	int frameCounter = 0;

	unsigned long scancode;
	int ipc_status;
	message msg;
	int irq_set_timer = BIT(INTERRUPT_BIT), irq_set_kbd = BIT(
			INTERRUPT_BIT_KBD), r;

	if (timer_subscribe_int() != INTERRUPT_BIT) {
		printf("TIMER subscribe error!\n");
		return 1;
	}

	if (kbd_subscribe_int() != INTERRUPT_BIT_KBD) {
		printf("KBD subscribe error!\n");
		return 1;
	}

	int test = 1;

	while (test) {
		/* Get a reqest message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set_timer) { /* subscribed interrupt */

					if ((counter % frames) == 0) {
						vg_draw_sprite(sprite);
						vg_clear_sprite(sprite);

						if (flag) {
							sprite->x = sprite->x + sprite->xspeed;
							sprite->y = sprite->y + sprite->yspeed;

						} else {
							if ((frameCounter % abs(s)) == 0) {
								if (deltaX == 0) {
									sprite->y = sprite->y + inc;
								} else if (deltaY == 0) {
									sprite->x = sprite->x + inc;
								}
							}
						}

						vg_draw_sprite(sprite);
						video_dump_fb();
						frameCounter++;
					}
					counter++;

				}

				if (msg.NOTIFY_ARG & irq_set_kbd) {
					if (kbd_read_scancode(&scancode) != 0) {
						printf("Error reading from out buffer!\n");
						return 1;
					}
				}
				break;
				/* You may want to use a different condition */default:
				break; /* no other notifications expected: do nothing */
			}

			if (deltaX > 0 || deltaY > 0) {
				if (deltaX == 0)
					test = (sprite->y < yf) && (scancode != ESC_BREAK);
				else if (deltaY == 0)
					test = (sprite->x < xf) && (scancode != ESC_BREAK);
			} else if (deltaX < 0 || deltaY < 0) {
				if (deltaX == 0)
					test = (sprite->y > yf) && (scancode != ESC_BREAK);
				else if (deltaY == 0)
					test = (sprite->x > xf) && (scancode != ESC_BREAK);
			}

		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	if (timer_unsubscribe_int() != 0) {
		printf("TIMER failed unsubscribe\n");
		return 1;
	}

	if (kbd_unsubscribe_int() != 0) {
		printf("KBD failed unsubscribe\n");
		return 1;
	}

	destroy_sprite(sprite);

	if (vg_exit()) {
		printf("Error exiting from vg\n");
		return 1;
	}

	printf("Terminou !\n");

	return 0;
}

int test_controller() {

	mmap_t mapa;

	VbeInfoBlock infoBlock;

	unsigned char *baseAdress;
	baseAdress = lm_init();
	if (baseAdress == NULL) {
		printf("\tvbe_get_mode_info() : lm_init() has failed !");
		return 1;
	}

	lm_alloc(sizeof(VbeInfoBlock), &mapa);

	struct reg86u reg86;

	reg86.u.b.intno = VBE_VIDEOCARD;
	reg86.u.b.ah = VBE_FUNCTION;
	reg86.u.b.al = VBE_RETURN_CONTROLLER_INFO;
	reg86.u.w.es = PB2BASE(mapa.phys);
	reg86.u.w.di = PB2OFF(mapa.phys);
	if (sys_int86(&reg86) != OK) {
		printf("\tvbe_get_mode_info(): sys_int86() failed \n");
		return 1;
	} else {
		if (reg86.u.b.ah == VBE_FUNCTION_CALL_FAILED
				|| reg86.u.b.ah == VBE_FUNCTION_NOT_SUPPORTED
				|| reg86.u.b.ah == VBE_FUNCTION_INVALID_IN_CURRENT_VM) {
			lm_free(&mapa);
			return 1;

		}
	}

	infoBlock = *(VbeInfoBlock*) mapa.virtual;

	lm_free(&mapa);

	//

	printf("%x\n", infoBlock.VbeVersion);

	unsigned short* ptr = (unsigned short*)infoBlock.Reserved;

	while(*ptr != 0xffff) {
		printf("0x%03x:", *ptr);
		ptr++;
	}
	printf("\n");

	printf("%d\n", infoBlock.TotalMemory);

	return 0;

}
