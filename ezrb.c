#include "ezrb.h"
#include "malloc.h"

ezrb_t *ezrb_create(void);
rb_sta_t ezrb_push(ezrb_t *buffer,RB_DATA_T dat);
rb_sta_t ezrb_pop(ezrb_t *buffer,RB_DATA_T *dat);
void ezrb_destroy(ezrb_t *buffer);

/**
 * create a ringbuffer
 * @author Jinlin Deng
 */
ezrb_t * ezrb_create(void)
{
    ezrb_t *buffer = (ezrb_t *)malloc(sizeof(ezrb_t));
    buffer->head = 0;
    buffer->tail = 0;
    for (RB_DATA_T i = 0; i < RB_BUF_LEN; i++) {
        buffer->buffer[i] = 0;
    }
    return buffer;
}

/**
 * push the data to buffer
 * @param  data :the data
 * @author Jinlin Deng
 */  
rb_sta_t ezrb_push(ezrb_t *cb, RB_DATA_T data) {  
    // buffer is full?  
    if (MOD_BUFLEN(cb->tail + 1) == cb->head) {  
        return RB_FULL;  
    }  
    // write  
    cb->buffer[cb->tail] = data;  
    cb->tail = MOD_BUFLEN(cb->tail + 1); 
	return RB_OK; 
}  
  
/**
 * get the data from buffer
 * @param  rev :the data received
 * @author Jinlin Deng
 */  
rb_sta_t ezrb_pop(ezrb_t *cb,RB_DATA_T *rev) {  
    // buffer is empty ?
    if (cb->head == cb->tail) {  
        return RB_EMPTY;  
    }  
    // read  
    RB_DATA_T data = cb->buffer[cb->head];  
    cb->head = MOD_BUFLEN(cb->head + 1);  
	*rev=data;
    return RB_OK;  
}  

void ezrb_destroy(ezrb_t *buffer)
{
   if(buffer!=NULL){
    free(buffer);
    buffer=NULL;
   }
}
