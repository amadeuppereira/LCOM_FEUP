#include "test4.h"
#include "mouse.h"
#include "timer.h"
#include "math.h"
#include <stdio.h>




int mouse_test_packet(unsigned short cnt) {

	unsigned long byte;

	int ipc_status;
	message msg;
	int irq_set = BIT(INTERRUPT_BIT_MOUSE), r;

	if (mouse_subscribe_int() != INTERRUPT_BIT_MOUSE) {
		printf("Subscribe error! ");
		return 1;
	}

	if (mouse_set_stream_mode() != 0)  // enable stream mode and data report
		return 1;

	if (mouse_enable_data() != 0)
		return 1;

	while (cnt > 0) { /* You may want to use a different condition */
		/* Get a reqest message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					if (read_outbuf(&byte) != 0) {
						printf("Error reading from out buffer!\n");
						return 1;
					}
					if(packet_print(byte))
						cnt--;

				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	if(mouse_disable_data() != 0) {   // disable data report
		printf("Error disabling mouse");
		return 1;
	}

	if (mouse_unsubscribe_int() != 0) {
		printf("Unsubscribe error\n");
		return 1;
	}

	printf("\nReach total of packets\n");
	read_outbuf(&byte);

	return 0;

}	

int mouse_test_async(unsigned short idle_time){
	unsigned long byte;
	int cnt = idle_time*60;

	int ipc_status;
	message msg;
	int irq_set = BIT(INTERRUPT_BIT_MOUSE), r;
	int irq_set_timer = BIT(INTERRUPT_BIT);


	if (mouse_subscribe_int() != INTERRUPT_BIT_MOUSE) {
		printf("Subscribe error! ");
		return 1;
	}

	if (timer_subscribe_int() != INTERRUPT_BIT) {
		printf("TIMER subscribe error!\n");
		return 1;
	}

	if (mouse_set_stream_mode() != 0)
		return 1;

	if (mouse_enable_data() != 0)
		return 1;

	read_outbuf(&byte);

	while (cnt > 0) { /* You may want to use a different condition */
		/* Get a reqest message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					if (read_outbuf(&byte) != 0) {
						printf("Error reading from out buffer!\n");
						return 1;
					}
					packet_print(byte);
					cnt = idle_time*60;
				}
				if (msg.NOTIFY_ARG & irq_set_timer) { /* subscribed interrupt */

					cnt--;

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

	if (mouse_unsubscribe_int() != 0) {
		printf("Mouse unsubscribe error\n");
		return 1;
	}

	if (timer_unsubscribe_int() != 0) {
		printf("Timer unsubscribe error\n");
		return 1;
	}

	printf("\n%d seconds passed!\n", idle_time);
	read_outbuf(&byte);

	return 0;
}	

int mouse_test_remote(unsigned long period, unsigned short cnt){

	unsigned long command, byte;

	if(kbc_read_command(&command) != 0) {
		printf("Error reading command\n");
		return 1;
	}
	printf("%x\n", command);

	if (kbc_write_command(command & (~BIT(1))) != 0) { // setting command bit 1 to 0
		printf("Error writing new command\n");
		return 1;
	}

	if (mouse_enable_data() != 0)
		return 1;

	if (mouse_disable_data() != 0)
		return 1;

	if (mouse_set_remote_mode() != 0)
		return 1;

	while (cnt > 0) {

		if(mouse_request_packet() != 0)
			return 1;

		if(read_packet() != 0)
			return 1;
		cnt--;

		tickdelay(micros_to_ticks(period*1000));
	}

	if (mouse_set_stream_mode() != 0)
		return 1;

	if (kbc_write_command(command | BIT(1)) != 0) { // setting command bit 1 to 1
		printf("Error writing new command\n");
		return 1;
	}

	printf("\nReach total of packets\n");
	read_outbuf(&byte);
	return 0;
}	

typedef enum {INIT, DRAW, COMP} state_t;
typedef enum {RDOWN, RUP , MOVE , NOEV} ev_type_t;



void mouse_state_machine(ev_type_t evt , state_t* state) {

	switch(*state)
	{
	case INIT :
		if(evt == RDOWN)
			*state = DRAW;
		break;
	case DRAW :
		if (evt == MOVE)
		{
			*state = COMP;
		}
		else if (evt == RUP)
			*state = INIT;
		break;
	default :
		break;
	}

}


ev_type_t mouse_handle_event(unsigned long byte , state_t *state, short length, short *total){

	unsigned long tol = 0.9;
	static unsigned long packet[3];
	static int counter = 0;

	if (((byte & BIT(3)) && (counter == 0)) || (counter > 0)) {
		packet[counter] = byte;
		counter++;
	}
	if (counter == 3) {
		counter = 0;


		if ((packet[0] & BIT(6)) || (packet[0] & BIT(7)))
		{
			return NOEV;
		}
		if(*state == INIT) {
			if(packet[0] & RIGHT_BUTTON)
				return RDOWN;
		}

		else if (*state == DRAW)
		{
			if(!(packet[0] & RIGHT_BUTTON)) {
				*total = 0;
				return RUP;
			}

			int deltaX;
			int deltaY;
			// handle length
			if ((packet[0] & BIT(4)))
				deltaX = packet[1] - 256;
			else
				deltaX = packet[1];

			if ((packet[0] & BIT(5)))
				deltaY = packet[2] - 256;
			else
				deltaY = packet[2];

			if (length > 0) {
				if ((deltaY >= 0) && (deltaX >= 0)) {
					*total += deltaX;
				} else {
					if (deltaX < tol || deltaY < tol)   // tolerancia
						return NOEV;
					else
						*total = 0;
				}

			}
			else if (length < 0) {
				if ((deltaY <= 0) && (deltaX <= 0)) {
					*total += abs(deltaX);
				} else {
					if (abs(deltaX) < tol|| abs(deltaY) < tol)   // tolerancia
						return NOEV;
					else
						*total = 0;
				}

			}
			printf ("Progress :%d/%d\n" , *total,abs(length));
			if(*total >= abs(length))
				return MOVE;

		}

	}


	return NOEV;


}










int mouse_test_gesture(short length){

	unsigned long byte;
	short total = 0;

	int ipc_status;
	message msg;
	int irq_set = BIT(INTERRUPT_BIT_MOUSE), r;

	state_t status = INIT;
	ev_type_t event;

	if (mouse_subscribe_int() != INTERRUPT_BIT_MOUSE) {
		printf("Subscribe error! ");
		return 1;
	}

	if (mouse_set_stream_mode() != 0)
		return 1;

	if (mouse_enable_data() != 0)
		return 1;

	while (status != COMP) { /* You may want to use a different condition */
		/* Get a reqest message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					if (read_outbuf(&byte) != 0) {
						printf("Error reading from out buffer!\n");
						return 1;
					}
				    packet_print(byte);
				    event = mouse_handle_event(byte,&status, length, &total);
				    mouse_state_machine(event,&status);


				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	printf("\nPositive slope done!\n");

	if(mouse_disable_data() != 0) {
		printf("Error disabling mouse");
		return 1;
	}

	if (mouse_unsubscribe_int() != 0) {
		printf("Unsubscribe error\n");
		return 1;
	}

	read_outbuf(&byte);

	return 0;

}	
