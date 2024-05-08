#ifdef EZ_XMODEM
#ifndef _EZXMODEM_H_
#define _EZXMODEM_H_

#include "ezcsl_port.h"

#include "ezrb.h"

#ifdef __cplusplus 
extern "C" { 
#endif 

typedef enum{
    X_SEND_NEXT = 0,
    X_SEND_REPEAT,
    X_SEND_ABORT
}xmodem_rev_func_t;

typedef enum{
    X_TRANS_OK = 0,
    X_TRANS_TIMEOUT
}xmodem_rev_trans_t;


typedef struct{
    xmodem_rev_func_t (*frame_cb)(char *);
    void (*delay_ms)(uint16_t );
}xmodem_cfg_t;

extern xmodem_rev_trans_t xmodem_start(ezrb_t *rb, xmodem_cfg_t *cfg);

#ifdef __cplusplus 
}
#endif 

#endif
#endif