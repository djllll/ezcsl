#include "ezcsl.h"
#include "stdarg.h"
#include "stdint.h"
#include "string.h"

/* your include begin */
#include "stdio.h"
/* your include end */

#define BUF_LEN   50
#define DBGprintf printf

static struct EzCslHandleStruct {
    uint8_t prefix_len;
    char buf[BUF_LEN];
    uint16_t bufp;
    uint16_t bufl;
} ezhdl;
#define BUFP_RST() ezhdl.bufp = ezhdl.prefix_len

/* ez console port ,user need to achieve this himself */
void ezport_receive_a_char(char c);
void ezport_send_str(char *str, uint16_t len);

void ezcsl_init(const char *prefix);
static void ezcsl_send_printf(const char *fmt, ...);
static void ezcsl_submit(void);


/**
 * use this function by `extern void ezport_receive_a_char(char c)`
 * place it in a loop
 * @param  c the char from input
 * @author Jinlin Deng
 */
void ezport_receive_a_char(char c)
{
    static uint8_t direction_flag = 0; // direction keys
    if (!direction_flag) {
        if (c >= 0x20 && c <= 0x7e && ezhdl.bufl < BUF_LEN) {
            /* visible char */
            for (uint16_t i = ezhdl.bufl; i >= ezhdl.bufp + 1; i--) {
                ezhdl.buf[i] = ezhdl.buf[i - 1];
            }
            ezhdl.buf[ezhdl.bufp] = c;
            ezcsl_send_printf("\033[s");

            ezport_send_str(ezhdl.buf + ezhdl.bufp, ezhdl.bufl - ezhdl.bufp + 1);
            ezcsl_send_printf("\033[u\033[1C");
            ezhdl.bufp++;
            ezhdl.bufl++;
        } else if (c == 0x08 && ezhdl.bufp > ezhdl.prefix_len) { // cannot delete the prefix
            /* backspace */
            ezcsl_send_printf("\033[1D\033[s");
            for (uint16_t i = ezhdl.bufp - 1; i < ezhdl.bufl; i++) {
                ezhdl.buf[i] = ezhdl.buf[i + 1];
                ezport_send_str(ezhdl.buf + i, 1);
            }
            ezhdl.bufp--;
            ezhdl.bufl--;
            ezcsl_send_printf("\033[K\033[u");
        } else if (c == 0x0d) {
            /* enter */
            ezhdl.buf[ezhdl.bufp] = 0; // cmd end
            ezcsl_submit();
        } else if (c == 0) {
            direction_flag = 1;
        }
    } else {
        if (c == 0x4b) {
            /* left arrow */
            if (ezhdl.bufp > ezhdl.prefix_len) {
                ezcsl_send_printf("\033[1D");
                ezhdl.bufp--;
            }
        } else if (c == 0x4d) {
            /* right arrow */
            if (ezhdl.bufp < ezhdl.bufl) {
                ezcsl_send_printf("\033[1C");
                ezhdl.bufp++;
            }
        }
        direction_flag = 0;
    }
}

/**
 * use this function to send
 * @param str str need to send
 * @param len the length of the str
 * @author Jinlin Deng
 */
void ezport_send_str(char *str, uint16_t len)
{
    /**
     * Write your code here
     */
    for (int i = 0; i < len; i++) {
        putchar(str[i]);
    }
}


/**
 * init
 * @param
 * @author Jinlin Deng
 */
void ezcsl_init(const char *prefix)
{
    ezhdl.prefix_len = strlen(prefix);
    uint16_t i;
    for (i = 0; i < BUF_LEN; i++) {
        ezhdl.buf[i] = i < ezhdl.prefix_len ? prefix[i] : 0;
    }
    ezhdl.bufp = ezhdl.prefix_len;
    ezhdl.bufl = ezhdl.prefix_len;
    ezport_send_str(ezhdl.buf, ezhdl.prefix_len);
}


static void ezcsl_send_printf(const char *fmt, ...)
{
    uint16_t printed;
    va_list args;
    char dat_buf[BUF_LEN];
    va_start(args, fmt);
    printed = vsprintf(dat_buf, fmt, args);
    va_end(args);
    ezport_send_str(dat_buf, printed);
}


static void ezcsl_submit(void)
{
    ezcsl_send_printf("\r\n");
    DBGprintf("cmd submit! (%s)", ezhdl.buf); // 模拟执行
    ezcsl_send_printf("\r\n");
    ezport_send_str(ezhdl.buf, ezhdl.prefix_len);
    ezhdl.buf[ezhdl.prefix_len] = 0;
    ezhdl.bufl = ezhdl.prefix_len;
    ezhdl.bufp = ezhdl.prefix_len;
}