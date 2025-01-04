#include <stdio.h>
#include "stm32f10x.h"

#include "delay.h"
#include "controller.h"
#include "drv_uart.h"
#include "im948.h"
#include "timer.h"


char u3_msg[150];
uint32_t sdcard_line_ctr = 0;

volatile int pwm_count, pre_pwm_state;
volatile int rec_mode=5, rec_signal=0, pre_rec=0, key_up=1, dbc=0;
volatile int pre_mode;


void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        im948_cmd_11();
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

void TIM4_IRQHandler(void) {
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        if (EXTI0 == 1) {
            pwm_count++;
        } else {
            if (pwm_count > 300 && pwm_count < 600) {
                if (!rec_signal) rec_signal = 1;
            } else if (pwm_count > 800) {
                if (rec_signal) rec_signal = 0;
            }
            TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
        }
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}

void check_rec_mode(void) {
    pre_mode = rec_mode;
    if (PIN0 == 0 && PIN1 == 0 && pre_mode != 1) {
        timer_set_rec_prd(REC_PER_1S);
        rec_mode = 1;
    } else if (PIN0 == 0 && PIN1 == 1 && pre_mode != 2) {
        timer_set_rec_prd(REC_PER_2S);
        rec_mode = 2;
    } else if (PIN0 == 1 && PIN1 == 0 && pre_mode != 3) {
        timer_set_rec_prd(REC_PER_3S);
        rec_mode = 3;
    } else if (PIN0 == 1 && PIN1 == 1 && pre_mode != 5) {
        timer_set_rec_prd(REC_PER_5S);
        rec_mode = 5;
    }
}

void check_rec_status(void) {
    if ((PIN12 == 0) && key_up && (dbc == 8000)) {
        key_up = 0;
        rec_signal = !rec_signal;
    } else if (PIN12 == 1)
        key_up = 1;
    if (dbc > 8000)
        dbc = 0;
    else
        dbc++;
    if (pre_rec != rec_signal) {
        if (rec_signal) {
            RECLED = 1;
            TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
            TIM_Cmd(TIM3, ENABLE);
            TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
        } else {
            RECLED = 0;
            TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
            TIM_Cmd(TIM3, DISABLE);
        }
    }
    pre_rec = rec_signal;
}



int main() {
    ctrl_rec_init();
    ctrl_led_init();
    ctrl_time_jumper_init();
    ctrl_relay_init();
    
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
    
    timer_init(3, REC_PER_5S, 7199);
    timer_init(4, 1, 71);
    
    while (1) {
        check_rec_mode();
        check_rec_status();
        
        im948_parse_fifo_buf();
        
        if (im948_have_new_data) {
            im948_have_new_data = 0;
            Sdp("%.1f,%.1f,%.1f\r\n", im948_angle_x, im948_angle_y, im948_angle_z);
            ctrl_trigger_relay();
            // if (No == 0)
                // Usart3_Send(" No.,GpsDate,GpsTime,Latitude,Longitude,Altitude,Pitch,Roll,Yaw\r\n");
            // sprintf(u3_msg, "%d,%s,%s,%.12f,%.12f,%.2f,%.1f,%.1f,%.1f\r\n", No, GPS_Date, GPS_Utc, GPS_Latitude, GPS_Longitude, GPS_Msl, g_16Pitch / 10.0, g_16Roll / 10.0, g_16Yaw / 10.0);  // 更新所有參數
            // Usart3_Send(u3_msg);
            // ctrl_trigger_relay();
            sdcard_line_ctr = (sdcard_line_ctr + 1) % UINT32_MAX;
        }
        
    }
}
