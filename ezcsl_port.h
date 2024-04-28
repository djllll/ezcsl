#ifndef _EZCSL_PORT_H_
#define _EZCSL_PORT_H_

typedef unsigned short ezuint16_t;
typedef unsigned char ezuint8_t;

extern void ezport_send_str(char *str, ezuint16_t len);
extern void ezport_receive_a_char(char c);

#define CSL_BUF_LEN     40  //console buf len (include prefix)
#define HISTORY_LEN     3  //history record
#define PRINT_BUF_LEN   150
#define PARA_LEN_MAX    5
#define SPLIT_CHAR      ',' 

#endif
