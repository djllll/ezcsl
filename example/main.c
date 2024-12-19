#include "ezcsl.h"
#include "tcpserver.h"
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>


#if (!TCP_MODE && !CMD_MODE)
#error Please Choose a mode (CMD_MODE/TCP_MODE) !!
#endif


#define WELCOME \
    "\033[36m    ______      ______                       __\r\n\
   / ____/___  / ____/___  ____  _________  / /__\r\n\
  / __/ /_  / / /   / __ \\/ __ \\/ ___/ __ \\/ / _ \\\r\n\
 / /___  / /_/ /___/ /_/ / / / (__  ) /_/ / /  __/\r\n\
/_____/ /___/\\____/\\____/_/ /_/____/\\____/_/\\___/\033[m\r\n"


#define TEST_ADD2_ID 0
#define TEST_ADD3_ID 1

#define ECHO_NONE_ID 0
#define ECHO_ONE_ID  1
#define ECHO_MUL_ID  2
#define ECHO_TIME_ID 3

ez_cmd_ret_t test_cmd_callback(uint16_t id, ez_param_t *para)
{
    switch (id) {
    case TEST_ADD2_ID:
        EZ_PRT("result is %d\r\n", EZ_PtoI(para[0]) + EZ_PtoI(para[1]));
        break;
    case TEST_ADD3_ID:
        EZ_PRT("result is %d\r\n", EZ_PtoI(para[0]) + EZ_PtoI(para[1]) + EZ_PtoI(para[2]));
        break;
    default:
        break;
    }
    return CMD_FINISH;
}

ez_cmd_ret_t echo_cmd_callback(uint16_t id, ez_param_t *para)
{
    switch (id) {
    case ECHO_NONE_ID:
        EZ_LOGE("test", "your input is none ");
        break;
    case ECHO_ONE_ID:
        EZ_PRT("your input :%d\r\n", EZ_PtoI(para[0]));
        break;
    case ECHO_MUL_ID:
        EZ_PRT("your input :%s f:%f i:%d\r\n", EZ_PtoS(para[0]), EZ_PtoF(para[1]), EZ_PtoI(para[2]));
        break;
    case ECHO_TIME_ID: {
        time_t now_time;
        time(&now_time);
        EZ_PRT("=> %s", ctime(&now_time));
        return CMD_TIMEOUT_1000MS_AGAIN;
    } break;
    default:
        break;
    }
    return CMD_FINISH;
}

static modem_rev_func_t modem_rev_cb(char *rev, uint16_t len)
{
    printf("get");
    if (rev != NULL) {
        for (uint16_t i = 0; i < len; i++) {
            printf("%02x ", rev[i]);
        }
    } else {
        printf("finish");
    }
    return M_SEND_NEXT;
}


int main(void)
{
#ifdef TCP_MODE
    tcp_server_open();
    tcp_server_start_receving_thread();
#endif

    /* EzCsl init */
    ezcsl_init("\033[36mTEST:\033[m ", WELCOME, NULL);

    // EZ_LOGI("EzCsl","init ok");

    ezcsl_modem_set("rb", modem_rev_cb);

    /* add cmd */
    ez_cmd_unit_t *test_unit = ezcsl_cmd_unit_create("test", "add test callback", 0, test_cmd_callback);
    ezcsl_cmd_register(test_unit, TEST_ADD2_ID, "add2", "add,a,b", "ii");
    ezcsl_cmd_register(test_unit, TEST_ADD3_ID, "add3", "add,a,b,c", "iii");

    ez_cmd_unit_t *echo_unit = ezcsl_cmd_unit_create("echo", "echo your input", 1, echo_cmd_callback);
    ezcsl_cmd_register(echo_unit, ECHO_NONE_ID, "none", "input ", "");
    ezcsl_cmd_register(echo_unit, ECHO_ONE_ID, "one", "input 'i'", "i");
    ezcsl_cmd_register(echo_unit, ECHO_MUL_ID, "mul", "input 'sfi'", "sfi");
    ezcsl_cmd_register(echo_unit, ECHO_TIME_ID, "time", "time echo", "");

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
