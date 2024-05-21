#include "ezrb.h"
#include "stdlib.h"

ezrb_t *ezrb_create(uint8_t len);
rb_sta_t ezrb_push(ezrb_t *cb, RB_DATA_T dat);
rb_sta_t ezrb_pop(ezrb_t *cb, RB_DATA_T *dat);
void ezrb_destroy(ezrb_t *cb);

/**
 * create a ringbuffer
 * @author Jinlin Deng
 */
ezrb_t *ezrb_create(uint8_t len)
{
    if (len < 1) {
        return NULL;
    }
    ezrb_t *rb = (ezrb_t *)malloc(sizeof(ezrb_t));
    if (rb == NULL) {
        return NULL;
    }
    rb->head = 0;
    rb->tail = 0;
    rb->len = len;
    rb->buffer = (RB_DATA_T *)malloc(sizeof(RB_DATA_T) * len);
    if (rb->buffer == NULL) {
        ezrb_destroy(rb);
        return NULL;
    }
    for (uint16_t i = 0; i < len; i++) {
        rb->buffer[i] = 0;
    }
    return rb;
}

/**
 * push the data to buffer
 * @param  data :the data
 * @author Jinlin Deng
 */
rb_sta_t ezrb_push(ezrb_t *rb, RB_DATA_T data)
{
    if (rb == NULL) {
        return RB_ERR;
    }
    // buffer is full?
    if (rb->tail + 1 == rb->head || (rb->tail+1 == rb->len && rb->head==0)) {
        return RB_FULL;
    }
    // write
    rb->buffer[rb->tail] = data;
    rb->tail = rb->tail + 1 == rb->len ? 0 : rb->tail + 1;
    return RB_OK;
}

/**
 * get the data from buffer
 * @param  rev :the data received
 * @author Jinlin Deng
 */
rb_sta_t ezrb_pop(ezrb_t *rb, RB_DATA_T *rev)
{
    if (rb == NULL) {
        return RB_ERR;
    }
    // buffer is empty ?
    if (rb->head == rb->tail) {
        return RB_EMPTY;
    }
    // read
    RB_DATA_T data = rb->buffer[rb->head];
    rb->head = rb->head + 1 == rb->len ? 0 :rb->head + 1;
    *rev = data;
    return RB_OK;
}

void ezrb_destroy(ezrb_t *rb)
{
    if (rb->buffer != NULL) {
        free(rb->buffer);
        rb->buffer = NULL;
    }
    if (rb != NULL) {
        free(rb);
        rb = NULL;
    }
}
