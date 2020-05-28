#ifndef __FFT_H__
#define __FFT_H__

void *fft_init(int n);
void  fft_free(void *c);
void  fft_execute(void *c, float *in, float *out);

#endif



