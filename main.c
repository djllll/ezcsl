#include "ezcsl.h"
#include <stdio.h>
#include <conio.h>

#include "test.h"

extern void ezport_receive_a_char(char c);

#define WELCOME \
"\033[36m    ______      ______                       __\r\n\
   / ____/___  / ____/___  ____  _________  / /__\r\n\
  / __/ /_  / / /   / __ \\/ __ \\/ ___/ __ \\/ / _ \\\r\n\
 / /___  / /_/ /___/ /_/ / / / (__  ) /_/ / /  __/\r\n\
/_____/ /___/\\____/\\____/_/ /_/____/\\____/_/\\___/\033[m\r\n"

int main(void)
{
    ezcsl_init("\033[36mTEST:\033[m ",WELCOME);
    eztest_cmd_init();
    char c;
    do {
        c = getch();
        ezport_receive_a_char(c);
        ezcsl_tick();
    } while (c!=0x1b); //esc
    return 0;
}
