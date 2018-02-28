#include "input.h"
#include "mouse.h"

Mouse_Input handler_mouse_event(unsigned long packet[3]) {

	Mouse_Input input;

	input.LB = (packet[0] & BIT(0));
	input.MB = (packet[0] & BIT(2)) >> 2;
	input.RB = (packet[0] & BIT(1)) >> 1;

	if ((packet[0] & BIT(4)))
		input.x = packet[1] - 256;
	else
		input.x = packet[1];

	if ((packet[0] & BIT(5)))
		input.y = packet[2] - 256;
	else
		input.y = packet[2];

	return input;
}

KBD_Input handler_kbd_event(unsigned long scancode, int two_byte){

	KBD_Input input;

	input.ENTER = (scancode == ENTER_SCANCODE) ? 1 : 0;

	input.Z = (scancode == Z_SCANCODE) ? 1 : 0;

	input.ESC = (scancode == ESC_SCANCODE) ? 1 : 0;



	return input;
}

Event createEvent() {

	Event e;

	e.z_time = 0;
	e.gameover = 0;
	e.kbd.ENTER = 0;
	e.kbd.Z = 0;
	e.kbd.ESC = 0;

	e.play = 0;
	e.multiplayer = 0;
	e.scores = 0;
	e.exit = 0;

	return e;
}
