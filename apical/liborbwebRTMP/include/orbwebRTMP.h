#ifndef __ORBWEBRTMP_H__
#define __ORBWEBRTMP_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Start RTMP service
 *
 * @param url   Assigned RTMP push URL
 *
 * @return int  0:success -1:fail
 */
int rtmpServiceStart(char * url);

/*
 * Stop RTMP service
 *
 */
void rtmpServiceStop(void);

/*
 * Put H.264 frame data to RTMP service
 *
 * @param frame_data    Original H.264 data
 * @param len   Original size of H.264 data
 * @param ts    Assigned Timestamp(ms) of the frame
 * @param frameType Original H.264 Nal type
 *
 */
void put_video_to_rtmp(unsigned char *frame_data, uint32_t len, uint32_t ts, int frameType);

/*
 * Put G711 alaw audio data to RTMP service
 *
 * @param frame_data    Original G711 alaw data
 * @param len   Original size of G711 alaw data
 * @param ts    Assigned Timestamp(ms) of the frame
 *
 */
void put_audio_to_rtmp(unsigned char * frame_data, uint32_t len, uint32_t ts);

#ifdef __cplusplus
}
#endif

#endif

