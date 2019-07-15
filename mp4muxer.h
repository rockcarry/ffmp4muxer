#ifndef __MP4MUXER_H__
#define __MP4MUXER_H__

void* mp4muxer_init(char *file, int w, int h, int frate, int gop, int duration, char *sps_data, int sps_len, char *pps_data, int pps_len);
void  mp4muxer_exit(void *ctx);

#endif



