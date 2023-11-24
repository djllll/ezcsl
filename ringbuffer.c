#include "ringbuffer.h"
#include "malloc.h"

  
// 初始化环形缓冲区
ring_buffer_t * RingBufferCreate(void)
{
    ring_buffer_t *buffer = (ring_buffer_t *)malloc(sizeof(ring_buffer_t));
    buffer->head = 0;
    buffer->tail = 0;
    for (uint8_t i = 0; i < RB_BUF_LEN; i++) {
        buffer->buffer[i] = 0;
    }
    return buffer;
}

// 向环形缓冲区写入数据  
rb_sta_t RingBufferPush(ring_buffer_t *cb, uint8_t data) {  
    // 判断缓冲区是否已满  
    if (MOD_BUFLEN(cb->tail + 1) == cb->head) {  
        return RB_FULL;  
    }  
    // 将数据写入缓冲区  
    cb->buffer[cb->tail] = data;  
    cb->tail = MOD_BUFLEN(cb->tail + 1); 
	return RB_OK; 
}  
  
// 从环形缓冲区读取数据  
rb_sta_t RingBufferPop(ring_buffer_t *cb,uint8_t *rev) {  
    // 判断缓冲区是否为空  
    if (cb->head == cb->tail) {  
        return RB_EMPTY;  
    }  
    // 读取数据并返回  
    uint8_t data = cb->buffer[cb->head];  
    cb->head = MOD_BUFLEN(cb->head + 1);  
	*rev=data;
    return RB_OK;  
}  
