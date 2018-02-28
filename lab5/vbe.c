#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {

	mmap_t mapa;

	if (lm_init() == NULL) {
		printf("\tvbe_get_mode_info() : lm_init() has failed !");
		return 1;
	}

	lm_alloc(sizeof(vbe_mode_info_t), &mapa);

	struct reg86u reg86;

	reg86.u.b.intno = VBE_VIDEOCARD; /* BIOS video services */
	reg86.u.b.ah = VBE_FUNCTION; /* Set Video Mode function */
	reg86.u.b.al = VBE_RETURN_MODE_INFO ;
	reg86.u.w.cx = mode | BIT(LINEAR_MODEL_BIT);
	reg86.u.w.es = PB2BASE(mapa.phys);  /*set a segment base*/
	reg86.u.w.di = PB2OFF(mapa.phys);   /*set the offset accordingly*/
	if (sys_int86(&reg86) != OK) {
		printf("\tvbe_get_mode_info(): sys_int86() failed \n");
		return 1;
	} else {
		if (reg86.u.b.ah == VBE_FUNCTION_CALL_FAILED||reg86.u.b.ah ==  VBE_FUNCTION_NOT_SUPPORTED||reg86.u.b.ah ==  VBE_FUNCTION_INVALID_IN_CURRENT_VM) {
			lm_free(&mapa);
			return 1;

		}
	}
	*vmi_p = *((vbe_mode_info_t *)mapa.virtual);
	lm_free(&mapa);

	return 0;
}






















