#include "objects.h"
#include "video_gr.h"



int asteroidRate = ASTEROID_RATE;
int asteroidMinSpeed = ASTEROID_MIN_SPEED;
int asteroidMaxSpeed = ASTEROID_MAX_SPEED;
int bulletSpeed = BULLET_SPEED;
int bulletRate = BULLET_RATE;


Spaceship* createSpaceship() {

	Spaceship* spc = (Spaceship*) malloc(sizeof(Spaceship));

	spc->bmp = loadBitmap("/home/lcom/svn/proj/src/images/spaceship2.bmp");
	spc->shield = loadBitmap("/home/lcom/svn/proj/src/images/spaceship_shield.bmp");
	spc->x = 0;
	spc->y = 0;

	return spc;
}

void destroySpaceship(Spaceship* spc) {
	if(spc == NULL) {
		return;
	}

	deleteBitmap(spc->bmp);
	deleteBitmap(spc->shield);
	free(spc);

}

Asteroid* createAsteroid(Bitmap* asteroidBitmap, int hp){
	Asteroid* newAst = (Asteroid*) malloc (sizeof(Asteroid));

	newAst->bmp = asteroidBitmap ;//loadBitmap("/home/lcom/svn/proj/src/images/asteroid.bmp");
	newAst->x = rand() % (get_x_res() - newAst->bmp->bitmapInfoHeader.width);
	newAst->y = -50;
	newAst->ySpeed = (rand() % (asteroidMaxSpeed + 1 - asteroidMinSpeed)) + asteroidMinSpeed;
	newAst->hp = hp;
	return newAst;
}

Background * createBackground(){

	Background * space = (Background*)malloc(sizeof(Background));

	space->bmp = loadBitmap("/home/lcom/svn/proj/src/images/space.bmp");

	return space;


}


Main_menu * createMainMenu(){

	Main_menu * menu = (Main_menu*)malloc(sizeof(Main_menu));


	menu->bmp = loadBitmap("/home/lcom/svn/proj/src/images/main_menu.bmp");
	if(menu->bmp == NULL){
		printf("erro\n");
	}

	return menu;
}


void destroyBackground(Background * b) {
	if(b == NULL) {
		return;
	}
	deleteBitmap(b->bmp);
	free(b);
}


void destroyMainMenu(Main_menu * b) {
	if(b == NULL) {
		return;
	}
	deleteBitmap(b->bmp);
	free(b);
}



void destroyAsteroid(Asteroid * ast){
	if (ast == NULL){
		return;
	}
	free(ast);
}

void drawAsteroid(Asteroid * ast){
	drawBitmapCollision(ast->bmp , ast->x ,ast->y);
}

int drawSpaceship(Spaceship* spc) {
	return drawBitmapTestCollision(spc->bmp, spc->x, spc->y);
}

int drawSpaceshipShield(Spaceship* spc) {
	return drawBitmapTestCollision(spc->shield, spc->x, spc->y);
}

void drawBackground(Background * bg){
	drawBitmap(bg->bmp , 0 , 0);
}

void drawMainMenu(Main_menu * menu){
	drawBitmap(menu->bmp , 0 , 0);
}

Bullet* createBullet(Bitmap* bmp, int x, int y) {

	Bullet* b = (Bullet*) malloc(sizeof(Bullet));

	b->bmp = bmp;
	b->x = x;
	b->y = y;
	b->ySpeed = bulletSpeed;

	return b;
}

void destroyBullet(Bullet* b) {
	if(b == NULL) {
		return;
	}

	free(b);

}

int drawBullet(Bullet* b) {
	return drawBitmapTestCollision(b->bmp, b->x, b->y);
}

void updateDifficulty(int score) {
	static int flag = 1;
	if (score == 0) {
		asteroidRate = ASTEROID_RATE;
		asteroidMinSpeed = ASTEROID_MIN_SPEED;
		asteroidMaxSpeed = ASTEROID_MAX_SPEED;
	}
	else if((score % 10) == 0 && flag) {
		asteroidRate -= 1;
		//asteroidMinSpeed += 2;
		asteroidMaxSpeed += 2;
		flag = 0;
		//printf("Rate: %d, Min: %d, Max: %d\n", asteroidRate, asteroidMinSpeed, asteroidMaxSpeed);
	}
	else if ((score % 10) != 0){
		flag = 1;
	}

}

Gameover* createGameover() {

	Gameover * go = (Gameover*)malloc(sizeof(Gameover));

	go->bmp = loadBitmap("/home/lcom/svn/proj/src/images/gameover.bmp");

	return go;
}

void destroyGameover(Gameover* go) {
	if(go == NULL) {
		return;
	}
	deleteBitmap(go->bmp);
	free(go);
}

void drawGameover(Gameover* go) {
	drawBitmap(go->bmp , (800 - go->bmp->bitmapInfoHeader.width) / 2 , (600 - go->bmp->bitmapInfoHeader.height) / 2 );
}

Scores * createScores(){
	Scores * scores = (Scores*)malloc(sizeof(Scores));
	scores->bmp = loadBitmap("/home/lcom/svn/proj/src/images/highscores.bmp");
	return scores;
}

void destroyScores(Scores * s){
	if (s == NULL){
		return;
	}
	deleteBitmap(s->bmp);
	free(s);
}

void drawScores(Scores * s){
	drawBitmap(s->bmp , 0 , 0);
}


Tip* createTip() {

	Tip * t = (Tip*)malloc(sizeof(Tip));

	t->bmp = loadBitmap("/home/lcom/svn/proj/src/images/tip.bmp");

	return t;
}

void destroyTip(Tip* t) {
	if(t == NULL) {
		return;
	}
	deleteBitmap(t->bmp);
	free(t);
}

void drawTip(Tip* t) {
	drawBitmap(t->bmp , (800 - t->bmp->bitmapInfoHeader.width) / 2 , (600 - t->bmp->bitmapInfoHeader.height) / 2 );
}



Mouse_menu * createMouseMenu(){

	Mouse_menu* mouse = (Mouse_menu*) malloc(sizeof(Mouse_menu));
	mouse->bmp = loadBitmap("/home/lcom/svn/proj/src/images/mousemenu.bmp");
	mouse->x = 0;
	mouse->y = 0;

	return mouse;
}

void destroyMouseMenu(Mouse_menu * mouse){
	if (mouse == NULL){
		return;
	}
	deleteBitmap(mouse->bmp);
	free(mouse);
}

void drawMouseMenu(Mouse_menu * mouse){
	drawBitmap(mouse->bmp , mouse->x ,mouse->y);
}


Main_menu_options * createMenuOptions(){

	Main_menu_options* main_menu_op = (Main_menu_options*) malloc(sizeof(Main_menu_options));

	main_menu_op->bmp_play = loadBitmap("/home/lcom/svn/proj/src/images/play_menu.bmp");
	main_menu_op->bmp_exit  = loadBitmap("/home/lcom/svn/proj/src/images/exit_menu.bmp");
	main_menu_op->bmp_highscores = loadBitmap("/home/lcom/svn/proj/src/images/highscores_menu.bmp");
	main_menu_op->bmp_multiplayer = loadBitmap("/home/lcom/svn/proj/src/images/multiplayer_menu.bmp");
	main_menu_op->bmp_play_selected = loadBitmap("/home/lcom/svn/proj/src/images/play_menu_select.bmp");
	main_menu_op->bmp_exit_selected  = loadBitmap("/home/lcom/svn/proj/src/images/exit_menu_select.bmp");
	main_menu_op->bmp_highscores_selected = loadBitmap("/home/lcom/svn/proj/src/images/highscores_menu_select.bmp");
	main_menu_op->bmp_multiplayer_selected = loadBitmap("/home/lcom/svn/proj/src/images/multiplayer_menu_select.bmp");

	main_menu_op->play_selected = 0;
	main_menu_op->exit_selected = 0;
	main_menu_op->highscores_selected = 0;
	main_menu_op->multiplayer_selected = 0;

	return main_menu_op;
}

void destroyMenuOptions(Main_menu_options * menu_op){
	if (menu_op == NULL){
		return;
	}
	deleteBitmap(menu_op->bmp_play);
	deleteBitmap(menu_op->bmp_exit);
	deleteBitmap(menu_op->bmp_highscores);
	deleteBitmap(menu_op->bmp_multiplayer);
	deleteBitmap(menu_op->bmp_play_selected);
	deleteBitmap(menu_op->bmp_exit_selected);
	deleteBitmap(menu_op->bmp_highscores_selected);
	deleteBitmap(menu_op->bmp_multiplayer_selected);
	free(menu_op);
}

void drawMenuOptions(Main_menu_options * menu_op){

	if (menu_op->play_selected){
		drawBitmap(menu_op->bmp_play_selected , (800 - menu_op->bmp_play_selected->bitmapInfoHeader.width) / 2 , (600 - menu_op->bmp_play_selected->bitmapInfoHeader.height) / 2 );
	}
	else {
		drawBitmap(menu_op->bmp_play , (800 - menu_op->bmp_play->bitmapInfoHeader.width) / 2 , (600 - menu_op->bmp_play->bitmapInfoHeader.height) / 2 );
	}

	if (menu_op->multiplayer_selected){
		drawBitmap(menu_op->bmp_multiplayer_selected , (800 - menu_op->bmp_multiplayer_selected->bitmapInfoHeader.width) / 2 , (750 - menu_op->bmp_multiplayer_selected->bitmapInfoHeader.height) / 2 );
	}

	else {
		drawBitmap(menu_op->bmp_multiplayer , (800 - menu_op->bmp_multiplayer->bitmapInfoHeader.width) / 2 , (750 - menu_op->bmp_multiplayer->bitmapInfoHeader.height) / 2 );
	}

	if (menu_op->highscores_selected){
		drawBitmap(menu_op->bmp_highscores_selected , (800 - menu_op->bmp_highscores_selected->bitmapInfoHeader.width) / 2 , (900 - menu_op->bmp_highscores_selected->bitmapInfoHeader.height) / 2 );
	}

	else {
		drawBitmap(menu_op->bmp_highscores , (800 - menu_op->bmp_highscores->bitmapInfoHeader.width) / 2 , (900 - menu_op->bmp_highscores->bitmapInfoHeader.height) / 2 );
	}

	if (menu_op->exit_selected){
		drawBitmap(menu_op->bmp_exit_selected , (800 - menu_op->bmp_exit_selected->bitmapInfoHeader.width) / 2 , (1050 - menu_op->bmp_exit_selected->bitmapInfoHeader.height) / 2 );
	}

	else {
		drawBitmap(menu_op->bmp_exit , (800 - menu_op->bmp_exit->bitmapInfoHeader.width) / 2 , (1050 - menu_op->bmp_exit->bitmapInfoHeader.height) / 2 );
	}


}




