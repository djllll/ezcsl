#ifndef _EZCSL_PORT_H_
#define _EZCSL_PORT_H_

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

extern void ezport_send_str(char *str, uint16_t len);
extern void ezport_receive_a_char(char c);

#define CSL_BUF_LEN     40  //console buf len (include prefix)
#define HISTORY_LEN     3  //history record
#define PRINT_BUF_LEN   150
#define PARA_LEN_MAX    5
#define SPLIT_CHAR      ',' 

#define LOG_LEVEL_V    0x01
#define LOG_LEVEL_E    0x02
#define LOG_LEVEL_I    0x04
#define LOG_LEVEL_D    0x08
#define LOG_LEVEL_ALL  0xff
#define LOG_LEVEL_NONE 0x00

#define LOG_DEFINE     LOG_LEVEL_ALL  //log level define 

#endif
