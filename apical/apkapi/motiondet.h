#ifndef __MOTIONDET_H__
#define __MOTIONDET_H__

#define MD_VIDEO_WIDTH   256
#define MD_VIDEO_HEIGHT  160

void* motion_detect_init(int sensitivity);
void  motion_detect_exit(void *ctxt);
int   motion_detect_run (void *ctxt, char *data, int td); // td - time diff in ms between current frame and last frame
void  motion_detect_sensitivity(void *ctxt, int sensitivity);

#endif
