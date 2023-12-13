#include "test.h"
#include "ezcsl.h"

#define TEST_ADD2_ID 0
#define TEST_ADD3_ID 1

#define TEST_MUL_ID 0

void eztest_cmd_callback(uint16_t id, ez_param_t* para)
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
void eztest_auto_callback(uint16_t id,ez_param_t* para){
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


void eztest_cmd_init(void)
{
    Ez_CmdUnit_t *testunit = ezcsl_cmd_unit_create("test", "add test callback",eztest_cmd_callback);
    ezcsl_cmd_register(testunit, TEST_ADD2_ID, "add2", "add,a,b", "ii");
    ezcsl_cmd_register(testunit, TEST_ADD3_ID, "add3", "add,a,b,c", "iii");

     Ez_CmdUnit_t *testautocomplete = ezcsl_cmd_unit_create("mul", "multi-type parameters",eztest_auto_callback);
    ezcsl_cmd_register(testautocomplete, TEST_MUL_ID, "test", "input 'sfi'","sfi");

}

