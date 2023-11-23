#include "ezcsl.h"
#include <stdio.h>
#include <conio.h>

#include "test.h"

extern void ezport_receive_a_char(char c);

int main(void)
{
    ezcsl_init("TEST: ");
    Ez_CmdUnit_t *testunit= ezcsl_cmd_unit_create("test","add test callback");
    ezcsl_cmd_register(testunit,"add2","add,a,b",2,CslTest_Add2);
    ezcsl_cmd_register(testunit,"add3","add,a,b,c",3,CslTest_Add3);
    char c;
    do {
        c = getch();
        ezport_receive_a_char(c);
    } while (c!=0x1b); //esc
    return 0;
}
