#ifndef _INPUT_H_
#define _INPUT_H_

/** @defgroup Input Input
 * @{
 * Functions for inputs and events
 */

///Represents a mouse input
typedef struct {
	int x, y;
	int LB;
	int MB;
	int RB;
} Mouse_Input;

///Represents a keyboard input
typedef struct {
	int ENTER;
	int Z;
	int ESC;
} KBD_Input;

///Represents an event
typedef struct {
	KBD_Input kbd;
	int z_time;
	int gameover;

	int play;
	int multiplayer;
	int scores;
	int exit;
}Event;

/**
 * @brief creates a Mouse_Input with a given packet
 *
 * @param packet packet to analyse
 *
 * @return an object of the Mouse_Input struct
 */
Mouse_Input handler_mouse_event(unsigned long packet[3]);

/**
 * @brief creates a KBD_Input with a given scancode
 *
 * @param scancode keyboard scancode
 * @param two_byte flag telling if it is a two byte scancode
 *
 * @return an object of the KBD_Input struct
 */
KBD_Input handler_kbd_event(unsigned long scancode, int two_byte);

/**
 * @brief creates an Event
 *
 * @return an "basic" object of the Event struct
 */
Event createEvent();


/**@}*/

#endif
