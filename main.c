#include "ezcsl.h"
#include <stdio.h>
#include <unistd.h>

#ifdef _WIN32
#include <conio.h>
#elif defined(__linux__)
#include "stdlib.h"
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
#define ECHO_ONE_ID 1
#define ECHO_MUL_ID 2

void test_cmd_callback(ezuint16_t id, ez_param_t* para)
{
    switch (id) {
    case TEST_ADD2_ID: 
        ezcsl_printf("result is %d\r\n",  EZ_PtoI(para[0]) +EZ_PtoI(para[1]));
        break;
    case TEST_ADD3_ID:
        ezcsl_printf("result is %d\r\n", EZ_PtoI(para[0]) + EZ_PtoI(para[1]) + EZ_PtoI(para[2]));
        break;
    default:
        break;
    }
}
void echo_cmd_callback(ezuint16_t id,ez_param_t* para){
    switch (id)
    {
    case ECHO_NONE_ID:
        EZ_LOGE("test","your input is none ");
        break;
    case ECHO_ONE_ID:
        ezcsl_printf("your input :%d\r\n", EZ_PtoI(para[0]));
        break;
    case ECHO_MUL_ID:
        ezcsl_printf("your input :%s f:%f i:%d\r\n", EZ_PtoS(para[0]) ,EZ_PtoF(para[1]) , EZ_PtoI(para[2]));
        break;
    default:
        break;
    }
    return ;
}

// static xmodem_rev_func_t xmodem_rev_frame_cb(char *rev){
//     printf("get");
//     if(rev!=NULL){
//         for(int i=0;i<128;i++){
//             printf("%02x ",rev[i]);
//         }
//     }else{
//         printf("finish");
//     }
//     return X_SEND_NEXT;
// }

// static void xmodem_delay_ms(ezuint16_t ms)
// {
//     for (int i = 0; i < 10000; i++) {
//         for (int j = 0; j < 1000; j++) {
//         }
//     }
// }

// static xmodem_cfg_t xmodem_cfg = {
//     .delay_ms = xmodem_delay_ms,
//     .frame_cb = xmodem_rev_frame_cb
// } ;

int main(void)
{
    /* init */
    ezcsl_init("\033[36mTEST:\033[m ",WELCOME,"123");
    EZ_LOGI("EzCsl","init ok");


    // ezcsl_xmodem_set("rx",&xmodem_cfg);


    /* add cmd */
    ez_cmd_unit_t *test_unit = ezcsl_cmd_unit_create("test", "add test callback",0,test_cmd_callback);
    ezcsl_cmd_register(test_unit, TEST_ADD2_ID, "add2", "add,a,b", "ii");
    ezcsl_cmd_register(test_unit, TEST_ADD3_ID, "add3", "add,a,b,c", "iii");

    ez_cmd_unit_t *echo_unit = ezcsl_cmd_unit_create("echo", "echo your input",1,echo_cmd_callback);
    ezcsl_cmd_register(echo_unit, ECHO_NONE_ID, "none", "input ","");
    ezcsl_cmd_register(echo_unit, ECHO_ONE_ID, "one", "input 'i'","i");
    ezcsl_cmd_register(echo_unit, ECHO_MUL_ID, "mul", "input 'sfi'","sfi");

    /* input */
    char c;
    do {
        #ifdef _WIN32
        c = getch();
        #elif defined(__linux__)
        system("stty raw -echo");
        c = getchar();
        system("stty cooked echo");
        #else
        print('others platform')
        return 0;
        #endif
        ezport_receive_a_char(c);
    } while (!ezcsl_tick()); //quit


    /* deinit */
    ezcsl_deinit();
    return 0;
}
