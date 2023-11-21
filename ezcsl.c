#include "ezcsl.h"
#include "stdarg.h"
#include "stdint.h"
#include "string.h"

/* your include begin */
#include "stdio.h"
/* your include end */

#define BUF_LEN   128
#define DBGprintf printf

static struct EzCslHandleStruct {
    uint8_t appname_len;
    char buf[BUF_LEN];
    uint16_t bufp;
    uint16_t bufl;
} ezhdl;
#define BUFP_RST() ezhdl.bufp = ezhdl.appname_len

void ezport_receive_a_char(char c);
void ezport_send_str(char *str, uint16_t len);

void ezcsl_init(const char *appname);
static void ezcsl_send_printf(const char *fmt, ...);


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
            // if (ezhdl.bufp == ezhdl.bufl) { // add
            //     ezhdl.buf[ezhdl.bufp] = c;
            //     ezhdl.bufp++;
            //     ezhdl.bufl++;
            //     if (ezhdl.bufp >= BUF_LEN) {
            //         ezhdl.bufp = 0;
            //     }
            //     ezhdl.buf[ezhdl.bufp] = 0;
            //     ezport_send_str(&c, 1);
            // } else { // insert
                
                for (uint16_t i = ezhdl.bufl; i >= ezhdl.bufp+1; i--) {
                        ezhdl.buf[i]=ezhdl.buf[i-1];
                }
                ezhdl.buf[ezhdl.bufp] = c;
                ezcsl_send_printf("\033[s");

                ezport_send_str(ezhdl.buf+ezhdl.bufp, ezhdl.bufl-ezhdl.bufp+1);
                ezcsl_send_printf("\033[u\033[1C");
                ezhdl.bufp++;
                ezhdl.bufl++;
                
            // }
        } else if (c == 0x08) {
            /* backspace */
            ezcsl_send_printf("\033[1D\033[s");
            if (ezhdl.bufp > 0) {
                for (uint16_t i = ezhdl.bufp - 1; i < ezhdl.bufl; i++) {
                        ezhdl.buf[i] = ezhdl.buf[i + 1];
                        ezport_send_str(ezhdl.buf + i, 1);
                }
                ezhdl.bufp--;
                ezhdl.bufl--;
            }
            ezcsl_send_printf("\033[K\033[u");
        } else if (c == 0x0d) {
            /* enter */
            DBGprintf("\ncurrent buf is :%s", ezhdl.buf);
        } else if (c == 0) {
            direction_flag = 1;
        }
    } else {
        if (c == 0x4b) {
            /* left arrow */
            if (ezhdl.bufp > 0) {
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

void ezport_send_str(char *str, uint16_t len)
{
    /**
     * Write your code here
     */
    for (int i = 0; i < len; i++) {
        putchar(str[i]);
    }
}


void ezcsl_init(const char *appname)
{
    ezhdl.appname_len = strlen(appname);
    uint16_t i;
    for (i = 0; i < BUF_LEN; i++) {
        ezhdl.buf[i] = 0;
    }
    ezhdl.bufp = 0;
    ezhdl.bufl = 0;
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
