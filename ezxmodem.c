#include "ezxmodem.h"
#include "stdlib.h"

#include "stdio.h"
#define XM_SOH 0x01
#define XM_EOT 0x04
#define XM_ACK 0x06
#define XM_NAK 0x15
#define XM_CAN 0x18
#define XM_C   0x43
#define XMODEM_BUF_LEN 150


xmodem_rev_trans_t xmodem_start(ezrb_t *rb, xmodem_cfg_t *cfg);
static uint16_t crc16_xmodem(uint8_t *data, uint16_t length);


static uint16_t crc16_xmodem(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0;            // Initial value
    while(length--)
    {
        crc ^= (uint16_t)(*data++) << 8; // crc ^= (uint16_t)(*data)<<8; data++;
        for (i = 0; i < 8; ++i)
        {
            if ( crc & 0x8000 )
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}


/**
 * @brief
 *
 * @param rb
 * @param frame_cb callback(char*) when get an frame(128-bytes) if char* is NULL , xmodem finish
 * @param delay_ms xmodem_start can delay n ms by calling it
 */
xmodem_rev_trans_t xmodem_start(ezrb_t *rb, xmodem_cfg_t *cfg)
{
    static uint8_t wait_final_eot = 0;
    uint8_t buf[XMODEM_BUF_LEN];
    uint8_t bufp = 0;
    uint8_t sendbuf;
    uint16_t timeout = 0;
    uint8_t last_packet_num = 0;
    sendbuf = XM_C;
    ezport_send_str(&sendbuf, 1);

    while (1) {
        if (ezrb_pop(rb, buf + bufp) == RB_EMPTY) {
            cfg->delay_ms(1);
            timeout++;
            if (timeout > 3000) {
                return X_TRANS_TIMEOUT;
            }
        } else {
            timeout = 0;
            bufp++;
            if (bufp == 133) { // frame size
                if (buf[0] == XM_SOH && buf[1] == (uint8_t)(last_packet_num + 1) && buf[1] == (~buf[2])) {
                    if (crc16_xmodem(buf, 3 + 128) == ((buf[131] << 8) | buf[132])) {
                        bufp = 0;
                        switch (cfg->frame_cb(buf + 3)) {
                        case X_SEND_NEXT:
                            sendbuf = XM_ACK;
                            ezport_send_str(&sendbuf, 1);
                            break;
                        case X_SEND_REPEAT:
                            sendbuf = XM_NAK;
                            ezport_send_str(&sendbuf, 1);
                            break;
                        case X_SEND_ABORT:
                        default:
                            sendbuf = XM_CAN;
                            ezport_send_str(&sendbuf, 1);
                            break;
                        };
                    } else {
                        /* repeat when crc wrong */
                        sendbuf = XM_NAK;
                        ezport_send_str(&sendbuf, 1);
                    }
                } else {
                    /* abort when frame goes wrong */
                    sendbuf = XM_CAN;
                    ezport_send_str(&sendbuf, 1);
                }
            } else if (bufp == 1 && buf[0] == XM_EOT) {
                sendbuf = XM_ACK;
                ezport_send_str(&sendbuf, 1);
                if (wait_final_eot == 0) {
                    wait_final_eot == 1;
                } else {
                    wait_final_eot = 0;
                    sendbuf = XM_ACK;
                    ezport_send_str(&sendbuf, 1);
                    cfg->frame_cb(NULL);
                    return X_TRANS_OK;
                }
            }
        }
    }
    return X_TRANS_OK;
}
