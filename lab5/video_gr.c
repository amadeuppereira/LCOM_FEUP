#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "video_gr.h"
#include "vbe.h"
#include "xpm.h"
#include "sprite.h"

#define COLOR_BLACK	 		0

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000  //  0xF0000000  pc da feup
#define H_RES             1024
#define V_RES		  	768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem; /* Process (virtual) address to which VRAM is mapped */

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = VBE_VIDEOCARD; /* BIOS video services */

	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

void *vg_init(unsigned short mode) {

	struct reg86u reg86;
	vbe_mode_info_t modeInfo;

	reg86.u.b.intno = VBE_VIDEOCARD; /* BIOS video services */
	reg86.u.b.ah = VBE_FUNCTION; /* Set Video Mode function */
	reg86.u.b.al = VBE_SET_MODE;
	reg86.u.w.bx = mode | BIT(14);

	if (sys_int86(&reg86) == 0) {
		if (reg86.u.b.ah == VBE_FUNCTION_CALL_FAILED
				|| reg86.u.b.ah == VBE_FUNCTION_NOT_SUPPORTED
				|| reg86.u.b.ah == VBE_FUNCTION_INVALID_IN_CURRENT_VM) {
			return NULL;

		}
	}

	if (vbe_get_mode_info(mode, &modeInfo)) {
		return NULL;
	}
	int r;
	struct mem_range mr;
	unsigned int vram_size; /* VRAM's physical addresss */
	//unsigned int vram_size;  no need , use frame-buffer size

	h_res = modeInfo.XResolution;
	v_res = modeInfo.YResolution;
	bits_per_pixel = modeInfo.BitsPerPixel;

	vram_size = h_res * v_res * (bits_per_pixel / 8);

	/* Allow memory mapping */

	mr.mr_base = modeInfo.PhysBasePtr;
	mr.mr_limit = mr.mr_base + vram_size;

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
		panic("\tvg_init() : sys_privctl (ADD_MEM) failed: %d\n", r);
		return NULL;
	}

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

	if (video_mem == MAP_FAILED) {
		panic("\tvg_init() : couldn't map video memory");
		return NULL;
	}

	return video_mem;

}

void vg_set_color_pixel(unsigned short x, unsigned short y, unsigned long color) {

	if (x < 0 || x >= h_res || y < 0 || y >= v_res) {
	} else
		*(video_mem + (y * h_res * (bits_per_pixel / 8)) + (x * (bits_per_pixel / 8))) = (char) color;

}

void trocaValores(unsigned short *xi, unsigned short *yi, unsigned short *xf, unsigned short *yf) {
	int swap;
	if(*xi > *xf) {
		swap = *xi;
		*xi = *xf;
		*xf = swap;
		swap = *yi;
		*yi = *yf;
		*yf = swap;
	}

	if(*yi > *yf) {
			swap = *xi;
			*xi = *xf;
			*xf = swap;
			swap = *yi;
			*yi = *yf;
			*yf = swap;
		}

}

int vg_draw_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {

	int swap;
	unsigned int x, y, deltax, deltay;

	deltax = xf - xi;
	deltay = yf - yi;

	if (deltay == 0) {
		trocaValores(&xi,&yi, &xf, &yf);
		y = yi;
		for (x = xi; x < xf; x++) {
			vg_set_color_pixel(x, y, color);

		}

	}

	else if (deltax == 0) {
		trocaValores(&xi,&yi, &xf, &yf);
		x = xi;
		for (y = yi; y < yf; y++) {
			vg_set_color_pixel(x, y, color);

		}

	}

	else {
		if(xi > xf) {
			swap = xi;
			xi = xf;
			xf = swap;
			swap = yi;
			yi = yf;
			yf = swap;
		}

		float m = (float) (yf - yi) / (xf - xi);
		float b = yi - m * xi;

		for (x = xi; x <= xf ; x++) {
			y = m * x + b;
			vg_set_color_pixel(x, y, color);
		}
	}

	return 0;
}

int vg_draw_xpm(char *xpm[] ,unsigned short x ,unsigned short y){

	int width, height;
	char * pixmap;
	// get the pix map from the XPM
	pixmap = read_xpm(xpm, &width, &height, h_res, v_res);

	unsigned int i;
	unsigned int j;
	for (i = 0; i < width; ++i) {
		for (j = 0; j < height; ++j) {
			vg_set_color_pixel(x + i, y + j,
					*(pixmap + (i + j * width) * (bits_per_pixel / 8)));
		}
	}
	return 0;
}

int vg_draw_sprite(Sprite* s) {

	unsigned int i;
	unsigned int j;
	for (i = 0; i < s->width; ++i) {
		for (j = 0; j < s->height; ++j) {
			vg_set_color_pixel(s->x + i, s->y + j, *(s->map + j*s->width + i));
		}
	}

	return 0;
}

int vg_clear_sprite(Sprite* s) {
	unsigned int i;
	unsigned int j;
	for (i = 0; i < s->width; ++i) {
		for (j = 0; j < s->height; ++j) {
			vg_set_color_pixel(s->x + i, s->y + j, 0);
		}
	}
	return 0;
}

int vg_draw_square(unsigned short x , unsigned short y ,unsigned short size , unsigned long color){

	unsigned short xi = x + h_res/2 - size/2;
	unsigned short yi = y + v_res/2 - size/2;

	unsigned int i, j;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			vg_set_color_pixel(xi + j, yi + i, color);
		}
	}

	return 0;


}






