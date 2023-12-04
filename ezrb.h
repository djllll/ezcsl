#ifndef _EZRB_H_
#define _EZRB_H_

#include "stdint.h"

#define RB_BUF_LEN 32
#define MOD_BUFLEN(x) ((x)&31)
#define RB_DATA_T uint8_t

typedef enum{
    RB_OK,
    RB_FULL,
    RB_EMPTY
}rb_sta_t;

typedef struct {  
    RB_DATA_T buffer[RB_BUF_LEN];  
    int head;    
    int tail;    
} ezrb_t;  

extern ezrb_t *ezrb_create(void);
extern rb_sta_t ezrb_push(ezrb_t *buffer,RB_DATA_T dat);
extern rb_sta_t ezrb_pop(ezrb_t *buffer,RB_DATA_T *dat);

#endif
