#ifndef _COLLISIONS_H_
#define _COLLISIONS_H_

#define BIT(n) (0x01<<(n))

char mask[600][100]; ///< collision mask(100x600 bytes, using every bit)

/**
 * @brief reset collision mask, setting all values to 0
 */
void resetMask();

/**
 * @brief set a collision mask coordinate value to 1
 *
 * @param x x coord
 * @param y y coord
 */
void mask_setValue(int x, int y);

/**
 * @brief return the value of a coordinate from the collision mask
 *
 * @param x x coord
 * @param y y coord
 *
 * @return 1 if there is an object at those coordinates, 0 otherwise
 */
int mask_getValue(int x, int y);

#endif
