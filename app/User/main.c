#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"
#include "btn.h"
#include "drv_uart.h"
#include "im948.h"


int main() {
    LED_Init();
    delay_init();
    
    uart_open(DBP_UART_PORT, 115200, 8, 1, 'n');
    uart_open(IM948_UART_PORT, 115200, 8, 1, 'n');
    
    // safe delay
    delay_ms(1000);
    
    // 喚醒 im948
    im948_cmd_03();
    
    // 0x400 只訂閱歐拉角
    im948_cmd_12(5, 255, 0,  0, 3, 2, 2, 4, 9, 0x0040);
    
    // 關閉自動回傳訂閱
    im948_cmd_18();
    
    while (1) {
        RECLED = 1;
        delay_ms(500);
        im948_cmd_11();
        im948_parse_fifo_buf();
        
        if (im948_have_new_data) {
            im948_have_new_data = 0;
            Sdp("%.1f,%.1f,%.1f\r\n", im948_angle_x, im948_angle_y, im948_angle_z);
        }
        
        RECLED = 0;
        delay_ms(500);
    }
    
}
