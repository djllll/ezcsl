#ifndef _EZCSL_H_
#define _EZCSL_H_

#include "stdint.h"
#include "stdarg.h"

#define ez_param_t int16_t
#define CSL_BUF_LEN     30  //console buf len (include prefix)
#define HISTORY_LEN     3  //history record

typedef enum{
    EZ_OK=0,
    EZ_ERR
}ez_sta_t;

typedef struct CmdUnitObj{
    const char *title_main;
    const char *describe;
    void (*callback)(uint16_t ,ez_param_t*);
    struct CmdUnitObj *next;
}Ez_CmdUnit_t;

typedef struct CmdObj{
    struct CmdUnitObj *unit;
    const char *title_sub;
    const char *describe;
    uint16_t id;
    uint8_t para_num;
    struct CmdObj *next;
}Ez_Cmd_t;

extern void ezcsl_init(const char *prefix,const char *welcome);
extern void ezcsl_send_printf(const char *fmt, ...);
extern Ez_CmdUnit_t *ezcsl_cmd_unit_create(const char *title_main,const char *describe ,void (*callback)(uint16_t,ez_param_t *));
extern ez_sta_t ezcsl_cmd_register(Ez_CmdUnit_t *unit,uint16_t id,const char *title_sub,const char *describe,uint8_t para_num);
#endif