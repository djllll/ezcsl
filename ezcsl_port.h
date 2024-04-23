#ifndef _EZCSL_PORT_H_
#define _EZCSL_PORT_H_

typedef unsigned short ezuint16_t;
typedef unsigned char ezuint8_t;

extern void ezport_send_str(char *str, ezuint16_t len);
extern void ezport_receive_a_char(char c);


#endif
