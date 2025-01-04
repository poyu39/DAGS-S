#include "im948.h"

#define pow2(x) ((x)*(x)) // 求平方

// 轉換常數
#define SCALE_ACCEL             0.00478515625f      // 加速度 [-16g~+16g]    9.8*16/32768
#define SCALE_QUAT              0.000030517578125f  // 四元數 [-1~+1]         1/32768
#define SCALE_ANGLE             0.0054931640625f    // 角度   [-180~+180]     180/32768
#define SCALE_ANGLE_SPEED       0.06103515625f      // 角速度 [-2000~+2000]    2000/32768
#define SCALE_MAG               0.15106201171875f   // 磁場 [-4950~+4950]   4950/32768
#define SCALE_TEMPERATURE       0.01f               // 溫度
#define SCALE_AIR_PRESSURE      0.0002384185791f    // 氣壓 [-2000~+2000]    2000/8388608
#define SCALE_HEIGHT            0.0010728836f       // 高度 [-9000~+9000]    9000/8388608

#define CmdPacket_Begin         0x49                // 起始碼
#define CmdPacket_End           0x4D                // 結束碼
#define CmdPacketMaxDatSizeRx   73                  // 模組發來的   資料包的資料體最大長度
#define CmdPacketMaxDatSizeTx   31                  // 傳送給模組的 資料包的資料體最大長度


uint8_t targetDeviceAddress = 0xFF;

float im948_angle_x = 0, im948_angle_y = 0, im948_angle_z = 0;
uint8_t im948_have_new_data = 0;

uint8_t cal_sum(uint8_t *buf, int len) {
    uint8_t sum = 0;
    while (len-- > 0) {
        sum += buf[len];
    }
    return sum;
}

/**
 * 解析 rx 封包
 * @param buf 封包資料體
 */
static void cmd_rx_unpack(uint8_t *buf) {
    uint16_t ctl;
    uint8_t L;
    float tmpX, tmpY, tmpZ;
    
    switch (buf[0]) {
    // 感測器 已睡眠 回覆
    case 0x02:
        Dbp("\t sensor off success\r\n");
        break;
    
    // 感測器 已喚醒 回覆
    case 0x03:
        Dbp("\t sensor on success\r\n");
        break;
    
    // 獲取訂閱的功能資料 回覆或主動上報
    case 0x11:
        // 位元組[2-1] 為功能訂閱標識，指示當前訂閱了哪些功能
        ctl = ((uint16_t)buf[2] << 8) | buf[1];
        
        // 從第7字節開始根據 訂閱標識 tag 來解析剩下的數據
        L = 7;
        
        // 尤拉角xyz 使用時需*scaleAngle
        if ((ctl & 0x0040) != 0) {
            tmpX = (int16_t)(((int16_t)buf[L + 1] << 8) | buf[L]) * SCALE_ANGLE;
            L += 2;
            Dbp("\tangleX: %.3f\r\n", tmpX);  // x角度
            tmpY = (int16_t)(((int16_t)buf[L + 1] << 8) | buf[L]) * SCALE_ANGLE;
            L += 2;
            Dbp("\tangleY: %.3f\r\n", tmpY);  // y角度
            tmpZ = (int16_t)(((int16_t)buf[L + 1] << 8) | buf[L]) * SCALE_ANGLE;
            L += 2;
            Dbp("\tangleZ: %.3f\r\n", tmpZ);  // z角度
            im948_angle_x = tmpX;
            im948_angle_y = tmpY;
            im948_angle_z = tmpZ;
        }
        im948_have_new_data = 1;  // 更新了新資料
        break;
    case 0x12:  // 設定引數 回覆
        Dbp("\t set parameters success\r\n");
        break;
    case 0x18:  // 已關閉主動上報 回覆
        Dbp("\t auto report off\r\n");
        break;
    case 0x19:  // 已開啟主動上報 回覆
        Dbp("\t auto report on\r\n");
        break;
    default:
        break;
    }
}

/**
 * 傳送指令
 * @param data 要傳送的資料體
 * @param len 資料體的長度
 * @return int 0=成功, -1=失敗
 */
static int cmd_pack_and_tx(uint8_t *data, uint8_t len) {
    // uint8_t buf[50 + 5 + CmdPacketMaxDatSizeTx] =
    //     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff};  // 傳送包快取 開頭50位元組是前導碼，用於喚醒可能處於睡眠狀態的模組
    uint8_t buf[50 + 5 + CmdPacketMaxDatSizeTx];
    memset(buf, 0, sizeof(buf));
    // 前導碼
    buf[47] = 0xff;
    buf[49] = 0xff;
    
    if ((len == 0) || (len > CmdPacketMaxDatSizeTx) || (data == NULL)) return -1;
    
    buf[50] = CmdPacket_Begin;                      // 起始碼
    buf[51] = targetDeviceAddress;                  // 目前裝置地址碼
    buf[52] = len;                                 // 長度
    // memcpy(&buf[53], pDat, DLen);
    memcpy(&buf[53], data, len);                   // 資料體
    buf[53 + len] = cal_sum(&buf[51], len + 2);   // CS 從 地址碼開始算到資料體結束
    buf[54 + len] = CmdPacket_End;                 // 結束碼
    
    uart_write(IM948_UART_PORT, buf, len + 55);
    return 0;
}

/**
 * 組裝資料封包
 * @param byte 傳入接收到的每位元組資料
 * @return uint8_t 1=接收到完整封包, 0未獲取到完整封包
 */
static uint8_t cmd_get_pkt(uint8_t byte) {
    static uint8_t CS = 0;  // 校驗和
    static uint8_t i = 0;
    static uint8_t RxIndex = 0;
    static uint8_t buf[5 + CmdPacketMaxDatSizeRx];  // 接收包快取
    
    #define cmdBegin buf[0]         // 起始碼
    #define cmdAddress buf[1]       // 通訊地址
    #define cmdLen buf[2]           // 長度
    #define cmdDat &buf[3]          // 資料體
    #define cmdCS buf[3 + cmdLen]   // 校驗和
    #define cmdEnd buf[4 + cmdLen]  // 結束碼
    
    CS += byte;  // 邊收資料邊計算校驗碼，校驗碼為地址碼開始(包含地址碼)到校驗碼之前的資料的和
    switch (RxIndex) {
    case 0:  // 起始碼
        if (byte == CmdPacket_Begin) {
            i = 0;
            buf[i++] = CmdPacket_Begin;
            CS = 0;  // 下個位元組開始計算校驗碼
            RxIndex = 1;
        }
        break;
    case 1:  // 資料體的地址碼
        buf[i++] = byte;
        if (byte == 255) {  // 255是廣播地址，模組作為從機，它的地址不可會出現255
            RxIndex = 0;
            break;
        }
        RxIndex++;
        break;
    case 2:  // 資料體的長度
        buf[i++] = byte;
        if ((byte > CmdPacketMaxDatSizeRx) || (byte == 0)) {  // 長度無效
            RxIndex = 0;
            break;
        }
        RxIndex++;
        break;
    case 3:  // 獲取資料體的資料
        buf[i++] = byte;
        if (i >= cmdLen + 3) {  // 已收完資料體
            RxIndex++;
        }
        break;
    case 4:  // 對比 效驗碼
        CS -= byte;
        if (CS == byte) {  // 校驗正確
            buf[i++] = byte;
            RxIndex++;
        } else {  // 校驗失敗
            RxIndex = 0;
        }
        break;
    case 5:  // 結束碼
        RxIndex = 0;
        if (byte == CmdPacket_End) {  // 捕獲到完整包
            buf[i++] = byte;
            
            if ((targetDeviceAddress == cmdAddress) || (targetDeviceAddress == 255)) {  // 地址匹配，是目標裝置發來的資料 才處理
                Dbp("\r\nrx: ");
                for (int j = 0; j < i; j++) Dbp("%02X ", buf[j]);
                Dbp("\r\n");
                cmd_rx_unpack(&buf[3]);  // 處理資料包的資料體
                return 1;
            }
        }
        break;
    default:
        RxIndex = 0;
        break;
    }
    
    return 0;
}

// 睡眠感測器
void im948_cmd_02(void) {
    uint8_t buf[1] = {0x02};
    Dbp("\r\nsensor off--\r\n");
    cmd_pack_and_tx(buf, 1);
}

// 喚醒感測器
void im948_cmd_03(void) {
    uint8_t buf[1] = {0x03};
    Dbp("\r\nsensor on--\r\n");
    cmd_pack_and_tx(buf, 1);
}

// 獲取1次訂閱的功能數據
void im948_cmd_11(void) {
    uint8_t buf[1] = {0x11};
    Dbp("\r\nget report--\r\n");
    cmd_pack_and_tx(buf, 1);
}

// 關閉資料主動上報
void im948_cmd_18(void) {
    uint8_t buf[1] = {0x18};
    Dbp("\r\nauto report off--\r\n");
    cmd_pack_and_tx(buf, 1);
}

// 開啟資料主動上報
void im948_cmd_19(void) {
    uint8_t buf[1] = {0x19};
    Dbp("\r\nauto report on--\r\n");
    cmd_pack_and_tx(buf, 1);
}

void im948_cmd_12(uint8_t acc_still,
                  uint8_t still_to_zero,
                  uint8_t move_to_zero,
                  uint8_t is_compass_on,
                  uint8_t barometer_filter,
                  uint8_t report_hz,
                  uint8_t gyro_filter,
                  uint8_t acc_filter,
                  uint8_t compass_filter,
                  uint16_t cmd_report_tag) {
    uint8_t buf[11] = {0x12};
    buf[1] = acc_still;
    buf[2] = still_to_zero;
    buf[3] = is_compass_on;
    buf[4] = ((barometer_filter & 3) << 1) | (is_compass_on & 1);  // bit[2-1]: BMP280的濾波等級[取值0-3]   bit[0]: 1=已開啟磁場 0=已關閉磁場
    buf[5] = report_hz;
    buf[6] = gyro_filter;
    buf[7] = acc_filter;
    buf[8] = compass_filter;
    buf[9] = cmd_report_tag & 0xff;
    buf[10] = (cmd_report_tag >> 8) & 0xff;
    Dbp("\r\nset parameters--\r\n");
    cmd_pack_and_tx(buf, 11);
}

void im948_parse_fifo_buf(void) {
    uint8_t rx_byte;
    while (uart_fifos[IM948_UART_PORT].cnt > 0) {// 從fifo獲取串列埠發來的資料
        rx_byte = uart_fifos[IM948_UART_PORT].rx_buf[uart_fifos[IM948_UART_PORT].out];
        if (++uart_fifos[IM948_UART_PORT].out >= FIFO_SIZE) {
            uart_fifos[IM948_UART_PORT].out = 0;
        }
        __disable_irq();// 關中斷
        --uart_fifos[IM948_UART_PORT].cnt;
        __enable_irq();// 開中斷
        
        if (cmd_get_pkt(rx_byte)) break;
    }
}
