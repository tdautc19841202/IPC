#ifndef __ZSCANNER_H__
#define __ZSCANNER_H__

#define ZBAR_VIDEO_WIDTH   704
#define ZBAR_VIDEO_HEIGHT  396

void* zscanner_init(int w, int h, int crop_x, int crop_y, int crop_w, int crop_h);
void  zscanner_exit(void *ctxt);
char* zscanner_scan(void *ctxt, char *image);

#endif
