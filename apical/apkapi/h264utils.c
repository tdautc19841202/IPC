#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int h264_parse_nalu_header(uint8_t *data, int len)
{
    int counter, i;
    for (counter=0,i=0; i<len; i++) {
        if (data[i] == 0) counter++;
        else if (counter >= 2 && data[i] == 0x01) {
            return i;
        } else {
            counter = 0;
        }
    }
    return -1;
}

int h264_parse_key_sps_pps(uint8_t *data, int len, int *key, uint8_t **sps_buf, int *sps_len, uint8_t **pps_buf, int *pps_len)
{
    uint8_t *sbuf, *pbuf;
    int slen, plen, type, i;

#if 0
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[0 ], data[1 ], data[2 ], data[3 ], data[4 ], data[5 ], data[6 ], data[7 ]);
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[8 ], data[9 ], data[10], data[11], data[12], data[13], data[14], data[15]);
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[16], data[17], data[18], data[19], data[20], data[21], data[22], data[23]);
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[24], data[25], data[26], data[27], data[28], data[29], data[30], data[31]);
    printf("\n");
#endif

    if (key    ) *key     = 0;
    if (sps_buf) *sps_buf = NULL;
    if (sps_len) *sps_len = 0;
    if (pps_buf) *pps_buf = NULL;
    if (pps_len) *pps_len = 0;
    i = h264_parse_nalu_header(data, len);
    if (i > 0 && i+1 < len && (data[i+1] & 0x1f) == 7) { // find sps
        len -= i + 1;
        data+= i + 1;
        i = h264_parse_nalu_header(data, len);
        if (i > 0) {
            sbuf = data;
            slen = i - 3;
            len -= i + 1;
            data+= i + 1;
        } else {
            goto find_frame_data;
        }
        if (len > 2 && (data[0] & 0x1f) == 8) { // find pps
            i = h264_parse_nalu_header(data, len);
            if (i > 0) {
                pbuf = data;
                plen = i - 2;
                len -= i - 2;
                data+= i - 2;
            } else {
                goto find_frame_data;
            }
        }
        if (sps_buf) *sps_buf = sbuf;
        if (sps_len) *sps_len = slen;
        if (pps_buf) *pps_buf = pbuf;
        if (pps_len) *pps_len = plen;
        if (key    ) *key     = 1;
    }

find_frame_data:
    while (1) {
        i = h264_parse_nalu_header(data, len);
        if (i < 0) break;
        data += i;
        len  -= i;
        if (len >= 2) {
            type = data[1] & 0x1f;
            if (type >= 1 && type <= 5) {
                return len - 1;
            }
        }
    }
    return 0;
}
