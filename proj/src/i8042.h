#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8254 Keyboard and Mouse
 */

//Keyboard

#define TWOBYTE_SCANCODE  0xE0
#define READ_KBD_COMMAND     0x20
#define WRITE_KBD_COMMAND    0x60
#define ENTER_SCANCODE	0x1c
#define ESC_SCANCODE	0x01
#define Z_SCANCODE 		0x2c

#define BREAK_CODE 		0x80

//Mouse

#define SEND_MOUSE_COMMAND   0xD4
#define ENABLE_MOUSE    0xF4
#define DISABLE_MOUSE   0xF5
#define WRITE_BYTE_MOUSE 0xD4
#define ACK 			0xFA
#define NACK			0xFE
#define ERROR			0xFC
#define MOUSE_SET_STREAM_MODE  0xEA
#define MOUSE_REMOTE_MODE 0xF0
#define CNTRL_REG       0x64
#define READ_DATA 		0xEB
#define RIGHT_BUTTON    BIT(1)


#define BIT(n) (0x01<<(n))


#define KBD_IRQ         1               ///< keyboard IRQ line
#define MOUSE_IRQ		12				///< mouse IRQ line

#define STATUS_PORT     0x64
#define KBC_OUT_BUF     0x60
#define KBC_IN_BUF      0x60
#define KBC_CMD_BUF    	0x64
#define KBC_DATA_BUF	0x60
#define PORT_A          0x60
#define PORT_B          0x61



#define OUT_BUF_FULL    0x01
#define IN_BUF_FULL     0x02
#define SYS_FLAG        0x04
#define CMD_DATA 		0x08
#define KBD_INH 		0x10
#define TRANS_TIMEOUT   0x20
#define RCV_TIMEOUT  	0x40
#define PARITY_EVEN		0x80
#define INH_KBD 		0x10

#define KBD_ENA			0xAE
#define KBD_DIS			0xAD


/**@}*/











#endif /* _LCOM_I8042_H */
