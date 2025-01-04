#include "exti.h"

void EXTIX_Init(void) {
    /******** �ŧi EXTI�BNVIC ���c�� ********/
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /******** �]�w PA0 �� ���_�ӷ� ********/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    /******** EXTI0 �򥻰ѼƳ]�w ********/
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* �ҥ�	EXTI0 ���_ */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// void EXTI0_IRQHandler(void) {
//     pwm_count = 0;                              // Count�k�s
//     TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);  // �ҥ�Timer4
//     EXTI_ClearITPendingBit(EXTI_Line0);
// }
