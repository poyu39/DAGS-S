#ifndef __IM948_H__
#define __IM948_H__

#include <string.h>
#include "drv_uart.h"

extern float im948_angle_x, im948_angle_y, im948_angle_z;
extern uint8_t im948_have_new_data;

// ================================模組的操作指令=================================

extern void im948_cmd_02(void);           // 睡眠感測器
extern void im948_cmd_03(void);           // 喚醒感測器
extern void im948_cmd_18(void);           // 關閉資料主動上報
extern void im948_cmd_19(void);           // 開啟資料主動上報
extern void im948_cmd_11(void);           // 獲取訂閱的資料

/**
 * 設定 IM948 參數
 * @param acc_still             慣導-靜止狀態加速度閥值 單位dm/s?
 * @param still_to_zero         慣導-靜止歸零速度(單位cm/s) 0:不歸零 255:立即歸零
 * @param move_to_zero          慣導-動態歸零速度(單位cm/s) 0:不歸零
 * @param is_compass_on         1=需融合磁場 0=不融合磁場
 * @param barometer_filter      氣壓計的濾波等級[取值0-3],數值越大越平穩但實時性越差
 * @param report_hz             資料主動上報的傳輸幀率[取值0-250HZ], 0表示0.5HZ
 * @param gyro_filter           陀螺儀濾波係數[取值0-2],數值越大越平穩但實時性越差
 * @param acc_filter            加速計濾波係數[取值0-4],數值越大越平穩但實時性越差
 * @param compass_filter        磁力計濾波係數[取值0-9],數值越大越平穩但實時性越差
 * @param cmd_report_tag        功能訂閱標識
 */
extern void im948_cmd_12(uint8_t acc_still, uint8_t still_to_zero, uint8_t move_to_zero, uint8_t is_compass_on, uint8_t barometer_filter, uint8_t report_hz, uint8_t gyro_filter, uint8_t acc_filter, uint8_t compass_filter, uint16_t cmd_report_tag);


extern void im948_parse_fifo_buf(void);

#endif
