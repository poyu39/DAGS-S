#include "drv_Uart.h"

uart_fifo_t uart_fifos[3];

int fputc(int ch, FILE *f) {
#if (DBP_UART_PORT == 0)
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
#elif (DBP_UART_PORT == 1)
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, ch);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
#elif (DBP_UART_PORT == 2)
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, ch);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
#endif
    return ch;
}

void uart_open(uint8_t n, uint32_t speed, uint8_t databits, uint8_t stopbits, uint8_t parity) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    uint32_t rDatabits, rParity, rStopbits;
    
    databits = databits;
    rStopbits = (stopbits == 2) ? USART_StopBits_2 : USART_StopBits_1;
    switch (parity) {
    case 'o':
    case 'O':
    case 1:
        rDatabits = USART_WordLength_9b;
        rParity = USART_Parity_Odd;
        break;
    case 'e':
    case 'E':
    case 2:
        rDatabits = USART_WordLength_9b;
        rParity = USART_Parity_Even;
        break;
    default:
        rDatabits = USART_WordLength_8b;
        rParity = USART_Parity_No;
        break;
    }
    
    switch (n) {
    case 0: // 串列埠0 TXD->PA9, RXD->PA10
        USART_DeInit(USART1);
        
        // 設定對應的GPIO
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        // 設定 USART1 mode
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        USART_InitStructure.USART_BaudRate = speed;
        USART_InitStructure.USART_WordLength = rDatabits;
        USART_InitStructure.USART_StopBits = rStopbits;
        USART_InitStructure.USART_Parity = rParity;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART1, &USART_InitStructure);
        
        // 設為接收中斷
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        USART_Cmd(USART1, ENABLE);
        break;
    case 1:  // 串列埠1 TXD->PA2, RXD->PA3 或 TXD->PD5, RXD->PD6
        USART_DeInit(USART2);
        
        // 設定對應的GPIO
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        // 設定 USART2 mode
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        USART_InitStructure.USART_BaudRate = speed;
        USART_InitStructure.USART_WordLength = rDatabits;
        USART_InitStructure.USART_StopBits = rStopbits;
        USART_InitStructure.USART_Parity = rParity;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART2, &USART_InitStructure);
        
        // 設為接收中斷
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        USART_Cmd(USART2, ENABLE);
        break;
    case 2:  // 串列埠2 TXD->PB10, RXD->PB11
        USART_DeInit(USART3);
        
        // 設定對應的GPIO
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
        // 設定 USART3 mode
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        USART_InitStructure.USART_BaudRate = speed;
        USART_InitStructure.USART_WordLength = rDatabits;
        USART_InitStructure.USART_StopBits = rStopbits;
        USART_InitStructure.USART_Parity = rParity;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART3, &USART_InitStructure);
        
        // 設為接收中斷
        USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        USART_Cmd(USART3, ENABLE);
        break;
    default:
        break;
    }
    Dbp("open uart%d OK\r\n", n);
}


int uart_write(uint8_t n, const uint8_t *buf, int Len) {
    int i;
    switch (n) {
    case 0:  // 串列埠1
        for (i = 0; i < Len; i++) {
            while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
            USART_SendData(USART1, buf[i]);
        }
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        break;
    case 1:  // 串列埠2
        for (i = 0; i < Len; i++) {
            while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
            USART_SendData(USART2, buf[i]);
        }
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        break;
    case 2:  // 串列埠3
        for (i = 0; i < Len; i++) {
            while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
            USART_SendData(USART3, buf[i]);
        }
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
        break;
    
    default:
        return 0;
    }
    
    return Len;
}

static void fifo_in(uart_fifo_t* uart_fifo, uint8_t rx_byte) {
    if (FIFO_SIZE > uart_fifo->cnt) {
        uart_fifo->rx_buf[uart_fifo->in] = rx_byte;
        if (++uart_fifo->in >= FIFO_SIZE) {
            uart_fifo->in = 0;
        }
        ++uart_fifo->cnt;
    }
}

void USART1_IRQHandler(void) {
    if (USART_GetFlagStatus(USART1, USART_IT_RXNE) == SET) {
        uint16_t rx_byte = USART_ReceiveData(USART1);
        fifo_in(&uart_fifos[0], rx_byte);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

void USART2_IRQHandler(void) {
    if (USART_GetFlagStatus(USART2, USART_IT_RXNE) == SET) {
        uint16_t rx_byte = USART_ReceiveData(USART2);
        fifo_in(&uart_fifos[1], rx_byte);
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

void USART3_IRQHandler(void) {
    if (USART_GetFlagStatus(USART3, USART_IT_RXNE) == SET) {
        uint16_t rx_byte = USART_ReceiveData(USART3);
        fifo_in(&uart_fifos[2], rx_byte);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
