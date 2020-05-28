#ifndef __H264UTILS_H__
#define __H264UTILS_H__

#include <stdint.h>

int h264_parse_key_sps_pps(uint8_t *data, int len, int *key, uint8_t **sps_buf, int *sps_len, uint8_t **pps_buf, int *pps_len);

#endif

