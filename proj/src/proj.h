#ifndef _PROJ_H_
#define _PROJ_H_

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include <minix/sysutil.h>

#include "objects.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "vbe.h"
#include "lmlib.h"
#include "video_gr.h"
#include "i8254.h"
#include "i8042.h"
#include "input.h"
#include "collisions.h"
#include "highscores.h"
#include "rtc.h"


#define MAX_ASTEROIDS 100
#define MAX_BULLETS 400

#define Z_CD 	60*25 // 25 sec
#define Z_TIME 	60*5  //5 sec

Bitmap* number_0;
Bitmap* number_1;
Bitmap* number_2;
Bitmap* number_3;
Bitmap* number_4;
Bitmap* number_5;
Bitmap* number_6;
Bitmap* number_7;
Bitmap* number_8;
Bitmap* number_9;

Bitmap* doisPontos;
Bitmap* barra;

/** @name States of the game */
/** @{ */
typedef enum {
	MAIN_MENU,
	TIP,
	PLAY,
	PLAY_INVULNERABLE,
	GAMEOVER,
	END,
	SCORES
} game_state_t;
/** @} */

///Represents a game
typedef struct {
	game_state_t state;
	int score;

	int z_timer;
	int z_cd;

	Asteroid* asteroids[MAX_ASTEROIDS];
	int asteroidTimer;
	int asteroidCounter;

	Bullet* bullets[MAX_BULLETS];
	int bulletCounter;
	int bulletTimer;

	Spaceship* ship;
	Background * space;
	Main_menu * menu;
	Gameover * gameover;
	Tip* tip;
	Mouse_menu * mouse;
	Main_menu_options * menu_op;
	Scores * scores;

	Bitmap* asteroidBitmapSmall;
	Bitmap* asteroidBitmapMedium;
	Bitmap* asteroidBitmapLarge;

	Bitmap* bullet;

} Game_t;

/**
 * @brief Destroys the global game, freeing all resources used by it.
 */
void destroyGame();

/**
 * @brief Starts the project
 * @return 0 if success, 1 if an error occurred
 */
int p_start();

/**
 * @brief Exits the project
 * @return 0 if success, 1 if an error occurred
 */
int p_exit();

/**
 * @brief Starts the game
 * @return 0 if success, 1 if an error occurred
 */
int start_game();

/**
 * @brief Creates a new game, setting it to the global game pointer
 */
void createGame();

/**
 * @brief Resets the global game
 */
void resetGame();

/**
 * @brief State machine that changes the game state based on the given event
 * @param in event
 */
void updateState(Event in);

/**
 * @brief Mouse interruption handler
 * @param packet packet given by the mouse interrupt
 * @return Returns 0 if success, non-zero otherwise
 */
int mouse_handler(unsigned long packet[3]);

/**
 * @brief Keyboard interruption handler
 * @param key scancode
 * @param twoByte flag that tells if is a two byte scancode
 * @return Returns 0 if success, non-zero otherwise
 */
int keyboard_handler(unsigned long key , int twoByte);

/**
 * @brief Timer interruption handler
 * @return Returns 0 if success, non-zero otherwise
 */
int timer_handler();

/**
 * @brief Draws the menu
 */
void drawMenu();

/**
 * @brief Draws the gameover object
 */
void drawGameOver();

/**
 * @brief Draws the tip object
 */
void drawGameTip();

/**
 * @brief Draws the playing game
 */
void drawGame();

/**
 * @brief Updates the playing game
 */
void updateGame();

/**
 * @brief Removes a given asteroid
 * @param i index from the asteroid in the global game variable
 */
void gameRemoveAsteroid(int i);

/**
 * @brief Removes a given bullet
 * @param i index from the bullet in the global game variable
 */
void gameRemoveBullet(int i);

/**
 * @brief Finds the closest asteroid to a set of coordinates
 * @param x x coord
 * @param y y cood
 * @return Returns the asteroid index in the global game variable, returns -1 if the was no asteroid
 */
int gameFindAsteroidHit(int x, int y);

/**
 * @brief Updates an given asteroid
 * @param i index from the asteroid in the global game variable
 */
void asteroidHit(int i);

/**
 * @brief Draws the score
 */
void drawScore(int score);

/**
 * @brief Loads all bitmaps non related to playing the game
 */
void loadBMPs();

/**
 * @brief Destroys all bitmaps non related to playing the game, freeing all resources used by them
 */
void destroyBMPs();

/**
 * @brief Checks if a given mouse is pointing to a given bitmap at a given set of coordinates
 * @param mouse mouse
 * @param bmp bitmap
 * @param x bmp x coord
 * @param y bmp y coord
 * @return Return 1 if pointing, 0 otherwise
 */
int check_if_is_pointing (Mouse_menu * mouse , Bitmap* bmp , int x,int y);

/**
 * @brief Reads the highscores file
 */
void read_file_HS();

/**
 * @brief Writes the highscores file
 */
void write_file_HS();

/**
 * @brief Draws all the highscores
 */
void drawHighScores();

/**
 * @brief Updates all the highscores
 */
void updateHighScores();

#endif
