#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_



#define ESC_BREAK       0x81   //  break code
#define TWOBYTE_SCANCODE  0xE0
#define READ_KBD_COMMAND     0x20
#define WRITE_KBD_COMMAND    0x60

#define BREAK_CODE 		0x80



#define BIT(n) (0x01<<(n))


#define KBD_IRQ         1               // keyboard IRQ line

#define STATUS_PORT     0x64
#define KBD_OUT_BUF     0x60
#define KBD_IN_BUF      0x64
#define KBD_IN_BUF_ARGS 0x60
#define KBD_CMD_BUF    	0x64
#define KBD_DATA_BUF	0x60
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

/* KBD commands */

#define KBD_MENU 		0xF1
#define KBD_ENABLE		0xF4
#define KBD_MAKEBREAK	0xF7
#define KBD_ECHO 		0xFE
#define KBD_RESET		0xFF

/* KBD responses */

#define KBD_OK			0xAA
#define KBD_ACK 		0xFA
#define KBD_OVERRUN		0xFF
#define KBD_RESEND 		0xFE
#define KBD_BREAK 		0xF0
#define KBD_FA			0x10
#define KBD_FE			0x20
























#endif /* _LCOM_I8042_H */
