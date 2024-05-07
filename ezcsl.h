#ifndef _EZCSL_H_
#define _EZCSL_H_

#include "stdarg.h"
#include "ezstring.h"
#include "ezcsl_port.h"
#include "ezxmodem.h"



#define EZ_PtoS(param) ((const char*)(param))   //ez_param_t => string
#define EZ_PtoI(param) (*(int*)(param))         //ez_param_t => integer
#define EZ_PtoF(param) (*(float*)(param))       //ez_param_t => float
#define ez_param_t      void*

typedef uint8_t ez_log_level_mask_t;

typedef enum{
    EZ_OK=0,
    EZ_ERR
}ez_sta_t;

typedef struct CmdUnitObj{
    const char *title_main;
    const char *describe;
    void (*callback)(uint16_t ,ez_param_t*);
    struct CmdUnitObj *next;
    uint8_t need_sudo;
}ez_cmd_unit_t;

typedef struct CmdObj{
    struct CmdUnitObj *unit;
    const char *title_sub;
    const char *describe;
    uint16_t id;
    uint8_t para_num;
    const char *para_desc;
    struct CmdObj *next;
}ez_cmd_t;



extern void ezcsl_init(const char *prefix ,const char *welcome,const char *sudo_psw);
extern void ezcsl_deinit(void); 
extern void ezcsl_log_level_set(ez_log_level_mask_t mask);
extern uint8_t ezcsl_log_level_allowed(ez_log_level_mask_t mask);
extern void ezcsl_xmodem_set(const char *modem_prefix,xmodem_cfg_t *cfg);
extern uint8_t ezcsl_tick(void);
extern void ezcsl_reset_prefix(void);

extern ez_cmd_unit_t *ezcsl_cmd_unit_create(const char *title_main,const char *describe ,uint8_t need_sudo, void (*callback)(uint16_t,ez_param_t*));
extern ez_sta_t ezcsl_cmd_register(ez_cmd_unit_t *unit, uint16_t id, const char *title_sub, const char *describe, const char* para_desc);
extern void ezport_send_str(char *str, uint16_t len);
extern void ezcsl_printf(const char *fmt, ...);


#define MOVE_CURSOR_ABS(n)      "\033["#n"G"
#define ERASE_TO_END()          "\033[K"
#define SAVE_CURSOR_POS()       "\033[s"
#define RESTORE_CURSOR_POS()    "\033[u"
#define CURSOR_FORWARD(n)       "\033["#n"C"
#define CURSOR_BACK(n)          "\033["#n"D"

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


#if (LOG_DEFINE & LOG_LEVEL_E)
#define EZ_LOGE(TAG, format, ...)                                                                \
    if (ezcsl_log_level_allowed(LOG_LEVEL_E)) {                                                  \
        ezcsl_printf(MOVE_CURSOR_ABS(0) COLOR_L_RED("[" TAG "] " format "\r\n"), ##__VA_ARGS__); \
        ezcsl_reset_prefix();                                                                    \
    }
#else
#define EZ_LOGE(TAG, format, ...) \
    {                             \
        ;                         \
    }
#endif


#if (LOG_DEFINE & LOG_LEVEL_I)
#define EZ_LOGI(TAG, format, ...)                                                                  \
    if (ezcsl_log_level_allowed(LOG_LEVEL_I)) {                                                    \
        ezcsl_printf(MOVE_CURSOR_ABS(0) COLOR_L_GREEN("[" TAG "] " format "\r\n"), ##__VA_ARGS__); \
        ezcsl_reset_prefix();                                                                      \
    }
#else
#define EZ_LOGI(TAG, format, ...) \
    {                             \
        ;                         \
    }
#endif


#if (LOG_DEFINE & LOG_LEVEL_D)
#define EZ_LOGD(TAG, format, ...)                                                                 \
    if (ezcsl_log_level_allowed(LOG_LEVEL_D)) {                                                   \
        ezcsl_printf(MOVE_CURSOR_ABS(0) COLOR_L_BLUE("[" TAG "] " format "\r\n"), ##__VA_ARGS__); \
        ezcsl_reset_prefix();                                                                     \
    }
#else
#define EZ_LOGD(TAG, format, ...) \
    {                             \
        ;                         \
    }
#endif

#if (LOG_DEFINE & LOG_LEVEL_V)
#define EZ_LOGV(TAG, format, ...)                                                   \
    if (ezcsl_log_level_allowed(LOG_LEVEL_V)) {                                     \
        ezcsl_printf(MOVE_CURSOR_ABS(0) "[" TAG "] " format "\r\n", ##__VA_ARGS__); \
        ezcsl_reset_prefix();                                                       \
    }
#else
#define EZ_LOGV(TAG, format, ...) \
    {                             \
        ;                         \
    }
#endif


#endif