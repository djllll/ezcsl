#include "ezcsl.h"
#include <stdio.h>
#include <conio.h>
extern void ezport_receive_a_char(char c);

int main(void)
{
    ezcsl_init("test: ");
    char c;
    do {
        c = getch();
        ezport_receive_a_char(c);
    } while (c!=0x1b); //esc
    return 0;
}
