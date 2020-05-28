#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include "mi_ao.h"
#include "wavapi.h"
#include "mp3dec.h"
#include "mad.h"

typedef struct {
    struct mad_decoder decoder;
    uint8_t *mp3_buffer;
    int      mp3_buflen;
    int16_t *pcm_buffer;
    int      pcm_buflen;
} CONTEXT;

void* mp3dec_init(void)
{
	CONTEXT *context = calloc(1, sizeof(CONTEXT));
    if (!context) return NULL;
    else{
		return context;
	}
}

void mp3dec_exit(void *ctxt)
{
    CONTEXT *context = (CONTEXT*)ctxt;
    if (!context) return;
    mad_decoder_finish(&context->decoder);
    if (context->pcm_buffer) free(context->pcm_buffer);
    free(context);
}

/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */
static enum mad_flow input(void *data, struct mad_stream *stream)
{
    CONTEXT *context = data;
    if (!context->mp3_buflen) return MAD_FLOW_STOP;
    mad_stream_buffer(stream, context->mp3_buffer, context->mp3_buflen);
    context->mp3_buflen = 0;
    return MAD_FLOW_CONTINUE;
}

/*
 * The following utility routine performs simple rounding, clipping, and
 * scaling of MAD's high-resolution samples down to 16 bits. It does not
 * perform any dithering or noise shaping, which would be recommended to
 * obtain any exceptional audio quality. It is therefore not recommended to
 * use this routine if high-quality output is desired.
 */
static inline signed int scale(mad_fixed_t sample)
{
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));

    /* clip */
    if (sample >= MAD_F_ONE) sample = MAD_F_ONE - 1;
    else if (sample < -MAD_F_ONE) sample = -MAD_F_ONE;

    /* quantize */
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */
static enum mad_flow output(void *data, struct mad_header const *header, struct mad_pcm *pcm)
{
    CONTEXT *context = data;
    unsigned int nchannels, nsamples;
    mad_fixed_t const *left_ch, *right_ch;
    MI_AUDIO_Frame_t audio = {};
	int32_t s32Ret;
    int16_t *buffer;

    /* pcm->samplerate contains the sampling frequency */
    nchannels = pcm->channels;
    nsamples  = pcm->length;
    left_ch   = pcm->samples[0];
    right_ch  = pcm->samples[1];

    if (context->pcm_buflen < nsamples * sizeof(int16_t)) {
        if (context->pcm_buffer) free(context->pcm_buffer);
        context->pcm_buffer = malloc(nsamples * sizeof(int16_t));
        if (context->pcm_buffer) context->pcm_buflen = nsamples * sizeof(int16_t);
    }
    if (!context->pcm_buffer) return MAD_FLOW_CONTINUE;

    buffer = context->pcm_buffer;
    while (nsamples--) {
        int32_t sample = scale(*left_ch++);
        if (nchannels == 2) {
            sample += scale(*right_ch++);
            sample /= 2;
        }
        *buffer++ = (int16_t)sample;
    }
    audio.apVirAddr[0] = (uint8_t*)context->pcm_buffer;
	audio.apVirAddr[1] = NULL;
    audio.u32Len  = pcm->length * sizeof(int16_t);
	do{
        s32Ret = MI_AO_SendFrame(AO_DEV_ID, AO_CHN_ID, &audio, 1);
    }while(s32Ret == MI_AO_ERR_NOBUF);
    return MAD_FLOW_CONTINUE;
}

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
 * header file.
 */
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame)
{
    CONTEXT *context = data;

    fprintf(stderr, "decoding error 0x%04x (%s) at byte offset %u\n",
            stream->error, mad_stream_errorstr(stream),
            stream->this_frame - context->mp3_buffer);

    /* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */
    return MAD_FLOW_CONTINUE;
}

static int decode(CONTEXT *context)
{
    int result;

    /* configure input, output, and error functions */
    mad_decoder_init(&context->decoder, context,
            input, 0 /* header */, 0 /* filter */, output,
            error, 0 /* message */);

    /* start decoding */
    result = mad_decoder_run(&context->decoder, MAD_DECODER_MODE_SYNC);

    /* release the decoder */
    mad_decoder_finish(&context->decoder);
    return result;
}

void mp3dec_play_file(void *ctxt, char *file)
{
    CONTEXT    *context = ctxt;
    void       *fdm     = NULL;
    int         fd;
    struct stat stat;

    if (!context) return;
    fd = open(file, O_RDONLY);
    if (fd < 0  ) return;

    if (fstat(fd, &stat) == -1 || stat.st_size == 0) {
        goto done;
    }
    fdm = mmap(0, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (fdm == MAP_FAILED) {
        goto done;
    }

    context->mp3_buffer = fdm;
    context->mp3_buflen = stat.st_size;
	printf("mp3_buflen = %d\n",stat.st_size);
    decode(context);
	printf("decode finish!\n");

done:
    if (fdm) munmap(fdm, stat.st_size);
    close(fd);
}


