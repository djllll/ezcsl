#include "app.h"
#include "ezcsl.h"
#include "ezcsl_port.h"

extern UART_HandleTypeDef hlpuart1;

static uint8_t uart_recv;


#define WELCOME " _   _      _ _       \r\n" \
                "| | | |    | | |      \r\n" \
                "| |_| | ___| | | ___  \r\n" \
                "|  _  |/ _ \\ | |/ _ \\ \r\n" \
                "| | | |  __/ | | (_) |\r\n" \
                "\\_| |_/\\___|_|_|\\___/ \r\n"

#define AT_FOO   0
#define AT_BAR   1
#define CALC_MPY 0
#define CALC_DIV 1


void app_main(void);
void AT_cmd_callback(uint16_t id, ez_param_t *para);
void Calc_cmd_callback(uint16_t id, ez_param_t *para);


/**
 * @brief AT命令单元的回调
 *
 * @param id
 * @param para
 */
void AT_cmd_callback(uint16_t id, ez_param_t *para)
{
    switch (id) {
    case AT_FOO:
        EZ_PRTL("bar");
        break;
    case AT_BAR:
        EZ_PRTL("foo");
        break;
    default:
        break;
    }
}

/**
 * @brief Calc命令单元的回调
 *
 * @param id
 * @param para
 */
void Calc_cmd_callback(uint16_t id, ez_param_t *para)
{
    switch (id) {
    case CALC_MPY:
        EZ_PRTL("%d", EZ_PtoI(para[0]) * EZ_PtoI(para[1]));
        break;
    case CALC_DIV:
        if (EZ_PtoI(para[1]) == 0) {
            EZ_PRTL("The divisor cannot be 0");
        } else {
            EZ_PRTL("%.3f", (float)EZ_PtoI(para[0]) / (float)EZ_PtoI(para[1]));
        }
        break;
    default:
        break;
    }
}


/**
 * @brief 主程序
 *
 */
void app_main(void)
{

    /**
     * @brief 开启接收中断
     *
     */
    HAL_UART_Receive_IT(&hlpuart1, &uart_recv, 1);

    /* 初始化EzCsl */
    ezcsl_init(COLOR_L_GREEN("STM32:"), COLOR_RED(WELCOME), NULL);

    /* 添加AT命令 */
    ez_cmd_unit_t *AT_unit = ezcsl_cmd_unit_create("AT", "AT Command", EZ_NSUDO, AT_cmd_callback);
    ezcsl_cmd_register(AT_unit, AT_FOO, "foo", "return bar", EZ_PARAM_NONE);
    ezcsl_cmd_register(AT_unit, AT_BAR, "bar", "return foo", EZ_PARAM_NONE);

    /* 添加Calc命令 */
    ez_cmd_unit_t *Calc_unit = ezcsl_cmd_unit_create("Calc", "Calc Command", EZ_NSUDO, Calc_cmd_callback);
    ezcsl_cmd_register(Calc_unit, CALC_MPY, "mpy", "a*b", EZ_PARAM_INT EZ_PARAM_INT);
    ezcsl_cmd_register(Calc_unit, CALC_DIV, "div", "a/b", EZ_PARAM_INT EZ_PARAM_INT);

    while (1) {
        ezcsl_tick();
    }

    ezcsl_deinit();
}


/**
 * @brief STM32串口接收的回调函数
 *
 * @param huart
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &hlpuart1) {
        // 调用ezcsl的接收函数
        ezport_receive_a_char(uart_recv);
        // 再次开启中断
        HAL_UART_Receive_IT(&hlpuart1, &uart_recv, 1);
    }
}
