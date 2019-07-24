#ifndef __MP4MUXER_H__
#define __MP4MUXER_H__

void* mp4muxer_init  (char *file, int duration, int w, int h, int frate, int gop, int chnum, int samprate, int sampbits, int sampnum, unsigned char *aacspecinfo);
void  mp4muxer_exit  (void *ctx);
void  mp4muxer_video (void *ctx, unsigned char *buf, int len, unsigned pts);
void  mp4muxer_audio (void *ctx, unsigned char *buf, int len, unsigned pts);
void  mp4muxer_spspps(void *ctx, uint8_t *spsbuf, int spslen, uint8_t *ppsbuf, int ppslen);
void  mp4muxer_aacdecspecinfo(void *ctx, unsigned char *specinfo);

#endif



