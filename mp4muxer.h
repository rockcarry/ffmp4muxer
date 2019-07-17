#ifndef __MP4MUXER_H__
#define __MP4MUXER_H__

void* mp4muxer_init (char *file, int w, int h, int frate, int gop, int duration, unsigned char *sps_data, int sps_len, unsigned char *pps_data, int pps_len);
void  mp4muxer_exit (void *ctx);
void  mp4muxer_video(void *ctx, unsigned char *buf, int len, unsigned pts);
void  mp4muxer_audio(void *ctx, unsigned char *buf, int len, unsigned pts);

#endif



