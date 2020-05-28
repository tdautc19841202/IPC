#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define SARADC_IOC_MAGIC                'a'
#define MS_SAR_INIT                     _IO(SARADC_IOC_MAGIC, 0)
#define MS_SAR_SET_CHANNEL_READ_VALUE   _IO(SARADC_IOC_MAGIC, 1)

typedef struct {
    int fd;
} SARADC;

typedef struct {
    int channel;
    int adcval ;
} ADC_CONFIG_READ_ADC;

void* saradc_init(void)
{
    SARADC *saradc = malloc(sizeof(SARADC));
    if (saradc) {
        saradc->fd = open("/dev/sar", O_RDWR);
        if (saradc->fd >= 0) {
            ioctl(saradc->fd, MS_SAR_INIT, 0);
            return saradc;
        } else {
            free(saradc);
        }
    }
    return NULL;
}

void saradc_exit(void *ctxt)
{
    SARADC *saradc = (SARADC*)ctxt;
    if (!saradc) return;
    if (saradc->fd >= 0) close(saradc->fd);
    free(saradc);
}

int saradc_read(void *ctxt, int ch)
{
    SARADC *saradc = (SARADC*)ctxt;
    if (!saradc) return -1;
    if (saradc->fd >= 0) {
        ADC_CONFIG_READ_ADC adccfg = { ch };
        ioctl(saradc->fd, MS_SAR_SET_CHANNEL_READ_VALUE, &adccfg);
        return adccfg.adcval;
    }
    return -1;
}
