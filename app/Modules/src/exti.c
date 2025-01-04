#include "exti.h"

void EXTIX_Init(void) {
    /******** 宣告 EXTI、NVIC 結構體 ********/
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /******** 設定 PA0 為 中斷來源 ********/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    /******** EXTI0 基本參數設定 ********/
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* 啟用	EXTI0 中斷 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// void EXTI0_IRQHandler(void) {
//     pwm_count = 0;                              // Count歸零
//     TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);  // 啟用Timer4
//     EXTI_ClearITPendingBit(EXTI_Line0);
// }
