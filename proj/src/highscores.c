#include "highscores.h"



int start_game_read_highscores(FILE* scores) {

	fseek(scores, 0, SEEK_END);

	if (0 == ftell(scores)) {
		return 1;
	}

	fseek(scores, 0, SEEK_SET);

	first = malloc(sizeof(score_t));

	if (first == NULL) {
		return 1;
	}

	fscanf(scores, "%d\t%d\t%d\t%d\t%d\t%d\t%d", &first->score, &first->time[0],
			&first->time[1], &first->time[2], &first->date[0], &first->date[1],
			&first->date[2]);

	if ((feof(scores))) {
		return 1;
	}

	second = malloc(sizeof(score_t));

	if (second == NULL) {
		return 1;
	}

	fscanf(scores, "%d\t%d\t%d\t%d\t%d\t%d\t%d", &second->score,
			&second->time[0], &second->time[1], &second->time[2],
			&second->date[0], &second->date[1], &second->date[2]);

	if ((feof(scores))) {
		return 1;
	}

	third = malloc(sizeof(score_t));

	if (third == NULL) {
		return 1;
	}

	fscanf(scores, "%d\t%d\t%d\t%d\t%d\t%d\t%d", &third->score, &third->time[0],
			&third->time[1], &third->time[2], &third->date[0], &third->date[1],
			&third->date[2]);

	return 0;

}

void game_write_highscores(FILE* scores){

//	fseek(scores, 0, SEEK_END);
//
//	if (0 == ftell(scores)) {
//		return ;
//	}
//	fseek(scores, 0, SEEK_SET);

	if (first == NULL) {
		return ;
	}

	fprintf(scores, "%d\t%d\t%d\t%d\t%d\t%d\t%d\n", first->score, first->time[0],
			first->time[1], first->time[2], first->date[0], first->date[1],
			first->date[2]);

	if (second == NULL) {
		return ;
	}

	fprintf(scores, "%d\t%d\t%d\t%d\t%d\t%d\t%d\n", second->score,
			second->time[0],second->time[1], second->time[2],
			second->date[0], second->date[1], second->date[2]);

	if (third == NULL) {
		return ;
	}


	fprintf(scores, "%d\t%d\t%d\t%d\t%d\t%d\t%d", third->score, third->time[0],
			third->time[1], third->time[2], third->date[0], third->date[1],
			third->date[2]);

}

void destroy_highscores() {
	if(first != NULL) {
		free(first);
	}

	if(second != NULL) {
		free(second);
	}

	if(third != NULL) {
		free(third);
	}
}
