#ifndef __BTN_H__
#define __BTN_H__

#include "stm32f10x.h"
#include "stdio.h"

#define REC_PER_1S 9999
#define REC_PER_2S 19999
#define REC_PER_3S 29999
#define REC_PER_5S 49999

#define PINA1 PAout(1)
#define PIN0 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)
#define PIN1 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)
#define PIN12 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)
#define EXTI0 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)
#define RECLED PBout(13)
#define RELAY PBout(9)

extern volatile int rec_signal;
extern volatile int rec_mode, pwm_count;

void RecButton_Init(void);
void LED_Init(void);

#endif
