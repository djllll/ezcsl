#include "ezcsl.h"
#include <stdio.h>


#define WINDOWS_INPUT 

#ifdef WINDOWS_INPUT
#include <conio.h>
#else

#define INPUT_CMD_MANUALLY(cmd)                \
    do {                                   \
        char *tmp = (char *)cmd;           \
        while (*tmp != 0) {                \
            ezport_receive_a_char(*tmp++); \
            ezcsl_tick();                  \
        }                                  \
        ezport_receive_a_char(ENTER_KV);   \
        ezcsl_tick();                      \
    } while (0)
#endif

extern void ezport_receive_a_char(char c);

#define WELCOME \
"\033[36m    ______      ______                       __\r\n\
   / ____/___  / ____/___  ____  _________  / /__\r\n\
  / __/ /_  / / /   / __ \\/ __ \\/ ___/ __ \\/ / _ \\\r\n\
 / /___  / /_/ /___/ /_/ / / / (__  ) /_/ / /  __/\r\n\
/_____/ /___/\\____/\\____/_/ /_/____/\\____/_/\\___/\033[m\r\n"



#define TEST_ADD2_ID 0
#define TEST_ADD3_ID 1
#define TEST_MUL_ID 0

void test_cmd_callback(ezuint16_t id, ez_param_t* para)
{
    switch (id) {
    case TEST_ADD2_ID: 
        ezcsl_send_printf("result is %d\r\n",  EZ_PtoI(para[0]) +EZ_PtoI(para[1]));
        break;
    case TEST_ADD3_ID:
        ezcsl_send_printf("result is %d\r\n", EZ_PtoI(para[0]) + EZ_PtoI(para[1]) + EZ_PtoI(para[2]));
        break;
    default:
        break;
    }
}
void test_auto_callback(ezuint16_t id,ez_param_t* para){
    switch (id)
    {
    case TEST_MUL_ID:
        ezcsl_send_printf("your input s:%s f:%f i:%d\r\n", EZ_PtoS(para[0]) ,EZ_PtoF(para[1]) , EZ_PtoI(para[2]));
        break;
    
    default:
        break;
    }
    return ;
}


int main(void)
{
    /* init */
    ezcsl_init("\033[36mTEST:\033[m ",WELCOME);
    
    /* add cmd */
    Ez_CmdUnit_t *testunit = ezcsl_cmd_unit_create("test", "add test callback",test_cmd_callback);
    ezcsl_cmd_register(testunit, TEST_ADD2_ID, "add2", "add,a,b", "ii");
    ezcsl_cmd_register(testunit, TEST_ADD3_ID, "add3", "add,a,b,c", "iii");

    Ez_CmdUnit_t *testautocomplete = ezcsl_cmd_unit_create("mul", "multi-type parameters",test_auto_callback);
    ezcsl_cmd_register(testautocomplete, TEST_MUL_ID, "test", "input 'sfi'","sfi");

    /* input */
#ifdef WINDOWS_INPUT
    char c;
    do {
        c = getch();
        ezport_receive_a_char(c);
        ezcsl_tick();
    } while (c!=0x1b); //esc
#else 
    INPUT_CMD_MANUALLY("test,add2,1,2");
#endif

    /* deinit */
    ezcsl_deinit();
    return 0;
}
