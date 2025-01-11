#include "timer.h"

void timer_init(uint8_t num, uint16_t arr, uint16_t psc) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 通用設定
    TIM_TimeBaseStructure.TIM_Period = arr;                      	// 設置自動重裝載寄存器的週期值，裝載到下一個更新事件
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                   	// 設置預分頻值，作為 TIMx 時鐘頻率的分頻器
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      	// 設置時鐘分頻：TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	// 設置 TIM 為向上計數模式
    
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         		// 設置子優先級為 3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            		// 使能 IRQ 通道
    if (num == 3) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
        
    } else if (num == 4) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
        TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    }
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //IRQ channel enable
    NVIC_Init(&NVIC_InitStructure);                            		// 初始化 NVIC 寄存器
    
    if (num == 3) {
        TIM_Cmd(TIM3, ENABLE);
    } else if (num == 4) {
        TIM_Cmd(TIM4, ENABLE);
    }
}

// 設置錄製頻率
void timer_set_rec_prd(uint16_t prd)  {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = prd;                      // 設置自動重裝載寄存器的週期值，裝載到下一個更新事件
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;                  // 設置預分頻值，作為 TIMx 時鐘頻率的分頻器
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // 設置時鐘分頻：TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 設置 TIM 為向上計數模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              // 根據指定參數初始化 TIMx 的時間基數單元
}
