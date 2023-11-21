#ifndef _EZCSL_H_
#define _EZCSL_H_

#include "stdint.h"
#include "stdarg.h"

#define EzParamType int16_t


typedef struct CmdUnitObj{
    const char *title_main;
    const char *describe;
    struct CmdUnitObj *next;
}Ez_CmdUnit_t;

typedef struct CmdObj{
    struct CmdUnitObj *unit;
    const char *title_sub;
    const char *describe;
    uint16_t para_num;
    void (*callback)(EzParamType*);
    struct CmdObj *next;
}Ez_Cmd_t;

extern void ezcsl_init(const char *prefix);
void ezcsl_send_printf(const char *fmt, ...);

#endif