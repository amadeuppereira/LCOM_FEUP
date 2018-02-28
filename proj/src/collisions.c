#include "collisions.h"


void resetMask() {
	unsigned int i, j;
	for(i = 0; i < 600; i++) {
		for(j = 0; j < 100; j++) {
			mask[i][j] = 0;
		}
	}
}

void mask_setValue(int x, int y) {

	if(x < 0 || y < 0 || x >= 800 || y >= 600) {
		return;
	}

	int i = y;
	int j = x/8;

	int temp = x - j*8 - 1;
	mask[i][j] |= BIT(temp);
}

int mask_getValue(int x, int y) {

	int i = y;
	int j = x/8;
	int temp = x - j*8 - 1;

	return mask[i][j] & BIT(temp);
}
