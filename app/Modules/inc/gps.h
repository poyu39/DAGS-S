#ifndef __GPS_H__
#define __GPS_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"
#include "drv_uart.h"

extern char   gps_utc[12];     // UTC
extern float  gps_latitude;    // 緯度
extern float  gps_longitude;   // 經度
extern float  gps_msl;         // 海拔

extern void gps_parse_fifo_buf(void);

#endif
