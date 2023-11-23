#include "test.h"

void CslTest_Add2(EzParamType *para);
void CslTest_Add3(EzParamType *para);

void CslTest_Add2(EzParamType *para){
    ezcsl_send_printf("%s:result is %d\r\n",__func__,para[0]+para[1]);
}
void CslTest_Add3(EzParamType *para){
    ezcsl_send_printf("%s:result is %d\r\n",__func__,para[0]+para[1]+para[2]);
}