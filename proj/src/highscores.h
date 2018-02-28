#ifndef _SCORES_H_
#define _SCORES_H_

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <sys/mman.h>
#include <sys/types.h>

/** @defgroup Highscores Highscores
 * @{
 * Functions for manipulating highscores
 */

///Represents an highscore
typedef struct {

	int time[3];
	int date[3];
	int score;

}score_t;

score_t * first; ///< best highscore
score_t * second; ///< second best highscore
score_t * third; ///< third best highscore

/**
 * @brief Reads the highscores from a file, creating the variables
 *
 * @param scores file to read from
 *
 * @return 0 if success, 1 otherwise
 */
int start_game_read_highscores(FILE* scores);

/**
 * @brief Write the highscores into a file
 *
 * @param scores file to write
 */
void game_write_highscores(FILE* scores);

/**
 * @brief destroy all highscores, freeing all resources used by them
 */
void destroy_highscores();

/**@}*/

#endif
