#ifndef __IM948_H__
#define __IM948_H__

#include <string.h>
#include "drv_uart.h"

extern float im948_angle_x, im948_angle_y, im948_angle_z;
extern uint8_t im948_have_new_data;

// ================================ģ�M�Ĳ���ָ��=================================

extern void im948_cmd_02(void);           // ˯�߸Мy��
extern void im948_cmd_03(void);           // ���ѸМy��
extern void im948_cmd_18(void);           // �P�]�Y�������ψ�
extern void im948_cmd_19(void);           // �_���Y�������ψ�
extern void im948_cmd_11(void);           // �@ȡӆ醵��Y��

/**
 * �O�� IM948 ����
 * @param acc_still             �T��-�oֹ��B���ٶ��yֵ ��λdm/s?
 * @param still_to_zero         �T��-�oֹ�w���ٶ�(��λcm/s) 0:���w�� 255:�����w��
 * @param move_to_zero          �T��-�ӑB�w���ٶ�(��λcm/s) 0:���w��
 * @param is_compass_on         1=���ںϴň� 0=���ںϴň�
 * @param barometer_filter      �≺Ӌ�ĞV���ȼ�[ȡֵ0-3],��ֵԽ��Խƽ�������r��Խ��
 * @param report_hz             �Y�������ψ�Ă�ݔ����[ȡֵ0-250HZ], 0��ʾ0.5HZ
 * @param gyro_filter           ���݃x�V���S��[ȡֵ0-2],��ֵԽ��Խƽ�������r��Խ��
 * @param acc_filter            ����Ӌ�V���S��[ȡֵ0-4],��ֵԽ��Խƽ�������r��Խ��
 * @param compass_filter        ����Ӌ�V���S��[ȡֵ0-9],��ֵԽ��Խƽ�������r��Խ��
 * @param cmd_report_tag        ����ӆ醘��R
 */
extern void im948_cmd_12(uint8_t acc_still, uint8_t still_to_zero, uint8_t move_to_zero, uint8_t is_compass_on, uint8_t barometer_filter, uint8_t report_hz, uint8_t gyro_filter, uint8_t acc_filter, uint8_t compass_filter, uint16_t cmd_report_tag);


extern void im948_parse_fifo_buf(void);

#endif
