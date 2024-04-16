#include "ezcsl_port.h"

/* your include begin */
#include "stdio.h"
/* your include end */

void ezport_send_str(char *str, ezuint16_t len);

/**
 * use this to send
 * @param str str need to send
 * @param len the length of the str
 * @author Jinlin Deng
 */
void ezport_send_str(char *str, ezuint16_t len)
{
    /** Write your code here ↓↓↓↓ */
    for (int i = 0; i < len; i++) {
        putchar(str[i]);
    }
    /** Write your code here ↑↑↑↑ */
}
