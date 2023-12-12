#include "test.h"
#define TEST_ADD2_ID 0
#define TEST_ADD3_ID 1

#define TEST_AUTOCOMPLETE 0

void eztest_cmd_callback(uint16_t id, ez_param_t *para)
{
    switch (id) {
    case TEST_ADD2_ID:
        ezcsl_send_printf("%s:result is %d\r\n", __func__, para[0] + para[1]);
        break;
    case TEST_ADD3_ID:
        ezcsl_send_printf("%s:result is %d\r\n", __func__, para[0] + para[1] + para[2]);
        break;
    default:
        break;
    }
}
void eztest_auto_callback(uint16_t id,ez_param_t *para){
    return ;
}


void eztest_cmd_init(void)
{
    Ez_CmdUnit_t *testunit = ezcsl_cmd_unit_create("test", "add test callback",eztest_cmd_callback);
    ezcsl_cmd_register(testunit, TEST_ADD2_ID, "add2", "add,a,b", 2);
    ezcsl_cmd_register(testunit, TEST_ADD3_ID, "add3", "add,a,b,c", 3);

     Ez_CmdUnit_t *testautocomplete = ezcsl_cmd_unit_create("autoc", "describe",eztest_auto_callback);
    ezcsl_cmd_register(testautocomplete, TEST_AUTOCOMPLETE, "autocomplete", "testautocomplete",0);

}

