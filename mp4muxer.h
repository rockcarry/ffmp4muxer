#ifndef __MP4MUXER_H__
#define __MP4MUXER_H__

void* mp4muxer_init (char *file, int duration, int rotation, int w, int h, int frate, int gop, int h265, int chnum, int samprate, int sampbits, int sampnum, unsigned char *aacspecinfo);
void  mp4muxer_exit (void *ctx);
void  mp4muxer_video(void *ctx, unsigned char *buf, int len, int key, unsigned pts);
void  mp4muxer_audio(void *ctx, unsigned char *buf, int len, int key, unsigned pts);

#endif



