#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdio.h>
#include <stdint.h>
#include "stm32f10x.h"
#include "controller.h"


/**
 * 初始化計時器
 * @param num 計時器編號
 * @param arr 自動重裝載寄存器的週期值
 * @param psc 預分頻值
 */
void timer_init(uint8_t num, uint16_t arr, uint16_t psc);

void timer_set_rec_prd(uint16_t prd);

#endif
