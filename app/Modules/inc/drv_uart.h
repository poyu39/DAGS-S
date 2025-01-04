#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#define DBP_UART_PORT           2  // debug uart
#define IM948_UART_PORT         0  // im948 uart

#include <stdio.h>
#include <stdint.h>

#include "stm32f10x.h"

#define FIFO_SIZE 200

typedef struct _uart_fifo_t {
    uint8_t rx_buf[FIFO_SIZE];
    volatile uint8_t in;
    volatile uint8_t out;
    volatile uint8_t cnt;
} uart_fifo_t;

extern uart_fifo_t uart_fifos[3];

// 如果開啟 Debug 將會從 DBP_UART_PORT 輸出除錯資訊
// #define __Debug
#ifdef __Debug
    #define Dbp(fmt, args...)  printf(fmt, ##args) // 若需要使用除錯資訊, 使用者對接Dbp函式名即可
#else
    #define Dbp(fmt, args...)
#endif

// 如果開啟 SDCard 將會從 DBP_UART_PORT 輸出 sdcard 格式資料
#define __SDCard
#ifdef __SDCard
    #define Sdp(fmt, args...)  printf(fmt, ##args) // 從 DBP_UART_PORT 輸出 sdcard 格式資料
#else
    #define Sdp(fmt, args...)
#endif

/**
 * 開啟串列埠
 * @param n             串列埠號(串列埠號為0~4，0~4對應CPU串列埠1~5);
 * @param speed         波特率;
 * @param databits      資料位數(8);
 * @param stopbits      停止位數(1或2);
 * @param parity        校驗位('n'=無  'o'=奇  'e'=偶);
 */
extern void uart_open(uint8_t n, uint32_t speed, uint8_t databits, uint8_t stopbits, uint8_t parity);

/**
 * Uart同步傳送資料，等待傳送完畢
 * @param n 串列埠號
 * @param buf 要傳送的資料指標
 * @param Len 資料長度
 * @return int 0=成功, -1=失敗
 */
extern int uart_write(uint8_t n, const uint8_t *buf, int Len);

#endif
