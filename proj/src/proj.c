#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "proj.h"

Game_t * gamePtr; ///< Global game pointer

int main(int argc, char **argv) {

	sef_startup();

	sys_enable_iop(SELF);

	if(p_start()) {
		p_exit();
		printf("Fugix:: Error starting the program!\n");
		return 1;
	}

	p_exit();

	return 0;
}

int	p_start() {

	vg_init(VBE_MODE_800_600_64K);

	loadBMPs();

	read_file_HS();

	start_game();

	return 0;
}

int p_exit() {

	destroyBMPs();

	write_file_HS();

	destroy_highscores();

	return vg_exit();

	return 0;
}

void createGame() {

	gamePtr = (Game_t*) malloc(sizeof(Game_t));
	gamePtr->state = MAIN_MENU;
	gamePtr->score = 0;
	gamePtr->z_cd = 0;
	gamePtr->z_timer = 0;
	gamePtr->menu_op = createMenuOptions();
	gamePtr->ship = createSpaceship();
	gamePtr->space = createBackground();
	gamePtr-> mouse = createMouseMenu();
	gamePtr->menu = createMainMenu();
	gamePtr->gameover = createGameover();
	gamePtr->tip = createTip();
	gamePtr->scores = createScores();
	gamePtr->ship->x = get_x_res() / 2
			- gamePtr->ship->bmp->bitmapInfoHeader.width / 2;
	gamePtr->ship->y = get_y_res() - 80;
	gamePtr->asteroidTimer = asteroidRate;
	gamePtr->asteroidCounter = 0;
	gamePtr->bulletTimer = 0;
	gamePtr->bulletCounter = 0;
	gamePtr->asteroidBitmapSmall = loadBitmap(
			"/home/lcom/svn/proj/src/images/asteroid.bmp");
	gamePtr->asteroidBitmapMedium = loadBitmap(
			"/home/lcom/svn/proj/src/images/asteroid2.bmp");
	gamePtr->asteroidBitmapLarge = loadBitmap(
			"/home/lcom/svn/proj/src/images/asteroid3.bmp");
	gamePtr->bullet = loadBitmap("/home/lcom/svn/proj/src/images/bullet.bmp");


}

void resetGame() {

	unsigned int i;

	//removesAsteroids
	for(i = 0; i < gamePtr->asteroidCounter; i++) {
		destroyAsteroid(gamePtr->asteroids[i]);
	}

	//removes bullets
	for(i = 0; i < gamePtr->bulletCounter; i++) {
		destroyBullet(gamePtr->bullets[i]);
	}


	gamePtr->score = 0;
	gamePtr->z_cd = 0;
	gamePtr->z_timer = 0;
	gamePtr->ship->x = get_x_res() / 2 - gamePtr->ship->bmp->bitmapInfoHeader.width / 2;
	gamePtr->ship->y = get_y_res() - 80;
	gamePtr->asteroidTimer = asteroidRate;
	gamePtr->asteroidCounter = 0;
	gamePtr->bulletTimer = 0;
	gamePtr->bulletCounter = 0;

}

int start_game() {

	createGame();


	int timer_int_cnt = 0;

	unsigned long scancode;
	int two_byte = 0;

	unsigned long packet[3];
	int packet_counter = 0;
	unsigned long byte;


	int ipc_status;
	message msg;
	int irq_set_timer = BIT(INTERRUPT_BIT), irq_set_mouse = BIT(
			INTERRUPT_BIT_MOUSE), irq_set_kbd = BIT(INTERRUPT_BIT_KBD), r;

	if (timer_subscribe_int() != INTERRUPT_BIT) {
		printf("TIMER subscribe error!\n");
		return 1;
	}

	if (mouse_subscribe_int() != INTERRUPT_BIT_MOUSE) {
		printf("MOUSE subscribe error!\n");
		return 1;
	}

	if (kbd_subscribe_int() != INTERRUPT_BIT_KBD) {
		printf("KEYBOARD subscribe error!\n");
		return 1;
	}

	if (mouse_set_stream_mode() != 0)  // enable stream mode and data report
		return 1;

	if (mouse_enable_data() != 0)
		return 1;

	while (gamePtr->state != END) {

		/* Get a reqest message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set_mouse) {
					if (read_outbuf(&byte) != 0) {
						printf("Error reading from out buffer!\n");
						return 1;
					}

					if (((byte & BIT(3)) && (packet_counter == 0))
							|| (packet_counter > 0)) {
						packet[packet_counter] = byte;
						packet_counter++;
					}
					if (packet_counter == 3) {
						packet_counter = 0;
						mouse_handler(packet);
					}
				}

				if (msg.NOTIFY_ARG & irq_set_kbd) {
					kbd_read_scancode(&scancode);
					if (scancode == TWOBYTE_SCANCODE) {
						two_byte = 1;
					} else {
						keyboard_handler(scancode, two_byte);
						two_byte = 0;
					}
				}

				if (msg.NOTIFY_ARG & irq_set_timer) { /* subscribed interrupt */
					timer_handler();
					timer_int_cnt++;
					if(timer_int_cnt % 60 == 0) {
						if (gamePtr->state == PLAY || gamePtr->state == PLAY_INVULNERABLE){
							gamePtr->score += 1;
						}
						timer_int_cnt = 0;
					}


				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}

		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}


	if (mouse_disable_data() != 0) {
		printf("Error disabling mouse");
		return 1;
	}

	if (kbd_unsubscribe_int() != 0) {
		printf("KEYBOARD failed unsubscribe\n");
		return 1;
	}

	if (timer_unsubscribe_int() != 0) {
		printf("TIMER failed unsubscribe\n");
		return 1;
	}

	if (mouse_unsubscribe_int() != 0) {
		printf("MOUSE failed unsubscribe\n");
		return 1;
	}
	return 0;
}

void updateState(Event in) {

	switch(gamePtr->state) {
	case MAIN_MENU:
		if(in.kbd.ENTER || in.play) {
			gamePtr->state = TIP;
		}
		if(in.kbd.ESC || in.exit) {
			gamePtr->state = END;
		}
		if(in.scores) {
			gamePtr->state = SCORES;
		}
		break;
	case TIP:
		if(in.kbd.ENTER) {
			gamePtr->state = PLAY;
		}
	case PLAY_INVULNERABLE:
		if(in.z_time) {
			gamePtr->state = PLAY;
		}
		break;
	case PLAY:
		if(in.kbd.Z && gamePtr->z_cd == 0) {
			gamePtr->state = PLAY_INVULNERABLE;
			gamePtr->z_timer = Z_TIME;
			gamePtr->z_cd = Z_CD;
		}
		if(in.gameover) {
			gamePtr->state = GAMEOVER;
			updateHighScores();
		}
		break;
	case GAMEOVER:
		if(in.kbd.ESC) {
			gamePtr->state = MAIN_MENU;
			resetGame();
		}
		else if(in.kbd.ENTER) {
			gamePtr->state = PLAY;
			resetGame();
		}
	case SCORES:
		if (in.kbd.ESC || in.kbd.ENTER) {
			gamePtr->state = MAIN_MENU;
		}
		break;

	default:
		break;
	}
}

int keyboard_handler(unsigned long key , int twoByte){

	KBD_Input kbd_input = handler_kbd_event(key, twoByte);
	Event in = createEvent();
	in.kbd = kbd_input;

	updateState(in);

	return 0;

}

int mouse_handler(unsigned long packet[3]){

	Mouse_Input mouse_input = handler_mouse_event(packet);
	Event in = createEvent();


	switch (gamePtr->state) {
	case MAIN_MENU:


				gamePtr->mouse->x += mouse_input.x;
				gamePtr->mouse->y -= mouse_input.y;

				if (gamePtr->mouse->x < 0) {
					gamePtr->mouse->x = 0;
				} else if (gamePtr->mouse->x + gamePtr->mouse->bmp->bitmapInfoHeader.width
						> get_x_res()) {
					gamePtr->mouse->x = get_x_res()
							- gamePtr->mouse->bmp->bitmapInfoHeader.width;
				}

				if (gamePtr->mouse->y < 0) {
					gamePtr->mouse->y = 0;
				} else if (gamePtr->mouse->y
						+ gamePtr->mouse->bmp->bitmapInfoHeader.height > get_y_res()) {
					gamePtr->mouse->y = get_y_res()
							- gamePtr->mouse->bmp->bitmapInfoHeader.height;
				}

				if (check_if_is_pointing(gamePtr->mouse,gamePtr->menu_op->bmp_play, (800 - gamePtr->menu_op->bmp_play->bitmapInfoHeader.width) / 2 , (600 - gamePtr->menu_op->bmp_play->bitmapInfoHeader.height) / 2)){

					gamePtr->menu_op->play_selected = 1;

					if (mouse_input.LB){
						in.play = 1;
					}

				}
				else {
					gamePtr->menu_op->play_selected = 0;
				}

				if (check_if_is_pointing(gamePtr->mouse,gamePtr->menu_op->bmp_multiplayer, (800 - gamePtr->menu_op->bmp_multiplayer->bitmapInfoHeader.width) / 2 , (750 - gamePtr->menu_op->bmp_multiplayer->bitmapInfoHeader.height) / 2 )){

					gamePtr->menu_op->multiplayer_selected = 1;
				}

				else {
					gamePtr->menu_op->multiplayer_selected = 0;
				}

				if (check_if_is_pointing(gamePtr->mouse,gamePtr->menu_op->bmp_highscores, (800 - gamePtr->menu_op->bmp_highscores->bitmapInfoHeader.width) / 2 , (900 - gamePtr->menu_op->bmp_highscores->bitmapInfoHeader.height) / 2)){

					gamePtr->menu_op->highscores_selected = 1;
					if (mouse_input.LB) {
						in.scores = 1;
					}

				}

				else {
					gamePtr->menu_op->highscores_selected = 0;
				}
				if (check_if_is_pointing(gamePtr->mouse,gamePtr->menu_op->bmp_exit, (800 - gamePtr->menu_op->bmp_exit->bitmapInfoHeader.width) / 2 , (1050 - gamePtr->menu_op->bmp_exit->bitmapInfoHeader.height) / 2 )){

					gamePtr->menu_op->exit_selected = 1;
					if (mouse_input.LB) {
						in.exit = 1;
					}

				}

				else {
					gamePtr->menu_op->exit_selected = 0 ;
				}

				updateState(in);
		break;
	case TIP:
		break;
	case END:
		break;
	case SCORES:
		break;

	default:

		gamePtr->ship->x += mouse_input.x;
		gamePtr->ship->y -= mouse_input.y;

		if (gamePtr->ship->x < 0) {
			gamePtr->ship->x = 0;
		} else if (gamePtr->ship->x + gamePtr->ship->bmp->bitmapInfoHeader.width
				> get_x_res()) {
			gamePtr->ship->x = get_x_res()
					- gamePtr->ship->bmp->bitmapInfoHeader.width;
		}

		if (gamePtr->ship->y < 0) {
			gamePtr->ship->y = 0;
		} else if (gamePtr->ship->y
				+ gamePtr->ship->bmp->bitmapInfoHeader.height > get_y_res()) {
			gamePtr->ship->y = get_y_res()
					- gamePtr->ship->bmp->bitmapInfoHeader.height;
		}

		//create bullet
		if (mouse_input.LB) {
			if (gamePtr->bulletTimer == 0) {
				if (gamePtr->bulletCounter < MAX_BULLETS) {
					int idx = gamePtr->bulletCounter;

					gamePtr->bullets[idx] = createBullet(gamePtr->bullet,
							gamePtr->ship->x
									+ gamePtr->ship->bmp->bitmapInfoHeader.width
											/ 2, gamePtr->ship->y);

					gamePtr->bulletCounter += 1;
					gamePtr->bulletTimer = bulletRate;
				}

			}
		}

		break;

	}
	return 0;

}

int timer_handler() {

	switch (gamePtr->state) {

	case MAIN_MENU:
		drawMenu();
		break;
	case TIP:
		drawGameTip();
		break;
	case PLAY:
		drawGame();
		updateGame();
		break;
	case PLAY_INVULNERABLE:
		drawGame();
		updateGame();
		break;
	case GAMEOVER:
		drawGameOver();
		break;
	case END:
		destroyGame(gamePtr);
		break;
	case SCORES:
		drawHighScores();
		break;

	default:
		break;
	}

	bufferSwap();

	return 0;

}


void destroyGame(Game_t * game){

	if (game == NULL){
		return ;
	}

	unsigned int i;

	//removesAsteroids
	for(i = 0; i < game->asteroidCounter; i++) {
		destroyAsteroid(game->asteroids[i]);
	}

		//removes bullets
	for(i = 0; i < game->bulletCounter; i++) {
		destroyBullet(game->bullets[i]);
	}

	destroySpaceship(game->ship);
	destroyBackground(game->space);
	destroyMainMenu(game->menu);
	destroyGameover(game->gameover);
	destroyTip(game->tip);
	deleteBitmap(game->asteroidBitmapSmall);
	deleteBitmap(game->asteroidBitmapMedium);
	deleteBitmap(game->asteroidBitmapLarge);
	deleteBitmap(game->bullet);
	destroyMenuOptions(game->menu_op);
	destroyMouseMenu(game->mouse);
	destroyScores(game->scores);

	free(game);

}

void drawMenu() {

	drawMainMenu(gamePtr->menu);
	drawMenuOptions(gamePtr->menu_op);
	drawMouseMenu(gamePtr->mouse);
}

void drawGameOver() {
	drawBackground(gamePtr->space);
	drawGameover(gamePtr->gameover);
}

void drawGameTip() {
	drawBackground(gamePtr->space);
	drawTip(gamePtr->tip);
}

void drawGame() {

	drawBackground(gamePtr->space);

	resetMask();
	unsigned int i;
	for(i = 0;i < gamePtr->asteroidCounter ;i++) {
		drawAsteroid(gamePtr->asteroids[i]);
	}

	switch(gamePtr->state) {
	case PLAY:
		if(drawSpaceship(gamePtr->ship)){
			Event in = createEvent();
			in.gameover = 1;
			updateState(in);
		}
		break;
	case PLAY_INVULNERABLE:
		if(drawSpaceshipShield(gamePtr->ship)){

			//to test several positions where the collision could have occured
			//left, top, bottom, right
			for(i = 0; i < 4; i++) {
				int x, y;

				switch(i) {
				case 0:
					x = gamePtr->ship->x;
					y = gamePtr->ship->y + gamePtr->ship->bmp->bitmapInfoHeader.height / 2;
					break;
				case 1:
					x = gamePtr->ship->x + gamePtr->ship->bmp->bitmapInfoHeader.width / 2;
					y = gamePtr->ship->y;
					break;
				case 2:
					x = gamePtr->ship->x;
					y = gamePtr->ship->y + gamePtr->ship->bmp->bitmapInfoHeader.height;
					break;
				case 3:
					x = gamePtr->ship->x + gamePtr->ship->bmp->bitmapInfoHeader.width;
					y = gamePtr->ship->y + gamePtr->ship->bmp->bitmapInfoHeader.height / 2;
					break;
				default:
					break;
				}
				int ai = gameFindAsteroidHit(x, y);
				if(ai != -1) {
				asteroidHit(ai);
				}
			}
		}
		break;
	default:
		break;
	}

	for(i = 0;i < gamePtr->bulletCounter ;i++) {
		if(drawBullet(gamePtr->bullets[i])) {
			int ai = gameFindAsteroidHit(gamePtr->bullets[i]->x, gamePtr->bullets[i]->y);
			if (ai != -1){
				gameRemoveBullet(i);
				asteroidHit(ai);
			}
		}
	}

	drawScore(gamePtr->score);

}

void asteroidHit(int i) {
	gamePtr->asteroids[i]->hp -= 1;
	if (gamePtr->asteroids[i]->hp == 0) {
		gameRemoveAsteroid(i);
	} else if (gamePtr->asteroids[i]->hp == 2) {
		gamePtr->asteroids[i]->bmp = gamePtr->asteroidBitmapMedium;
	} else if (gamePtr->asteroids[i]->hp == 1) {
		gamePtr->asteroids[i]->bmp = gamePtr->asteroidBitmapSmall;
	}
}

void drawScore(int score) {

	int c, d, u, i;
	c = score / 100;
	d = (score % 100) / 10;
	u = (score % 100) % 10;

	for (i = 0; i < 3; i++) {
		int temp;
		if(i == 0) {
			temp = c;
		}
		else if(i == 1) {
			temp = d;
		}
		else {
			temp = u;
		}

		switch (temp) {
		case 0:
			drawBitmap(number_0, 5 + i*27, 5);
			break;
		case 1:
			drawBitmap(number_1, 5 + i*27, 5);
			break;
		case 2:
			drawBitmap(number_2, 5 + i*27, 5);
			break;
		case 3:
			drawBitmap(number_3, 5 + i*27, 5);
			break;
		case 4:
			drawBitmap(number_4, 5 + i*27, 5);
			break;
		case 5:
			drawBitmap(number_5, 5 + i*27, 5);
			break;
		case 6:
			drawBitmap(number_6, 5 + i*27, 5);
			break;
		case 7:
			drawBitmap(number_7, 5 + i*27, 5);
			break;
		case 8:
			drawBitmap(number_8, 5 + i*27, 5);
			break;
		case 9:
			drawBitmap(number_9, 5 + i*27, 5);
			break;
		default:
			break;
		}
	}
}


void updateGame(){

	updateDifficulty(gamePtr->score);

	unsigned int i;

	for (i = 0; i < gamePtr->asteroidCounter; i++) {
		gamePtr->asteroids[i]->y = gamePtr->asteroids[i]->y + gamePtr->asteroids[i]->ySpeed;
		//removes asteroid
		if (gamePtr->asteroids[i]->y > get_y_res()) {
			gameRemoveAsteroid(i);
		}
	}

	//creates a new asteroid
	if (gamePtr->asteroidTimer == 0) {
		if (gamePtr->asteroidCounter < MAX_ASTEROIDS) {
			int r = rand() % 3;
			int idx = gamePtr->asteroidCounter;

			if (r == 0) {
				gamePtr->asteroids[idx] =
						createAsteroid(gamePtr->asteroidBitmapSmall , 1); // com hp  = 1
			} else if (r == 1) {
				gamePtr->asteroids[idx] =
						createAsteroid(gamePtr->asteroidBitmapMedium , 2); // com hp = 2
			} else {
				gamePtr->asteroids[idx] =
						createAsteroid(gamePtr->asteroidBitmapLarge , 3); // com hp = 3
			}

			gamePtr->asteroidCounter = gamePtr->asteroidCounter + 1;

			gamePtr->asteroidTimer = asteroidRate;
		}
	} else {
		gamePtr->asteroidTimer = gamePtr->asteroidTimer - 1;

	}

	for (i = 0; i < gamePtr->bulletCounter; i++) {
		gamePtr->bullets[i]->y = gamePtr->bullets[i]->y - gamePtr->bullets[i]->ySpeed;
		//removes bullet
		if (gamePtr->bullets[i]->y < 0) {
			gameRemoveBullet(i);
		}
	}

	if(gamePtr->bulletTimer > 0) {
		gamePtr->bulletTimer -= 1;
	}

	if(gamePtr->z_timer > 0) {
		gamePtr->z_timer -= 1;
	}
	else if(gamePtr->z_timer == 0) {
		Event in = createEvent();
		in.z_time = 1;
		updateState(in);
		//gamePtr->state = PLAY;
	}

	if(gamePtr->z_cd > 0) {
		gamePtr->z_cd -= 1;
	}
}


void gameRemoveAsteroid(int i) {
	destroyAsteroid(gamePtr->asteroids[i]);
	memmove(gamePtr->asteroids + i, gamePtr->asteroids + i + 1, MAX_ASTEROIDS - gamePtr->asteroidCounter);
	gamePtr->asteroidCounter -= 1;

}

void gameRemoveBullet(int i) {
	destroyBullet(gamePtr->bullets[i]);
	memmove(gamePtr->bullets + i, gamePtr->bullets + i + 1, MAX_BULLETS - gamePtr->bulletCounter);
	gamePtr->bulletCounter -= 1;

}

int gameFindAsteroidHit(int x, int y) {

	unsigned int i;
	int existe = 0;
	for (i = 0; i < gamePtr->asteroidCounter; i++) {
		if(x >= gamePtr->asteroids[i]->x && x <= (gamePtr->asteroids[i]->x + gamePtr->asteroids[i]->bmp->bitmapInfoHeader.width) && y >= gamePtr->asteroids[i]->y && y <= (gamePtr->asteroids[i]->y + gamePtr->asteroids[i]->bmp->bitmapInfoHeader.height)) {
			existe = 1;
			break;
		}
	}
	if (existe){
		return i;
	}
	else{
		i = -1;
	}

	return i;
}

void loadBMPs(){

	number_0 = loadBitmap("/home/lcom/svn/proj/src/images/numbers/0.bmp");
	number_1 = loadBitmap("/home/lcom/svn/proj/src/images/numbers/1.bmp");
	number_2 = loadBitmap("/home/lcom/svn/proj/src/images/numbers/2.bmp");
	number_3 = loadBitmap("/home/lcom/svn/proj/src/images/numbers/3.bmp");
	number_4 = loadBitmap("/home/lcom/svn/proj/src/images/numbers/4.bmp");
	number_5 = loadBitmap("/home/lcom/svn/proj/src/images/numbers/5.bmp");
	number_6 = loadBitmap("/home/lcom/svn/proj/src/images/numbers/6.bmp");
	number_7 = loadBitmap("/home/lcom/svn/proj/src/images/numbers/7.bmp");
	number_8 = loadBitmap("/home/lcom/svn/proj/src/images/numbers/8.bmp");
	number_9 = loadBitmap("/home/lcom/svn/proj/src/images/numbers/9.bmp");
	doisPontos = loadBitmap("/home/lcom/svn/proj/src/images/dois_pontos.bmp");
	barra = loadBitmap("/home/lcom/svn/proj/src/images/barra.bmp");
}

void destroyBMPs() {
	deleteBitmap(number_0);
	deleteBitmap(number_1);
	deleteBitmap(number_2);
	deleteBitmap(number_3);
	deleteBitmap(number_4);
	deleteBitmap(number_5);
	deleteBitmap(number_6);
	deleteBitmap(number_7);
	deleteBitmap(number_8);
	deleteBitmap(number_9);
	deleteBitmap(doisPontos);
	deleteBitmap(barra);
}

int check_if_is_pointing (Mouse_menu * mouse , Bitmap* bmp , int x,int y){

	int i = x;
	int j = y;
	int isPointing = 0;

	for (; j < (y +  bmp->bitmapInfoHeader.height) ; j++){

		for (; i < (x + bmp->bitmapInfoHeader.width) ; i++){


			if ((mouse->x == i) && (mouse->y == j)){
				isPointing = 1;
				break;
			}
		}
		if (isPointing){
			break;
		}
		i = x;
	}

	if (isPointing){
		return 1;
	}

	else return 0;
}

void read_file_HS(){

	FILE * scores;
	scores = fopen("/home/lcom/svn/proj/highscores.txt" , "r");
	if(scores != NULL) {
		start_game_read_highscores(scores);
		fclose(scores);
	}
}

void write_file_HS() {
	FILE * scores;
	scores = fopen("/home/lcom/svn/proj/highscores.txt" , "w");
	if(scores != NULL) {
		game_write_highscores(scores);
		fclose(scores);
	}
}

void drawHighScores() {



	drawScores(gamePtr->scores);

	Bitmap * numeros[10];
	numeros[0] = number_0;
	numeros[1] = number_1;
	numeros[2] = number_2;
	numeros[3] = number_3;
	numeros[4] = number_4;
	numeros[5] = number_5;
	numeros[6] = number_6;
	numeros[7] = number_7;
	numeros[8] = number_8;
	numeros[9] = number_9;

	// draw first

	if ((first->score) < 100) {
		drawBitmap(numeros[0], (200 - numeros[0]->bitmapInfoHeader.width) / 2,
				(500 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[(first->score) / 10],
				(250 - numeros[(first->score) / 10]->bitmapInfoHeader.width)
						/ 2,
				(500 - numeros[(first->score) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(first->score) % 10],
				(300 - numeros[(first->score) % 10]->bitmapInfoHeader.width)
						/ 2,
				(500 - numeros[(first->score) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	else {
		drawBitmap(numeros[(first->score) / 100],
				(200 - numeros[(first->score) / 100]->bitmapInfoHeader.width)
						/ 2,
				(500 - numeros[(first->score) / 100]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[((first->score) % 100) / 10],
				(250
						- numeros[((first->score) % 100) / 10]->bitmapInfoHeader.width)
						/ 2,
				(500
						- numeros[((first->score) % 100) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[((first->score) % 100) % 10],
				(300
						- numeros[((first->score) % 100) % 10]->bitmapInfoHeader.width)
						/ 2,
				(500
						- numeros[((first->score) % 100) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	// draw time

	if (first->time[0] < 10) {
		drawBitmap(numeros[0], (500 - numeros[0]->bitmapInfoHeader.width) / 2,
				(500 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[first->time[0]],
				(550 - numeros[first->time[0]]->bitmapInfoHeader.width) / 2,
				(500 - numeros[first->time[0]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(first->time[0]) / 10],
				(500 - numeros[(first->time[0]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(500 - numeros[(first->time[0]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(first->time[0]) % 10],
				(550 - numeros[(first->time[0]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(500 - numeros[(first->time[0]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	drawBitmap(doisPontos, (590 - doisPontos->bitmapInfoHeader.width) / 2,
			(500 - doisPontos->bitmapInfoHeader.height) / 2);

	if (first->time[1] < 10) {
		drawBitmap(numeros[0], (650 - numeros[0]->bitmapInfoHeader.width) / 2,
				(500 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[first->time[1]],
				(700 - numeros[first->time[1]]->bitmapInfoHeader.width) / 2,
				(500 - numeros[first->time[1]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(first->time[1]) / 10],
				(650 - numeros[(first->time[1]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(500 - numeros[(first->time[1]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(first->time[1]) % 10],
				(700 - numeros[(first->time[1]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(500 - numeros[(first->time[1]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	drawBitmap(doisPontos, (740 - doisPontos->bitmapInfoHeader.width) / 2,
			(500 - doisPontos->bitmapInfoHeader.height) / 2);

	if (first->time[2] < 10) {
		drawBitmap(numeros[0], (800 - numeros[0]->bitmapInfoHeader.width) / 2,
				(500 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[first->time[2]],
				(850 - numeros[first->time[2]]->bitmapInfoHeader.width) / 2,
				(500 - numeros[first->time[2]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(first->time[2]) / 10],
				(800 - numeros[(first->time[2]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(500 - numeros[(first->time[2]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(first->time[2]) % 10],
				(850 - numeros[(first->time[2]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(500 - numeros[(first->time[2]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	// draw date

	if (first->date[0] < 10) {
		drawBitmap(numeros[0], (1000 - numeros[0]->bitmapInfoHeader.width) / 2,
				(500 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[first->date[0]],
				(1050 - numeros[first->date[0]]->bitmapInfoHeader.width) / 2,
				(500 - numeros[first->date[0]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(first->date[0]) / 10],
				(1000 - numeros[(first->date[0]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(500 - numeros[(first->date[0]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(first->date[0]) % 10],
				(1050 - numeros[(first->date[0]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(500 - numeros[(first->date[0]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	drawBitmap(barra, (1100 - doisPontos->bitmapInfoHeader.width) / 2,
			(500 - doisPontos->bitmapInfoHeader.height) / 2);


	if (first->date[1] < 10) {
			drawBitmap(numeros[0], (1150 - numeros[0]->bitmapInfoHeader.width) / 2,
					(500 - numeros[0]->bitmapInfoHeader.height) / 2);
			drawBitmap(numeros[first->date[1]],
					(1200 - numeros[first->date[1]]->bitmapInfoHeader.width) / 2,
					(500 - numeros[first->date[1]]->bitmapInfoHeader.height) / 2);
		}

		else {
			drawBitmap(numeros[(first->date[1]) / 10],
					(1150 - numeros[(first->date[1]) / 10]->bitmapInfoHeader.width)
							/ 2,
					(500 - numeros[(first->date[1]) / 10]->bitmapInfoHeader.height)
							/ 2);
			drawBitmap(numeros[(first->date[1]) % 10],
					(1200 - numeros[(first->date[1]) % 10]->bitmapInfoHeader.width)
							/ 2,
					(500 - numeros[(first->date[1]) % 10]->bitmapInfoHeader.height)
							/ 2);
		}

	drawBitmap(barra, (1250 - doisPontos->bitmapInfoHeader.width) / 2,
			(500 - doisPontos->bitmapInfoHeader.height) / 2);

	// year
	drawBitmap(numeros[2],
			(1300 - numeros[2]->bitmapInfoHeader.width)
					/ 2,
			(500 - numeros[2]->bitmapInfoHeader.height)
					/ 2);
	drawBitmap(numeros[0],
			(1350 - numeros[0]->bitmapInfoHeader.width) / 2,
			(500 - numeros[0]->bitmapInfoHeader.height)
					/ 2);
	drawBitmap(numeros[(first->date[2])/10],
			(1400 - numeros[(first->date[2])/10]->bitmapInfoHeader.width) / 2,
			(500 - numeros[(first->date[2])/10]->bitmapInfoHeader.height)
					/ 2);
	drawBitmap(numeros[(first->date[2]) % 10],
			(1450 - numeros[(first->date[2]) % 10]->bitmapInfoHeader.width) / 2,
			(500 - numeros[(first->date[2]) % 10]->bitmapInfoHeader.height)
					/ 2);

	// draw second

	if ((second->score) < 100) {
		drawBitmap(numeros[0], (200 - numeros[0]->bitmapInfoHeader.width) / 2,
				(650 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[(second->score) / 10],
				(250 - numeros[(second->score) / 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->score) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(second->score) % 10],
				(300 - numeros[(second->score) % 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->score) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	else {
		drawBitmap(numeros[(second->score) / 100],
				(200 - numeros[(second->score) / 100]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->score) / 100]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[((second->score) % 100) / 10],
				(250
						- numeros[((second->score) % 100) / 10]->bitmapInfoHeader.width)
						/ 2,
				(650
						- numeros[((second->score) % 100) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[((second->score) % 100) % 10],
				(300
						- numeros[((second->score) % 100) % 10]->bitmapInfoHeader.width)
						/ 2,
				(650
						- numeros[((second->score) % 100) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	// draw time

	if (second->time[0] < 10) {
		drawBitmap(numeros[0], (500 - numeros[0]->bitmapInfoHeader.width) / 2,
				(650 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[second->time[0]],
				(550 - numeros[second->time[0]]->bitmapInfoHeader.width) / 2,
				(650 - numeros[second->time[0]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(second->time[0]) / 10],
				(500 - numeros[(second->time[0]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->time[0]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(second->time[0]) % 10],
				(550 - numeros[(second->time[0]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->time[0]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	drawBitmap(doisPontos, (590 - doisPontos->bitmapInfoHeader.width) / 2,
			(650 - doisPontos->bitmapInfoHeader.height) / 2);

	if (second->time[1] < 10) {
		drawBitmap(numeros[0], (650 - numeros[0]->bitmapInfoHeader.width) / 2,
				(650 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[second->time[1]],
				(700 - numeros[second->time[1]]->bitmapInfoHeader.width) / 2,
				(650 - numeros[second->time[1]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(second->time[1]) / 10],
				(650 - numeros[(second->time[1]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->time[1]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(second->time[1]) % 10],
				(700 - numeros[(second->time[1]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->time[1]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	drawBitmap(doisPontos, (740 - doisPontos->bitmapInfoHeader.width) / 2,
			(650 - doisPontos->bitmapInfoHeader.height) / 2);

	if (second->time[2] < 10) {
		drawBitmap(numeros[0], (800 - numeros[0]->bitmapInfoHeader.width) / 2,
				(650 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[second->time[2]],
				(850 - numeros[second->time[2]]->bitmapInfoHeader.width) / 2,
				(650 - numeros[second->time[2]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(second->time[2]) / 10],
				(800 - numeros[(second->time[2]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->time[2]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(second->time[2]) % 10],
				(850 - numeros[(second->time[2]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->time[2]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	// draw date

	if (second->date[0] < 10) {
		drawBitmap(numeros[0], (1000 - numeros[0]->bitmapInfoHeader.width) / 2,
				(650 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[second->date[0]],
				(1050 - numeros[second->date[0]]->bitmapInfoHeader.width) / 2,
				(650 - numeros[second->date[0]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(second->date[0]) / 10],
				(1000 - numeros[(second->date[0]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->date[0]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(second->date[0]) % 10],
				(1050 - numeros[(second->date[0]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->date[0]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	drawBitmap(barra, (1100 - doisPontos->bitmapInfoHeader.width) / 2,
			(650 - doisPontos->bitmapInfoHeader.height) / 2);

	if (second->date[1] < 10) {
		drawBitmap(numeros[0], (1150 - numeros[0]->bitmapInfoHeader.width) / 2,
				(650 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[second->date[1]],
				(1200 - numeros[second->date[1]]->bitmapInfoHeader.width) / 2,
				(650 - numeros[second->date[1]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(second->date[1]) / 10],
				(1150 - numeros[(second->date[1]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->date[1]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(second->date[1]) % 10],
				(1200 - numeros[(second->date[1]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(650 - numeros[(second->date[1]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	drawBitmap(barra, (1250 - doisPontos->bitmapInfoHeader.width) / 2,
			(650 - doisPontos->bitmapInfoHeader.height) / 2);

	// year
	drawBitmap(numeros[2],
			(1300 - numeros[2]->bitmapInfoHeader.width)
					/ 2,
			(650 - numeros[2]->bitmapInfoHeader.height)
					/ 2);
	drawBitmap(numeros[0],
			(1350 - numeros[0]->bitmapInfoHeader.width)
					/ 2,
			(650 - numeros[0]->bitmapInfoHeader.height)
					/ 2);
	drawBitmap(numeros[(second->date[2]) / 10],
			(1400 - numeros[(second->date[2]) / 10]->bitmapInfoHeader.width)
					/ 2,
			(650 - numeros[(second->date[2]) / 10]->bitmapInfoHeader.height)
					/ 2);
	drawBitmap(numeros[(second->date[2]) % 10],
			(1450 - numeros[(second->date[2]) % 10]->bitmapInfoHeader.width)
					/ 2,
			(650 - numeros[(second->date[2]) % 10]->bitmapInfoHeader.height)
					/ 2);

	//draw third

	if ((third->score) < 100) {
		drawBitmap(numeros[0], (200 - numeros[0]->bitmapInfoHeader.width) / 2,
				(800 - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[(third->score) / 10],
				(250 - numeros[(third->score) / 10]->bitmapInfoHeader.width)
						/ 2,
				(800 - numeros[(third->score) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(third->score) % 10],
				(300 - numeros[(third->score) % 10]->bitmapInfoHeader.width)
						/ 2,
				(800 - numeros[(third->score) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	else {
		drawBitmap(numeros[(third->score) / 100],
				(200 - numeros[(third->score) / 100]->bitmapInfoHeader.width)
						/ 2,
				(800 - numeros[(third->score) / 100]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[((third->score) % 100) / 10],
				(250
						- numeros[((third->score) % 100) / 10]->bitmapInfoHeader.width)
						/ 2,
				(800
						- numeros[((third->score) % 100) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[((third->score) % 100) % 10],
				(300
						- numeros[((third->score) % 100) % 10]->bitmapInfoHeader.width)
						/ 2,
				(800
						- numeros[((third->score) % 100) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	// draw time

	if (third->time[0] < 10) {
		drawBitmap(numeros[0], (500 - numeros[0]->bitmapInfoHeader.width) / 2,
				(800  - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[third->time[0]],
				(550 - numeros[third->time[0]]->bitmapInfoHeader.width) / 2,
				(800  - numeros[third->time[0]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(third->time[0]) / 10],
				(500 - numeros[(third->time[0]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(800  - numeros[(third->time[0]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(third->time[0]) % 10],
				(550 - numeros[(third->time[0]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(800 - numeros[(third->time[0]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	drawBitmap(doisPontos, (590 - doisPontos->bitmapInfoHeader.width) / 2,
			(800  - doisPontos->bitmapInfoHeader.height) / 2);

	if (third->time[1] < 10) {
		drawBitmap(numeros[0], (650 - numeros[0]->bitmapInfoHeader.width) / 2,
				(800  - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[third->time[1]],
				(700 - numeros[third->time[1]]->bitmapInfoHeader.width) / 2,
				(800  - numeros[third->time[1]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(third->time[1]) / 10],
				(650 - numeros[(third->time[1]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(800  - numeros[(third->time[1]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(third->time[1]) % 10],
				(700 - numeros[(third->time[1]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(800  - numeros[(third->time[1]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	drawBitmap(doisPontos, (740 - doisPontos->bitmapInfoHeader.width) / 2,
			(800  - doisPontos->bitmapInfoHeader.height) / 2);

	if (third->time[2] < 10) {
		drawBitmap(numeros[0], (800 - numeros[0]->bitmapInfoHeader.width) / 2,
				(800  - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[third->time[2]],
				(850 - numeros[third->time[2]]->bitmapInfoHeader.width) / 2,
				(800  - numeros[third->time[2]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(third->time[2]) / 10],
				(800 - numeros[(third->time[2]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(800  - numeros[(third->time[2]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(third->time[2]) % 10],
				(850 - numeros[(third->time[2]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(800  - numeros[(third->time[2]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	// draw date

	if (third->date[0] < 10) {
		drawBitmap(numeros[0], (1000 - numeros[0]->bitmapInfoHeader.width) / 2,
				(800  - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[third->date[0]],
				(1050 - numeros[third->date[0]]->bitmapInfoHeader.width) / 2,
				(800  - numeros[third->date[0]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(third->date[0]) / 10],
				(1000 - numeros[(third->date[0]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(800 - numeros[(third->date[0]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(third->date[0]) % 10],
				(1050 - numeros[(third->date[0]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(800 - numeros[(third->date[0]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	drawBitmap(barra, (1100 - doisPontos->bitmapInfoHeader.width) / 2,
			(800  - doisPontos->bitmapInfoHeader.height) / 2);

	if (third->date[1] < 10) {
		drawBitmap(numeros[0], (1150 - numeros[0]->bitmapInfoHeader.width) / 2,
				(800  - numeros[0]->bitmapInfoHeader.height) / 2);
		drawBitmap(numeros[third->date[1]],
				(1200 - numeros[third->date[1]]->bitmapInfoHeader.width) / 2,
				(800  - numeros[third->date[1]]->bitmapInfoHeader.height) / 2);
	}

	else {
		drawBitmap(numeros[(third->date[1]) / 10],
				(1150 - numeros[(third->date[1]) / 10]->bitmapInfoHeader.width)
						/ 2,
				(800  - numeros[(third->date[1]) / 10]->bitmapInfoHeader.height)
						/ 2);
		drawBitmap(numeros[(third->date[1]) % 10],
				(1200 - numeros[(third->date[1]) % 10]->bitmapInfoHeader.width)
						/ 2,
				(800  - numeros[(third->date[1]) % 10]->bitmapInfoHeader.height)
						/ 2);
	}

	drawBitmap(barra, (1250 - doisPontos->bitmapInfoHeader.width) / 2,
			(800  - doisPontos->bitmapInfoHeader.height) / 2);

	// year
	drawBitmap(numeros[2],
			(1300 - numeros[2]->bitmapInfoHeader.width)
					/ 2,
			(800 - numeros[2]->bitmapInfoHeader.height)
					/ 2);
	drawBitmap(numeros[0],
			(1350 - numeros[0]->bitmapInfoHeader.width)
					/ 2,
			(800 - numeros[0]->bitmapInfoHeader.height)
					/ 2);
	drawBitmap(numeros[(third->date[2]) / 10],
			(1400 - numeros[(third->date[2]) / 10]->bitmapInfoHeader.width)
					/ 2,
			(800 - numeros[(third->date[2]) / 10]->bitmapInfoHeader.height)
					/ 2);
	drawBitmap(numeros[(third->date[2]) % 10],
			(1450 - numeros[(third->date[2]) % 10]->bitmapInfoHeader.width)
					/ 2,
			(800 - numeros[(third->date[2]) % 10]->bitmapInfoHeader.height)
					/ 2);



}

void updateHighScores(){

	if(gamePtr->score > third->score) {


		unsigned long sec, min, hour, dia, mes, ano;

		if(rtc_get_time(&sec, &min, &hour))
			printf("Error , couldn't get the time !");

		if(rtc_get_date(&ano, &mes, &dia))
			printf("Error , couldn't get date !");


		third->score = gamePtr->score;
		third->time[0] = hour;
		third->time[1] = min;
		third->time[2] = sec;
		third->date[0] = dia;
		third->date[1] = mes;
		third->date[2] = ano;

		if(third->score > first->score) {
			score_t* temp1 = first;
			score_t* temp2 = second;
			first = third;
			second = temp1;
			third = temp2;
		}

		else if (third->score > second->score) {
			score_t* temp = second;
			second = third;
			third = temp;
		}

	}
}


