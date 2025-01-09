#include "ezcsl.h"
#include "tcpserver.h"
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#if (!TCP_MODE && !CMD_MODE)
#error Please Choose a mode (CMD_MODE/TCP_MODE) !!
#endif


#define WELCOME \
    "\033[36m    ______      ______                       __\r\n\
   / ____/___  / ____/___  ____  _________  / /__\r\n\
  / __/ /_  / / /   / __ \\/ __ \\/ ___/ __ \\/ / _ \\\r\n\
 / /___  / /_/ /___/ /_/ / / / (__  ) /_/ / /  __/\r\n\
/_____/ /___/\\____/\\____/_/ /_/____/\\____/_/\\___/\033[m\r\n"


/* Command ID */
#define TEST_ADD2_ID 0
#define TEST_ADD3_ID 1

#define ECHO_NONE_ID 0
#define ECHO_ONE_ID  1
#define ECHO_MUL_ID  2
#define ECHO_TIME_ID 3

#define INFO_VERSION_ID 0

/**
 * @brief test command callback
 * 
 * @param id 
 * @param para 
 * @return ez_cmd_ret_t 
 */
ez_cmd_ret_t test_cmd_callback(uint16_t id, ez_param_t *para)
{
    switch (id) {
    case TEST_ADD2_ID:
        EZ_PRTL("result is %d", EZ_PtoI(para[0]) + EZ_PtoI(para[1]));
        break;
    case TEST_ADD3_ID:
        EZ_PRTL("result is %d", EZ_PtoI(para[0]) + EZ_PtoI(para[1]) + EZ_PtoI(para[2]));
        break;
    default:
        break;
    }
    return CMD_FINISH;
}


/**
 * @brief echo command callback
 * 
 * @param id 
 * @param para 
 * @return ez_cmd_ret_t 
 */
ez_cmd_ret_t echo_cmd_callback(uint16_t id, ez_param_t *para)
{
    switch (id) {
    case ECHO_NONE_ID:
        EZ_PRTL("your input is none ");
        break;
    case ECHO_ONE_ID:
        EZ_PRTL("your input :%d", EZ_PtoI(para[0]));
        break;
    case ECHO_MUL_ID:
        EZ_PRTL("your input :%s f:%f i:%d", EZ_PtoS(para[0]), EZ_PtoF(para[1]), EZ_PtoI(para[2]));
        break;
    case ECHO_TIME_ID: {
        time_t now_time;
        time(&now_time);
        EZ_PRTL("=> %s", ctime(&now_time));
        return CMD_TIMEOUT_1000MS_AGAIN;
    } break;
    default:
        break;
    }
    return CMD_FINISH;
}


ez_cmd_ret_t info_cmd_callback(uint16_t id, ez_param_t *para)
{
    switch (id) {
    case INFO_VERSION_ID:
        EZ_PRTL("Version:"EZCSL_VERSION);
        break;
    default:
        break;
    }
    return CMD_FINISH;
}

/**
 * @brief modem receving callback
 * 
 * @param rev 
 * @param len 
 * @return modem_rev_func_t 
 */
static modem_rev_func_t modem_rev_cb(char *rev, uint16_t len)
{
    static int filesize = 0;
    static FILE *f;
    if (rev != NULL) {
        if (filesize == 0) {
            const char *filename = rev;
            filesize = atoi(rev + strnlen(rev, len) + 1);
            printf("FileName:%s\r\n", filename);
            printf("FileSize:%d\r\n", filesize);
            f = fopen(filename, "w");
            fwrite("", 0, 1, f);
            fclose(f);
            f = fopen(filename, "ab");
        } else {
            if (filesize > len) {
                filesize -= len;
            } else {
                len = filesize;
            }
            size_t written = fwrite(rev, len, 1, f);
        }
    } else {
        filesize = 0;
        fclose(f);
    }
    return M_SEND_NEXT;
}



/**
 * @brief main
 * 
 * @return int 
 */
int main(void)
{
#ifdef TCP_MODE
    tcp_server_open();
    tcp_server_start_receving_thread();
#endif

    /* EzCsl init */
    ezcsl_init(COLOR_CYAN("TEST:"), WELCOME, "123456");

    // EZ_LOGI("EzCsl","init ok");

    ezcsl_modem_set("rb", modem_rev_cb);

    /* add cmd */
    ez_cmd_unit_t *test_unit = ezcsl_cmd_unit_create("test", "add test callback", EZ_NSUDO, test_cmd_callback);
    ezcsl_cmd_register(test_unit, TEST_ADD2_ID, "add2", "add,a,b", "ii");
    ezcsl_cmd_register(test_unit, TEST_ADD3_ID, "add3", "add,a,b,c", "iii");

    ez_cmd_unit_t *echo_unit = ezcsl_cmd_unit_create("echo", "echo your input", EZ_NSUDO, echo_cmd_callback);
    ezcsl_cmd_register(echo_unit, ECHO_NONE_ID, "none", "input ", "");
    ezcsl_cmd_register(echo_unit, ECHO_ONE_ID, "one", "input 'int'", "i");
    ezcsl_cmd_register(echo_unit, ECHO_MUL_ID, "mul", "input 'str,float,int'", "sfi");
    ezcsl_cmd_register(echo_unit, ECHO_TIME_ID, "time", "time echo", "");

    ez_cmd_unit_t *info_unit = ezcsl_cmd_unit_create("info", "software info", EZ_SUDO, info_cmd_callback);
    ezcsl_cmd_register(info_unit, INFO_VERSION_ID, "version", "", "");

/* input */
#ifdef CMD_MODE
    do {
        ezport_receive_a_char(getch());
    } while (!ezcsl_tick()); // quit
#else
    while (!ezcsl_tick())
        ;
    tcp_server_close();
#endif


    /* deinit */
    ezcsl_deinit();

    printf("\r\nEzCsl: Bye.\r\n");
    return 0;
}
