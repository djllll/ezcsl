#ifndef _EZCSL_H_
#define _EZCSL_H_

#include "stdint.h"
#include "stdarg.h"

#define EzParamType int16_t
#define CSL_BUF_LEN     30  //console buf len (include prefix)
#define HISTORY_LEN     3  //history record

typedef struct CmdUnitObj{
    const char *title_main;
    const char *describe;
    struct CmdUnitObj *next;
}Ez_CmdUnit_t;

typedef struct CmdObj{
    struct CmdUnitObj *unit;
    const char *title_sub;
    const char *describe;
    uint16_t id;
    uint8_t para_num;
    void (*callback)(EzParamType*);
    struct CmdObj *next;
}Ez_Cmd_t;

extern void ezcsl_init(const char *prefix);
extern void ezcsl_send_printf(const char *fmt, ...);
extern Ez_CmdUnit_t *ezcsl_cmd_unit_create(const char *title_main,const char *describe);
extern void ezcsl_cmd_register(Ez_CmdUnit_t *unit,const char *title_sub,const char *describe,uint8_t para_num, void (*callback)(EzParamType *));
#endif