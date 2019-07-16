#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include "mp4muxer.h"

#ifndef offsetof
#define offsetof(type, member) ((size_t)&((type*)0)->member)
#endif
#define MP4_FOURCC(a, b, c, d)  (((a) << 0) | ((b) << 8) | ((c) << 16) | ((d) << 24))

#pragma pack(1)
typedef struct {
    uint32_t  ftyp_size;
    uint32_t  ftyp_type;
    uint32_t  ftyp_brand;
    uint32_t  ftyp_version;
    uint32_t  ftyp_compat1;
    uint32_t  ftyp_compat2;
    uint32_t  ftyp_compat3;

    uint32_t  moov_size;
    uint32_t  moov_type;

    uint32_t  mvhd_size;
    uint32_t  mvhd_type;
    uint8_t   mvhd_version;
    uint8_t   mvhd_flags[3];
    uint32_t  mvhd_create_time;
    uint32_t  mvhd_modify_time;
    uint32_t  mvhd_timescale;
    uint32_t  mvhd_duration;
    uint32_t  mvhd_playrate;
    uint16_t  mvhd_volume;
    uint8_t   mvhd_reserved[10];
    uint32_t  mvhd_matrix[9];
    uint8_t   mvhd_predefined[24];
    uint32_t  mvhd_next_trackid;

    uint32_t  trakv_size;
    uint32_t  trakv_type;

    uint32_t  tkhdv_size;
    uint32_t  tkhdv_type;
    uint8_t   tkhdv_version;
    uint8_t   tkhdv_flags[3];
    uint32_t  tkhdv_create_time;
    uint32_t  tkhdV_modify_time;
    uint32_t  tkhdv_trackid;
    uint32_t  tkhdv_reserved1;
    uint32_t  tkhdv_duration;
    uint32_t  tkhdv_reserved2;
    uint32_t  tkhdv_reserved3;
    uint16_t  tkhdv_layer;
    uint16_t  tkhdv_alternate_group;
    uint16_t  tkhdv_volume;
    uint16_t  tkhdv_reserved4;
    uint32_t  tkhdv_matrix[9];
    uint32_t  tkhdv_width;
    uint32_t  tkhdv_height;

    uint32_t  mdiav_size;
    uint32_t  mdiav_type;

    uint32_t  mdhdv_size;
    uint32_t  mdhdv_type;
    uint8_t   mdhdv_version;
    uint8_t   mdhdv_flags[3];
    uint32_t  mdhdv_create_time;
    uint32_t  mdhdv_modify_time;
    uint32_t  mdhdv_timescale;
    uint32_t  mdhdv_duration;
    uint16_t  mdhdv_language;
    uint16_t  mdhdv_predefined;

    uint32_t  hdlrv_size;
    uint32_t  hdlrv_type;
    uint8_t   hdlrv_version;
    uint8_t   hdlrv_flags[3];
    uint8_t   hdlrv_predefined[4];
    uint32_t  hdlrv_handler_type;
    uint8_t   hdlrv_reserved[12];
    uint8_t   hdlrv_name[16];

    uint32_t  minfv_size;
    uint32_t  minfv_type;

    uint32_t  vmhdv_size;
    uint32_t  vmhdv_type;
    uint8_t   vmhdv_version;
    uint8_t   vmhdv_flags[3];
    uint32_t  vmhdv_graph_mode;
    uint32_t  vmhdv_opcolor;

    uint32_t  dinfv_size;
    uint32_t  dinfv_type;

    uint32_t  drefv_size;
    uint32_t  drefv_type;
    uint8_t   drefv_version;
    uint8_t   drefv_flags[3];
    uint32_t  drefv_entry_count;

    uint32_t  urlv_size;
    uint32_t  urlv_type;
    uint8_t   urlv_version;
    uint8_t   urlv_flags[3];

    uint32_t  stblv_size;
    uint32_t  stblv_type;

    uint32_t  stsdv_size;
    uint32_t  stsdv_type;
    uint8_t   stsdv_version;
    uint8_t   stsdv_flags[3];
    uint32_t  stsdv_entry_count;

    uint32_t  avc1_size;
    uint32_t  avc1_type;
    uint8_t   avc1_reserved1[6];
    uint16_t  avc1_data_refidx;
    uint16_t  avc1_predefined1;
    uint16_t  avc1_reserved2;
    uint32_t  avc1_predefined2[3];
    uint16_t  avc1_width;
    uint16_t  avc1_height;
    uint32_t  avc1_horiz_res;
    uint32_t  avc1_vert_res;
    uint32_t  avc1_reserved3;
    uint16_t  avc1_frame_count;
    uint8_t   avc1_compressor[32];
    uint16_t  avc1_depth;
    uint16_t  avc1_predefined;

    uint32_t  avcc_size;
    uint32_t  avcc_type;
    uint8_t   avcc_config_ver;
    uint8_t   avcc_avc_profile;
    uint8_t   avcc_profile_compat;
    uint8_t   avcc_avc_level;
    uint8_t   avcc_nalulen;
    uint8_t   avcc_sps_num;
    uint16_t  avcc_sps_len;
    uint8_t   avcc_sps_data[256];
    uint8_t   avcc_pps_num;
    uint16_t  avcc_pps_len;
    uint8_t   avcc_pps_data[256];

    uint32_t  sttsv_size;
    uint32_t  sttsv_type;
    uint8_t   sttsv_version;
    uint8_t   sttsv_flags[3];
    uint32_t  sttsv_count;

    uint32_t  stssv_size;
    uint32_t  stssv_type;
    uint8_t   stssv_version;
    uint8_t   stssv_flags[3];
    uint32_t  stssv_count;

    uint32_t  stscv_size;
    uint32_t  stscv_type;
    uint8_t   stscv_version;
    uint8_t   stscv_flags[3];
    uint32_t  stscv_count;

    uint32_t  stszv_size;
    uint32_t  stszv_type;
    uint8_t   stszv_version;
    uint8_t   stszv_flags[3];
    uint32_t  stszv_sample_size;
    uint32_t  stszv_count;

    uint32_t  stcov_size;
    uint32_t  stcov_type;
    uint8_t   stcov_version;
    uint8_t   stcov_flags[3];
    uint32_t  stcov_count;

    // mdat box
    uint32_t  mdat_size;
    uint32_t  mdat_type;

    uint8_t   reserved[3];
    FILE     *fp;
    int       frate;
    int       gop;
} MP4FILE;
#pragma pack()

void* mp4muxer_init(char *file, int w, int h, int frate, int gop, int duration, char *sps_data, int sps_len, char *pps_data, int pps_len)
{
    MP4FILE *mp4 = calloc(1, sizeof(MP4FILE));
    if (!mp4) return NULL;
    mp4->fp    = fopen(file, "wb");
    mp4->frate = frate;
    mp4->gop   = gop;
    if (!mp4->fp) {
        free(mp4);
        return NULL;
    }

    mp4->ftyp_size           = htonl(offsetof(MP4FILE, moov_size ) - offsetof(MP4FILE, ftyp_size));
    mp4->ftyp_type           = MP4_FOURCC('f', 't', 'y', 'p');
    mp4->ftyp_brand          = MP4_FOURCC('i', 's', 'o', 'm');
    mp4->ftyp_version        = htonl(256);
    mp4->ftyp_compat1        = MP4_FOURCC('i', 's', 'o', 'm');
    mp4->ftyp_compat2        = MP4_FOURCC('i', 's', 'o', '2');
    mp4->ftyp_compat3        = MP4_FOURCC('m', 'p', '4', '1');

    mp4->moov_size           = offsetof(MP4FILE, trakv_size) - offsetof(MP4FILE, moov_size);
    mp4->moov_type           = MP4_FOURCC('m', 'o', 'o', 'v');
    mp4->mvhd_size           = htonl(offsetof(MP4FILE, trakv_size) - offsetof(MP4FILE, mvhd_size));
    mp4->mvhd_type           = MP4_FOURCC('m', 'v', 'h', 'd');
    mp4->mvhd_timescale      = htonl(1000      );
    mp4->mvhd_duration       = htonl(duration  );
    mp4->mvhd_playrate       = htonl(0x00010000);
    mp4->mvhd_volume         = htonl(0x0100    ) >> 16;
    mp4->mvhd_matrix[0]      = htonl(0x00010000);
    mp4->mvhd_matrix[4]      = htonl(0x00010000);
    mp4->mvhd_matrix[8]      = htonl(0x40000000);
    mp4->mvhd_next_trackid   = htonl(3         );

    mp4->trakv_size          = offsetof(MP4FILE, mdiav_size) - offsetof(MP4FILE, trakv_size);
    mp4->trakv_type          = MP4_FOURCC('t', 'r', 'a', 'k');
    mp4->tkhdv_size          = htonl(offsetof(MP4FILE, mdiav_size) - offsetof(MP4FILE, tkhdv_size));
    mp4->tkhdv_type          = MP4_FOURCC('t', 'k', 'h', 'd');
    mp4->tkhdv_flags[2]      = 0x7;
    mp4->tkhdv_trackid       = htonl(1         );
    mp4->tkhdv_duration      = htonl(duration  );
    mp4->tkhdv_matrix[0]     = htonl(0x00010000);
    mp4->tkhdv_matrix[4]     = htonl(0x00010000);
    mp4->tkhdv_matrix[8]     = htonl(0x40000000);
    mp4->tkhdv_width         = htonl(w << 16   );
    mp4->tkhdv_height        = htonl(h << 16   );

    mp4->mdiav_size          = offsetof(MP4FILE, minfv_size) - offsetof(MP4FILE, mdiav_size);
    mp4->mdiav_type          = MP4_FOURCC('m', 'd', 'i', 'a');
    mp4->mdhdv_size          = htonl(offsetof(MP4FILE, hdlrv_size) - offsetof(MP4FILE, mdhdv_size));
    mp4->mdhdv_type          = MP4_FOURCC('m', 'd', 'h', 'd');
    mp4->mdhdv_timescale     = htonl(1000    );
    mp4->mdhdv_duration      = htonl(duration);
    mp4->hdlrv_size          = htonl(offsetof(MP4FILE, minfv_size) - offsetof(MP4FILE, hdlrv_size));
    mp4->hdlrv_type          = MP4_FOURCC('h', 'd', 'l', 'r');
    mp4->hdlrv_handler_type  = MP4_FOURCC('v', 'i', 'd', 'e');;
    strcpy((char*)mp4->hdlrv_name, "VideoHandler");

    mp4->minfv_size          = offsetof(MP4FILE, stblv_size) - offsetof(MP4FILE, minfv_size);
    mp4->minfv_type          = MP4_FOURCC('m', 'i', 'n', 'f');
    mp4->vmhdv_size          = htonl(offsetof(MP4FILE, dinfv_size) - offsetof(MP4FILE, vmhdv_size));
    mp4->vmhdv_type          = MP4_FOURCC('v', 'm', 'h', 'd');
    mp4->vmhdv_flags[2]      = 1;

    mp4->dinfv_size          = htonl(offsetof(MP4FILE, stblv_size) - offsetof(MP4FILE, dinfv_size));
    mp4->dinfv_type          = MP4_FOURCC('d', 'i', 'n', 'f');
    mp4->drefv_size          = htonl(offsetof(MP4FILE, stblv_size) - offsetof(MP4FILE, drefv_size));
    mp4->drefv_type          = MP4_FOURCC('d', 'r', 'e', 'f');
    mp4->drefv_entry_count   = htonl(1);
    mp4->urlv_size           = htonl(offsetof(MP4FILE, stblv_size) - offsetof(MP4FILE, urlv_size ));
    mp4->urlv_type           = MP4_FOURCC('u', 'r', 'l', ' ');
    mp4->urlv_flags[2]       = 1;

    mp4->stblv_size          = offsetof(MP4FILE, stsdv_size) - offsetof(MP4FILE, stblv_size);
    mp4->stblv_type          = MP4_FOURCC('s', 't', 'b', 'l');
    mp4->stsdv_size          = offsetof(MP4FILE, avc1_size ) - offsetof(MP4FILE, stsdv_size);
    mp4->stsdv_type          = MP4_FOURCC('s', 't', 's', 'd');
    mp4->stsdv_entry_count   = htonl(1);

    mp4->avc1_size           = offsetof(MP4FILE, avcc_size) - offsetof(MP4FILE, avc1_size);
    mp4->avc1_type           = MP4_FOURCC('a', 'v', 'c', '1');
    mp4->avc1_data_refidx    = htonl(1 ) >> 16;
    mp4->avc1_width          = htonl(w ) >> 16;
    mp4->avc1_height         = htonl(h ) >> 16;
    mp4->avc1_horiz_res      = htonl(0x480000);
    mp4->avc1_vert_res       = htonl(0x480000);
    mp4->avc1_frame_count    = htonl(1 ) >> 16;
    mp4->avc1_depth          = htonl(24) >> 16;
    mp4->avc1_predefined     = 0xFFFF;

    memcpy(mp4->avcc_sps_data, sps_data, sps_len);
    memcpy(mp4->avcc_pps_data, pps_data, pps_len);
    mp4->avcc_size           = offsetof(MP4FILE, sttsv_size) - offsetof(MP4FILE, avcc_size) + sps_len + pps_len - sizeof(mp4->avcc_sps_data) - sizeof(mp4->avcc_pps_data);
    mp4->avcc_type           = MP4_FOURCC('a', 'v', 'c', 'C');
    mp4->avcc_config_ver     = 1;
    mp4->avcc_avc_profile    = mp4->avcc_sps_data[1];
    mp4->avcc_profile_compat = mp4->avcc_sps_data[2];
    mp4->avcc_avc_level      = mp4->avcc_sps_data[3];
    mp4->avcc_nalulen        = 0xFF;
    mp4->avcc_sps_num        = (0x7 << 5) | (1 << 0);
    mp4->avcc_sps_len        = htonl(sps_len) >> 16;
    mp4->avcc_pps_num        = 1;
    mp4->avcc_pps_len        = htonl(pps_len) >> 16;

    mp4->sttsv_count         = duration * frate / 1000 + frate / 2;
    mp4->sttsv_count = 0;
    mp4->sttsv_size          = 16 + mp4->sttsv_count * 8;
    mp4->sttsv_type          = MP4_FOURCC('s', 't', 't', 's');

    mp4->stssv_count         = mp4->sttsv_count / mp4->gop;
    mp4->stssv_size          = 16 + mp4->stssv_count * 4;
    mp4->stssv_type          = MP4_FOURCC('s', 't', 's', 's');

    mp4->stscv_count         = (mp4->sttsv_count + 4) / 5;
    mp4->stscv_size          = 16 + mp4->stscv_count * 12;
    mp4->stscv_type          = MP4_FOURCC('s', 't', 's', 'c');

    mp4->stszv_sample_size   = 0;
    mp4->stszv_count         = mp4->sttsv_count;
    mp4->stszv_size          = 20 + mp4->stscv_count * 4;
    mp4->stszv_type          = MP4_FOURCC('s', 't', 's', 'z');

    mp4->stcov_count         = mp4->stscv_count;
    mp4->stcov_size          = 16 + mp4->stscv_count * 4;
    mp4->stcov_type          = MP4_FOURCC('s', 't', 'c', 'o');

    mp4->avc1_size          += mp4->avcc_size;
    mp4->stsdv_size         += mp4->avc1_size;
    mp4->stblv_size         += mp4->stsdv_size + mp4->sttsv_size + mp4->stssv_size + mp4->stscv_size + mp4->stszv_size + mp4->stcov_size;
    mp4->minfv_size         += mp4->stblv_size;
    mp4->mdiav_size         += mp4->minfv_size;
    mp4->trakv_size         += mp4->mdiav_size;
    mp4->moov_size          += mp4->trakv_size;

    mp4->sttsv_size          = htonl(mp4->sttsv_size);
    mp4->stssv_size          = htonl(mp4->stssv_size);
    mp4->stscv_size          = htonl(mp4->stscv_size);
    mp4->stszv_size          = htonl(mp4->stszv_size);
    mp4->stcov_size          = htonl(mp4->stcov_size);

    mp4->avcc_size           = htonl(mp4->avcc_size );
    mp4->avc1_size           = htonl(mp4->avc1_size );
    mp4->stsdv_size          = htonl(mp4->stsdv_size);
    mp4->stblv_size          = htonl(mp4->stblv_size);
    mp4->minfv_size          = htonl(mp4->minfv_size);
    mp4->mdiav_size          = htonl(mp4->mdiav_size);
    mp4->trakv_size          = htonl(mp4->trakv_size);
    mp4->moov_size           = htonl(mp4->moov_size );

    mp4->mdat_size           = htonl(8);
    mp4->mdat_type           = MP4_FOURCC('m', 'd', 'a', 't');

    fwrite(mp4, 1, offsetof(MP4FILE, avcc_sps_data), mp4->fp);
    fwrite(mp4->avcc_sps_data, 1, ntohl(mp4->avcc_sps_len << 16), mp4->fp);
    fwrite(&(mp4->avcc_pps_num), 1, 3 , mp4->fp);
    fwrite(mp4->avcc_pps_data, 1, ntohl(mp4->avcc_pps_len << 16), mp4->fp);
    fwrite(&(mp4->sttsv_size  ), 1, 16, mp4->fp); fseek(mp4->fp, ntohl(mp4->sttsv_size) - 16, SEEK_CUR);
    fwrite(&(mp4->stssv_size  ), 1, 16, mp4->fp); fseek(mp4->fp, ntohl(mp4->stssv_size) - 16, SEEK_CUR);
    fwrite(&(mp4->stscv_size  ), 1, 16, mp4->fp); fseek(mp4->fp, ntohl(mp4->stscv_size) - 16, SEEK_CUR);
    fwrite(&(mp4->stszv_size  ), 1, 20, mp4->fp); fseek(mp4->fp, ntohl(mp4->stszv_size) - 20, SEEK_CUR);
    fwrite(&(mp4->stcov_size  ), 1, 16, mp4->fp); fseek(mp4->fp, ntohl(mp4->stcov_size) - 16, SEEK_CUR);
    fwrite(&(mp4->mdat_size   ), 1, ntohl(mp4->mdat_size), mp4->fp);
    return mp4;
}

void mp4muxer_exit(void *ctx)
{
    MP4FILE *mp4 = (MP4FILE*)ctx;
    if (mp4) {
        if (mp4->fp) fclose(mp4->fp);
        free(mp4);
    }
}

#ifdef _TEST_
int main(void)
{
    uint8_t sps[] = { 103,77,0,40,229,64,60,1,19,242,160 };
    uint8_t pps[] = { 104,238,49,18 };
    void *mp4 = mp4muxer_init("test.mp4", 1920, 1080, 25, 50, 60000, sps, sizeof(sps), pps, sizeof(pps));
    mp4muxer_exit(mp4);
    return 0;
}
#endif
