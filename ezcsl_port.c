#include "ezcsl_port.h"

/* your include begin */
#include "stdio.h"

/* your include end */

void ezport_send_str(char *str, uint16_t len);
void ezport_delay(uint16_t ms);




/**
 * use this to send
 * @param str str need to send
 * @param len the length of the str
 * @author Jinlin Deng
 */
void ezport_send_str(char *str, uint16_t len)
{
    /** Write your code here ↓↓↓↓ */
    for (int i = 0; i < len; i++) {
        putchar(str[i]);
    }
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
    for (int i = 0; i < (long)ms*1000; i++) {
    }
    /** Write your code here ↑↑↑↑ */
}
