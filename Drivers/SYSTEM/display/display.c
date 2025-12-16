// display.c
#include "display.h"
#include "./SYSTEM/uart/uart.h"

// 简单的OLED显示实现
void DISPLAY_Init(void)
{
    // 初始化OLED显示
    // 这里需要根据实际使用的显示屏编写初始化代码
    UART_Printf("Display Initialized\r\n");
}
