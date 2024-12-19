#include "ezcsl_port.h"

/* your include begin */
#include "tcpserver.h"
#include <stdio.h>
#include <windows.h>
/* your include end */

void ezport_send_str(char *str, uint16_t len);
void ezport_delay(uint16_t ms);




/**
 * use this to send
 * @param str str need to send
 * @param len the length of the str
 */
void ezport_send_str(char *str, uint16_t len)
{
    /** Write your code here ↓↓↓↓ */
    #ifdef TCP_MODE
    tcp_server_send_char(str,len);
    #else 
    for (int i = 0; i < len; i++) {
        putchar(str[i]);
    }
    #endif
    /** Write your code here ↑↑↑↑ */
}



/**
 * @brief write your delay here
 * if you use EzCsl in multithreading , this function must can suspend current task
 * 
 * @param ms 
 */
void ezport_delay(uint16_t ms)
{
    /** Write your code here ↓↓↓↓ */
    Sleep(ms);
    /** Write your code here ↑↑↑↑ */
}
