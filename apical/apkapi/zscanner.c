#include <stdlib.h>
#include <stdio.h>
#include "zbar.h"
#include "zscanner.h"

typedef struct {
    int w;
    int h;
    zbar_image_scanner_t *scanner;
    zbar_image_t         *image  ;
} CONTEXT;

static void zbar_image_free(zbar_image_t *image) { /* do nothing */ }

void* zscanner_init(int w, int h, int crop_x, int crop_y, int crop_w, int crop_h)
{
    CONTEXT *context = calloc(1, sizeof(CONTEXT));
    if (!context) return context;

    /* zbar scanner init */
    context->scanner = zbar_image_scanner_create();
    zbar_image_scanner_set_config(context->scanner, ZBAR_NONE  , ZBAR_CFG_ENABLE, 0);
    zbar_image_scanner_set_config(context->scanner, ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);

    /* wrap image data */
    context->image = zbar_image_create();
    zbar_image_set_format(context->image, *(int*)"Y800");
    zbar_image_set_size  (context->image, w, h);
    zbar_image_set_crop  (context->image, crop_x, crop_y, crop_w, crop_h);
    context->w = w;
    context->h = h;
    return context;
}

void zscanner_exit(void *ctxt)
{
    CONTEXT *context = (CONTEXT*)ctxt;
    if (!context) return;
    /* zbar scanner clean up */
    zbar_image_destroy(context->image);
    zbar_image_scanner_destroy(context->scanner);
}

char* zscanner_scan(void *ctxt, char *image)
{
    CONTEXT *context = (CONTEXT*)ctxt;
    const zbar_symbol_t *symbol = NULL;
    char *data = NULL;
    int   ret;
    if (!context) return NULL;

    /* set zbar image data */
    zbar_image_set_data(context->image, image, context->w * context->h, zbar_image_free);

    /* scan the image for barcodes */
    ret = zbar_scan_image(context->scanner, context->image);
    if (ret <= 0) return data;

    /* extract results */
    symbol = zbar_image_first_symbol(context->image);
    for(; symbol; symbol = zbar_symbol_next(symbol)) {
        /* do something useful with results */
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        data = (char*)zbar_symbol_get_data(symbol);
        printf("decoded %s symbol \"%s\"\n", zbar_get_symbol_name(typ), data);
    }

    return data;
}

