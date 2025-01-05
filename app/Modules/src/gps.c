#include "gps.h"

#define GPS_DATA_MAX_LEN 128

char gps_data[GPS_DATA_MAX_LEN] = "";
volatile uint8_t gps_rx_done = 0;

char   gps_utc[12]     = "";       // UTC
float  gps_latitude    = 0;        // 絥
float  gps_longitude   = 0;        // 竒
float  gps_msl         = 0;        // ┺


static void gps_parse_data(void) {
    if (gps_rx_done == 1) {
        char *token = strtok(gps_data, ",");
        uint8_t field = 0;
        uint8_t is_rmc = 0, is_gga = 0;
        
        // 戈璶秆猂 gga rmc ノ穝丁
        while (token != NULL) {
            // Sdp("token: %s\r\n", token);
            switch (field) {
            case 0: // 家Α
                if (strcmp(token, "$GNRMC") == 0) is_rmc = 1;
                if (strcmp(token, "$GNGGA") == 0) is_gga = 1;
                break;
            case 1: // UTC
                if (is_gga) sprintf(gps_utc, "%s", token);
                break;
            case 2: // 絥
                if (is_gga) {
                    int la_pre, la_pas;
                    gps_latitude = atof(token);
                    gps_latitude /= 100.0;           // 计翴玡簿ㄢ
                    la_pre = gps_latitude;           // 絥俱计场だ
                    la_pas = gps_latitude - la_pre;  // 絥计场だ
                    la_pas = la_pas / 6.0 * 10.0;    // 计场だ传衡
                    gps_latitude = la_pre + la_pas;  // 计传衡Ч俱计Google Mapぇ畒夹
                }
            case 4: // 竒
                if (is_gga) {
                    int lo_pre, lo_pas;
                    gps_longitude = atof(token);
                    gps_longitude /= 100.0;
                    lo_pre = gps_longitude;
                    lo_pas = gps_longitude - lo_pre;
                    lo_pas = lo_pas / 6.0 * 10.0;
                    gps_longitude = lo_pre + lo_pas;  // 絥ぇ传衡よΑ
                    break;
                }
                break;
            case 9: // ┺
                if (is_gga) gps_msl = atof(token);
                break;
            default:
                break;
            }
            field++;
            token = strtok(NULL, ",");
        }
        gps_rx_done = 0;
    }
}

static int get_pkt(uint8_t rx_byte) {
    // 睲 gps_data
    static uint8_t index = 0;
    if (index == 0) memset(gps_data, 0, strlen(gps_data));
    
    if (rx_byte == '\n') return 1;
    
    if (rx_byte != '\r') {
        sprintf(gps_data, "%s%c", gps_data, rx_byte);
        index++;
        return 0;
    } else {
        gps_rx_done = 1;
        index = 0;
        gps_parse_data();
        return 1;
    }
}

void gps_parse_fifo_buf() {
    uint8_t rx_byte;
    while (uart_fifos[GPS_UART_PORT].cnt > 0) {// 眖fifo莉﹃梆祇ㄓ戈
        rx_byte = uart_fifos[GPS_UART_PORT].rx_buf[uart_fifos[GPS_UART_PORT].out];
        if (++uart_fifos[GPS_UART_PORT].out >= FIFO_SIZE) {
            uart_fifos[GPS_UART_PORT].out = 0;
        }
        __disable_irq();// 闽い耞
        --uart_fifos[GPS_UART_PORT].cnt;
        __enable_irq();// 秨い耞
        
        if (get_pkt(rx_byte)) break;
    }
}
