#ifndef SPRITE_H
#define SPRITE_H

#include "vbe.h"



typedef struct {
int x, y; // current position
int width, height;  // dimensions
int xspeed, yspeed; // current speed
char* map;          // the pixmap
} Sprite;

Sprite * create_sprite(char *pic[], int x, int y, int xspeed, int yspeed, vbe_mode_info_t modeInfo);
void destroy_sprite(Sprite *sp);

#endif
