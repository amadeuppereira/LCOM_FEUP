#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include "vbe.h"

#define RGB_MAX				(1 << 8)

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

/**
 * @brief Sets a given pixel to a given color
 * @param x pixel x coord
 * @param y pixel y coord
 * @paramm color color
 */
void vg_set_color_pixel(unsigned short x, unsigned short y, unsigned long color);

/**
 * @brief Draws a squares at a given position, with a given size and a given color
 * @param x square center x coord
 * @param y square center y coord
 * @param size square side size
 * @param color color
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_square(unsigned short x , unsigned short y ,unsigned short size , unsigned long color);

/**
 * @brief Draws a line between two points with given color
 * @param xi beginning point x coord
 * @param yi beginning point y coord
 * @param xf end point x coord
 * @param yf end point y coord
 * @param color color
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_line(unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color);

/**
 * @brief Switches a given set of coordinates
 * @param xi point1 x coord
 * @param yi point1 y coord
 * @param xf point2 x coord
 * @param yf point2 y coord
 */
void trocaValores(unsigned short *xi, unsigned short *yi, unsigned short *xf, unsigned short *yf);

/**
 * @brief Draws a given xpm at a given position
 * @param xpm xpm
 * @param x x coord
 * @param y y coord
 * @return 0 upon success, non-zero upon failure
 */
int vg_draw_xpm(char *xpm[] ,unsigned short x ,unsigned short y);

/**
 * @brief Copies the data in the double buffer to the video_mem
 */
void bufferSwap();

/**
 * @brief Returns the double buffer pointer
 */
void *get_double_buffer();

/**
 * @brief Return the horizontal screen resolution, in pixel
 * @return Horizontal resolution
 */
int get_x_res();

/**
 * @brief Return the vertical screen resolution, in pixel
 * @return Vertical resolution
 */
int get_y_res();

/**
 * @brief Converts an rgb value to uint16_t
 * @param r red component
 * @param g green component
 * @param b blue component
 * @returns uint16_t equivalent of given rgb
 */
uint16_t rgb(int r, int g, int b);

 /** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
