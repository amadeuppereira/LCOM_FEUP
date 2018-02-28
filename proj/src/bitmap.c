#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "bitmap.h"
#include "vbe.h"
#include "stdio.h"
#include "video_gr.h"
#include "lmlib.h"
#include "collisions.h"

 Bitmap* loadBitmap(const char* filename) {
    // allocating necessary size
    Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

    // open filename in read binary mode
    FILE *filePtr;
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
        return NULL;

    // read the bitmap file header
    BitmapFileHeader bitmapFileHeader;
    fread(&bitmapFileHeader, 2, 1, filePtr);

    // verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.type != 0x4D42) {
        fclose(filePtr);
        return NULL;
    }

    int rd;
    do {
        if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
            break;
    } while (0);

    if (rd != 1) {
        fprintf(stderr, "Error reading file\n");
        exit(-1);
    }

    // read the bitmap info header
    BitmapInfoHeader bitmapInfoHeader;
    fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

    // move file pointer to the begining of bitmap data
    fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

    // allocate enough memory for the bitmap image data
    unsigned char* bitmapImage = (unsigned char*) malloc(
            bitmapInfoHeader.imageSize);

    // verify memory allocation
    if (!bitmapImage) {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

    // make sure bitmap image data was read
    if (bitmapImage == NULL) {
        fclose(filePtr);
        return NULL;
    }

    // close file and return bitmap image data
    fclose(filePtr);

    bmp->bitmapData = bitmapImage;
    bmp->bitmapInfoHeader = bitmapInfoHeader;

    return bmp;
}


void drawBitmap(Bitmap* bmp, int x, int y) {

    if (bmp == NULL) {
        return;
    }

    int width = bmp->bitmapInfoHeader.width;
    int height = bmp->bitmapInfoHeader.height;


	char* bufferStartPos;
	uint16_t* bufferStartPos16;
	unsigned char* imgStartPos;
	uint16_t *imgStartPos16;

	unsigned int i;
	for (i = 0; i < height; i++) {
		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= get_y_res())
			continue;

		bufferStartPos = (char*)get_double_buffer();
		bufferStartPos += x * 2 + pos * get_x_res() * 2;
		bufferStartPos16 = (uint16_t*) bufferStartPos;

		imgStartPos = bmp->bitmapData + i * width * 2;
		imgStartPos16 = (uint16_t *)imgStartPos;


		unsigned int j;
		for(j = 0; j < width; j++) {
			if(*(imgStartPos16) != TRANSPARENT_COLOR) {
				*(bufferStartPos16) = *(imgStartPos16);
			}
			bufferStartPos16++;
			imgStartPos16++;
		}

	}
}

void drawBitmapCollision(Bitmap* bmp, int x, int y) {

    if (bmp == NULL) {
        return;
    }

    int width = bmp->bitmapInfoHeader.width;
    int height = bmp->bitmapInfoHeader.height;


	char* bufferStartPos;
	uint16_t* bufferStartPos16;
	unsigned char* imgStartPos;
	uint16_t *imgStartPos16;

	unsigned int i;
	for (i = 0; i < height; i++) {
		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= get_y_res())
			continue;

		bufferStartPos = (char*)get_double_buffer();
		bufferStartPos += x * 2 + pos * get_x_res() * 2;
		bufferStartPos16 = (uint16_t*) bufferStartPos;

		imgStartPos = bmp->bitmapData + i * width * 2;
		imgStartPos16 = (uint16_t *)imgStartPos;


		unsigned int j;
		for(j = 0; j < width; j++) {
			if(*(imgStartPos16) != TRANSPARENT_COLOR) {
				*(bufferStartPos16) = *(imgStartPos16);
				mask_setValue(x+j, y+i);
			}
			bufferStartPos16++;
			imgStartPos16++;
		}

	}
}

int drawBitmapTestCollision(Bitmap* bmp, int x, int y) {

	if(bmp == NULL) {
		return -1;
	}

	int collision = 0;

    int width = bmp->bitmapInfoHeader.width;
    int height = bmp->bitmapInfoHeader.height;


	char* bufferStartPos;
	uint16_t* bufferStartPos16;
	unsigned char* imgStartPos;
	uint16_t *imgStartPos16;

	unsigned int i;
	for (i = 0; i < height; i++) {
		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= get_y_res())
			continue;

		bufferStartPos = (char*)get_double_buffer();
		bufferStartPos += x * 2 + pos * get_x_res() * 2;
		bufferStartPos16 = (uint16_t*) bufferStartPos;

		imgStartPos = bmp->bitmapData + i * width * 2;
		imgStartPos16 = (uint16_t *)imgStartPos;


		unsigned int j;
		for(j = 0; j < width; j++) {
			if(*(imgStartPos16) != TRANSPARENT_COLOR) {
				*(bufferStartPos16) = *(imgStartPos16);
				if(mask_getValue(x+j, y+i)){
					collision = 1;
				}
			}
			bufferStartPos16++;
			imgStartPos16++;
		}

	}

	return collision;
}


void deleteBitmap(Bitmap* bmp) {
    if (bmp == NULL)
        return;

    free(bmp->bitmapData);
    free(bmp);
}
