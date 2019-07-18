#ifndef __MP4MUXER_H__
#define __MP4MUXER_H__

void* mp4muxer_init  (char *file, int w, int h, int frate, int gop, int duration);
void  mp4muxer_exit  (void *ctx);
void  mp4muxer_video (void *ctx, unsigned char *buf, int len);
void  mp4muxer_audio (void *ctx, unsigned char *buf, int len);
void  mp4muxer_spspps(void *ctx, uint8_t *spsbuf, int spslen, uint8_t *ppsbuf, int ppslen);

#endif



