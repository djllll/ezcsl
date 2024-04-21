#ifndef _EZCSL_H_
#define _EZCSL_H_

#include "stdio.h"

#include "ezstring.h"
#include "ezcsl_port.h"

#define CSL_BUF_LEN     40  //console buf len (include prefix)
#define HISTORY_LEN     3  //history record
#define PRINT_BUF_LEN   60
#define PARA_LEN_MAX    5
#define SPLIT_CHAR      ',' 

#define EZ_PtoS(param) ((const char*)(param))   //ez_param_t => string
#define EZ_PtoI(param) (*(int*)(param))         //ez_param_t => integer
#define EZ_PtoF(param) (*(float*)(param))       //ez_param_t => float
#define ez_param_t      void*


#define BACKSPACE_KV    0x7f
#define TAB_KV          0x09
#define ENTER_KV        0x0d
#define CTRL_C_KV       0x03
#define CTRL_D_KV       0x04

typedef enum{
    EZ_OK=0,
    EZ_ERR
}ez_sta_t;

typedef struct CmdUnitObj{
    const char *title_main;
    const char *describe;
    void (*callback)(ezuint16_t ,ez_param_t*);
    struct CmdUnitObj *next;
}Ez_CmdUnit_t;

typedef struct CmdObj{
    struct CmdUnitObj *unit;
    const char *title_sub;
    const char *describe;
    ezuint16_t id;
    ezuint8_t para_num;
    const char *para_desc;
    struct CmdObj *next;
}Ez_Cmd_t;

extern void ezcsl_init(const char *prefix,const char *welcome);
extern void ezcsl_deinit(void); 
extern void ezcsl_tick(void);
extern Ez_CmdUnit_t *ezcsl_cmd_unit_create(const char *title_main,const char *describe ,void (*callback)(ezuint16_t,ez_param_t* ));
extern ez_sta_t ezcsl_cmd_register(Ez_CmdUnit_t *unit, ezuint16_t id, const char *title_sub, const char *describe, const char* para_desc);
extern void ezport_send_str(char *str, ezuint16_t len);

#define ezcsl_send_printf(fmt, ...)                                        \
    do {                                                                   \
        ezuint16_t _d_printed;                                               \
        char _d_dat_buf[PRINT_BUF_LEN];                                    \
        _d_printed = snprintf(_d_dat_buf, PRINT_BUF_LEN, fmt, ##__VA_ARGS__); \
        ezport_send_str(_d_dat_buf, _d_printed);                                 \
    } while (0)

#define ezcsl_send_string(str) ezport_send_str(str,estrlen(str))


#define COLOR_BLACK(s)	 	    "\033[0;30m"s"\033[0m"
#define COLOR_L_BLACK(s)	 	"\033[1;30m"s"\033[0m"
#define COLOR_RED(s)      	    "\033[0;31m"s"\033[0m"
#define COLOR_L_RED(s)    	    "\033[1;31m"s"\033[0m"
#define COLOR_GREEN(s)    	    "\033[0;32m"s"\033[0m"
#define COLOR_L_GREEN(s)  	    "\033[1;32m"s"\033[0m"
#define COLOR_YELLOW(s)   	    "\033[0;33m"s"\033[0m"
#define COLOR_L_YELLOW(s) 	    "\033[1;33m"s"\033[0m"
#define COLOR_BLUE(s)     	    "\033[0;34m"s"\033[0m"
#define COLOR_L_BLUE(s)	 	    "\033[1;34m"s"\033[0m"
#define COLOR_PURPLE(s)   	    "\033[0;35m"s"\033[0m"
#define COLOR_L_PURPLE(s)	    "\033[1;35m"s"\033[0m"
#define COLOR_CYAN(s)     	    "\033[0;36m"s"\033[0m"
#define COLOR_L_CYAN(s)   	    "\033[1;36m"s"\033[0m"
#define COLOR_WHITE(s)    	    "\033[0;37m"s"\033[0m"
#define COLOR_L_WHITE(s)    	"\033[1;37m"s"\033[0m"

//#define BOLD          "\033[1m"
//#define UNDERLINE     "\033[4m"

#define EZ_LOGE(TAG,format, ...) do{ezcsl_send_string(TAG);ezcsl_send_string(": ");ezcsl_send_printf(COLOR_L_RED(format), ##__VA_ARGS__);ezcsl_send_string("\r\n");}while(0)
#define EZ_LOGI(TAG,format, ...) do{ezcsl_send_string(TAG);ezcsl_send_string(": ");ezcsl_send_printf(COLOR_L_GREEN(format), ##__VA_ARGS__);ezcsl_send_string("\r\n");}while(0)
#define EZ_LOGD(TAG,format, ...) do{ezcsl_send_string(TAG);ezcsl_send_string(": ");ezcsl_send_printf(COLOR_L_BLUE(format), ##__VA_ARGS__);ezcsl_send_string("\r\n");}while(0)
#define EZ_LOG(TAG,format, ...)  do{ezcsl_send_string(TAG);ezcsl_send_string(": ");ezcsl_send_printf(format, ##__VA_ARGS__);ezcsl_send_string("\r\n");}while(0)

#endif