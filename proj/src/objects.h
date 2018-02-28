#ifndef _OBJ_H_
#define _OBJ_H_

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "bitmap.h"

#define ASTEROID_RATE 		20
#define ASTEROID_MIN_SPEED 	2
#define ASTEROID_MAX_SPEED 	5
#define BULLET_SPEED 		5
#define BULLET_RATE			10

extern int asteroidRate; ///< asteroid spawn rate
extern int asteroidMinSpeed; ///< asteroid minimum speed
extern int asteroidMaxSpeed; ///< asteroid maximum speed

extern int bulletSpeed; ///< bullets speed
extern int bulletRate; ///< bullets spawn rate

/** @defgroup Object Object
 * @{
 * Functions for manipulating objects
 */

///Represents an asteroid
typedef struct {
	int x, y;
	int ySpeed;
	Bitmap* bmp;
	int hp; // 1 hit para o small , 2 hits para o medium , 3 hits para o large
} Asteroid;

///Represents a spaceship
typedef struct {
	int x, y;
	Bitmap* bmp;
	Bitmap* shield;
} Spaceship;

///Represents a bullet
typedef struct {
	int x, y;
	int ySpeed;
	Bitmap * bmp;
} Bullet;

///Represents the background image
typedef struct {
	Bitmap * bmp;
} Background;

///Represents the main menu image
typedef struct {
	Bitmap * bmp;
} Main_menu;

///Represents the gameover image
typedef struct {
	Bitmap * bmp;
}Gameover;

///Represents the tip image
typedef struct {
	Bitmap * bmp;
}Tip;

///Represents the scores image
typedef struct {
	Bitmap * bmp;
}Scores;

///Represents the main menu mouse
typedef struct {
	Bitmap* bmp;
	int x ,y;
}Mouse_menu;

///Represents the main menu options with their possible variables (if selected or not)
typedef struct {
	Bitmap * bmp_play;
	Bitmap * bmp_play_selected;
	int play_selected;
	Bitmap * bmp_exit;
	Bitmap * bmp_exit_selected;
	int exit_selected;
	Bitmap * bmp_highscores;
	Bitmap * bmp_highscores_selected;
	int highscores_selected;
	Bitmap * bmp_multiplayer;
	Bitmap * bmp_multiplayer_selected;
	int multiplayer_selected;
}Main_menu_options;


/**
 * @brief Creates a spaceship object
 * @return Non NULL pointer to the spaceship object
 */
Spaceship* createSpaceship();

/**
 * @brief Destroys the given spaceship, freeing all resources used by it
 * @param spc spaceship to be destroyed
 */
void destroySpaceship(Spaceship* spc);

/**
 * @brief Draws a spaceship, checking if there was any collision
 * @param spc spaceship to draw
 * @return 1 if there was a collision, 0 otherwise
 */
int drawSpaceship(Spaceship* spc);

/**
 * @brief Draws a spaceship with the shield, checking if there was any collision
 * @param spc spaceship to draw
 * @return 1 if there was a collision, 0 otherwise
 */
int drawSpaceshipShield(Spaceship* spc);

/**
 * @brief Creates an asteroid object with a given health and a given bitmap
 * @param asteroidBitmap pointer to the bitmap wanted to be used
 * @param hp health points
 * @return Non NULL pointer to the asteroid object
 */
Asteroid* createAsteroid(Bitmap* asteroidBitmap, int hp);

/**
 * @brief Destroys the given asteroid, freeing all resources used by it
 * @param ast asteroid to be destroyed
 */
void destroyAsteroid(Asteroid * ast);

/**
 * @brief Draws an asteroids
 * @param ast asteroid to draw
 */
void drawAsteroid(Asteroid * ast);

/**
 * @brief Creates a background object
 * @return Non NULL pointer to the background object
 */
Background * createBackground();

/**
 * @brief Creates a main menu object
 * @return Non NULL pointer to the main menu object
 */
Main_menu * createMainMenu();

/**
 * @brief Creates a gameover object
 * @return Non NULL pointer to the gameover object
 */
Gameover* createGameover();

/**
 * @brief Destroys the given gameover object, freeing all resources used by it
 * @param go object to be destroyed
 */
void destroyGameover(Gameover* go);

/**
 * @brief Draws a gameover object
 * @param go object to draw
 */
void drawGameover(Gameover* go);

/**
 * @brief Creates a tip object
 * @return Non NULL pointer to the tip object
 */
Tip* createTip();

/**
 * @brief Destroys the given tip object, freeing all resources used by it
 * @param t object to be destroyed
 */
void destroyTip(Tip* t);

/**
 * @brief Draws a tip object
 * @param t object to draw
 */
void drawTip(Tip* t);

/**
 * @brief Destroys the given background object, freeing all resources used by it
 * @param b object to be destroyed
 */
void destroyBackground(Background * b);

/**
 * @brief Draws a background object
 * @param bg object to draw
 */
void drawBackground(Background * bg);

/**
 * @brief Destroys the given main menu object, freeing all resources used by it
 * @param b object to be destroyed
 */
void destroyMainMenu(Main_menu * b);

/**
 * @brief Draws a main menu object
 * @param menu object to draw
 */
void drawMainMenu(Main_menu * menu);

/**
 * @brief Creates a bullet, with a given bitmap and a set of coordinates
 * @param bmp bitmap points
 * @param x x coord
 * @param y y coord
 * @return Non NULL pointer to the bullet
 */
Bullet* createBullet(Bitmap* bmp, int x, int y);

/**
 * @brief Destroys the given bullet, freeing all resources used by it
 * @param b bullet to be destroyed
 */
void destroyBullet(Bullet* b);

/**
 * @brief Draws a bullet, checking if there was any collision
 * @param b bullet to draw
 * @return 1 if there was a collision, 0 otherwise
 */
int drawBullet(Bullet *b);

/**
 * @brief Updates the global variables based on the score
 * @param score game score
 */
void updateDifficulty(int score);

/**
 * @brief Creates a mouse
 * @return Non NULL pointer to the mouse
 */
Mouse_menu * createMouseMenu();

/**
 * @brief Destroys the given mouse, freeing all resources used by it
 * @param mouse mouse to be destroyed
 */
void destroyMouseMenu(Mouse_menu * mouse);

/**
 * @brief Draws a mouse
 * @param mouse mouse to draw
 */
void drawMouseMenu(Mouse_menu * mouse);

/**
 * @brief Creates a menu options object
 * @return Non NULL pointer to the menu options object
 */
Main_menu_options * createMenuOptions();

/**
 * @brief Destroys the given menu options object, freeing all resources used by it
 * @param menu_op menu options object to be destroyed
 */
void destroyMenuOptions(Main_menu_options * menu_op);

/**
 * @brief Draws a menu options object
 * @param menu_op menu options object to draw
 */
void drawMenuOptions(Main_menu_options * menu_op);

/**
 * @brief Creates a scores object
 * @return Non NULL pointer to the scores object
 */
Scores * createScores();

/**
 * @brief Destroys the given scores object, freeing all resources used by it
 * @param s scores object to be destroyed
 */
void destroyScores(Scores * s);

/**
 * @brief Draws a scores object
 * @param s scores objects to draw
 */
void drawScores(Scores * s);

/**@}*/

#endif
