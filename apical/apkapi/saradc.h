#ifndef __SARADC_H__
#define __SARADC_H__

void* saradc_init(void);
void  saradc_exit(void *ctxt);
int   saradc_read(void *ctxt, int ch);

#endif
