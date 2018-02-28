/**
 *  @author Henrique Ferrolho
 *  http://difusal.blogspot.pt/2014/09/minixtutorial-8-loading-bmp-images.html
 */

#define TRANSPARENT_COLOR  rgb(0,255,0)

/** @defgroup Bitmap Bitmap
 * @{
 * Functions for manipulating bitmaps
 */
typedef enum {
    ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
} Alignment;

typedef struct {
    unsigned short type; // specifies the file type
    unsigned int size; // specifies the size in bytes of the bitmap file
    unsigned int reserved; // reserved; must be 0
    unsigned int offset; // specifies the offset in bytes from the bitmapfileheader to the bitmap bits
} BitmapFileHeader;

typedef struct {
    unsigned int size; // specifies the number of bytes required by the struct
    int width; // specifies width in pixels
    int height; // specifies height in pixels
    unsigned short planes; // specifies the number of color planes, must be 1
    unsigned short bits; // specifies the number of bit per pixel
    unsigned int compression; // specifies the type of compression
    unsigned int imageSize; // size of image in bytes
    int xResolution; // number of pixels per meter in x axis
    int yResolution; // number of pixels per meter in y axis
    unsigned int nColors; // number of colors used by the bitmap
    unsigned int importantColors; // number of colors that are important
} BitmapInfoHeader;

/// Represents a Bitmap
typedef struct {
    BitmapInfoHeader bitmapInfoHeader;
    unsigned char* bitmapData;
} Bitmap;


/**
 * @brief Loads a bmp image
 *
 * @param filename Path of the image to load
 * @return Non NULL pointer to the image buffer
 */
Bitmap* loadBitmap(const char* filename);

/**
 * @brief Draws a bitmap without transparent color
 *
 * @param bitmap bitmap to be drawn
 * @param x destiny x coord
 * @param y destiny y coord
 */
void drawBitmap(Bitmap* bitmap, int x, int y);

/**
 * @brief Draws a bitmap without transparent color and sets the relative coordenates values in the collision mask to 1
 *
 * @param bitmap bitmap to be drawn
 * @param x destiny x coord
 * @param y destiny y coord
 */
void drawBitmapCollision(Bitmap* bmp, int x, int y);

/**
 * @brief Draws a bitmap without transparent color and tests the collision of the object
 *
 * @param bitmap bitmap to be drawn
 * @param x destiny x coord
 * @param y destiny y coord
 *
 * @return returns 0 if there was no collision and returns 1 if there was collision
 */
int drawBitmapTestCollision(Bitmap* bmp, int x, int y);

/**
 * @brief Destroys the given bitmap, freeing all resources used by it.
 *
 * @param bitmap bitmap to be destroyed
 */
void deleteBitmap(Bitmap* bmp);

/**@}*/
