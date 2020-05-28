#ifndef __MP3DEC_H__
#define __MP3DEC_H__

void* mp3dec_init(void);
void  mp3dec_exit(void *ctxt);
void  mp3dec_play_file(void *ctxt, char *file);

#endif

