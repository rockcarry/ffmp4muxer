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

static int h264_parse_nalu_header(uint8_t *data, int len, int *hdrsize)
{
    int  i;
    for (i=0; i<4 && i<len && !data[i]; i++);
    if (i < 2 || i == 4 || data[i] != 0x01 || ++i >= len) {
        printf("failed to find h264 frame data[%d] = %02x !\n", i, data[i]);
        return -1;
    }
    *hdrsize = i;
    return data[i] & 0x1f;
}

int h264_parse_key_sps_pps(uint8_t *data, int len, int *key, uint8_t **sps_buf, int *sps_len, uint8_t **pps_buf, int *pps_len)
{
    int i, last, type, hdrsize;
    uint8_t *sbuf, *pbuf;
    int      slen,  plen;

#if 0
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[0 ], data[1 ], data[2 ], data[3 ], data[4 ], data[5 ], data[6 ], data[7 ]);
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[8 ], data[9 ], data[10], data[11], data[12], data[13], data[14], data[15]);
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[16], data[17], data[18], data[19], data[20], data[21], data[22], data[23]);
    printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", data[24], data[25], data[26], data[27], data[28], data[29], data[30], data[31]);
#endif

    if (key    ) *key     = 0;
    if (sps_buf) *sps_buf = NULL;
    if (sps_len) *sps_len = 0;
    if (pps_buf) *pps_buf = NULL;
    if (pps_len) *pps_len = 0;
    type = h264_parse_nalu_header(data, len, &hdrsize);
    if (type == -1) return 0;
    data += hdrsize;
    len  -= hdrsize;

    if (type == 7) { // get sps
        for (i=0,last=1; i<len && (last || data[i]); last=data[i],i++);
        sbuf  = data;
        slen  = i-1;
        data += i-1;
        len  -= i-1;

        type = h264_parse_nalu_header(data, len, &hdrsize);
        if (type == -1) return 0;
        data += hdrsize;
        len  -= hdrsize;

        if (type == 8) { // get pps
            for (i=0,last=1; i<len && (last || data[i]); last=data[i],i++);
            pbuf  = data;
            plen  = i-1;
            data += i-1;
            len  -= i-1;
        } else {
            printf("not pps data !\n");
            return 0;
        }

        if (sps_buf) *sps_buf = sbuf;
        if (sps_len) *sps_len = slen;
        if (pps_buf) *pps_buf = pbuf;
        if (pps_len) *pps_len = plen;

        type = h264_parse_nalu_header(data, len, &hdrsize);
        if (type == -1) return 0;
        data += hdrsize;
        len  -= hdrsize;
        if (key) *key = 1;
    }
    return len;
}

#ifndef offsetof
#define offsetof(type, member) ((size_t)&((type*)0)->member)
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#define MP4_FOURCC(a, b, c, d)  (((a) << 0) | ((b) << 8) | ((c) << 16) | ((d) << 24))

#define VIDEO_TIMESCALE_BY_FRAME_RATE   1
#define AUDIO_TIMESCALE_BY_SAMPLE_RATE  1

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

    // video track
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
    uint8_t   avcc_sps_data[16];
    uint8_t   avcc_pps_num;
    uint16_t  avcc_pps_len;
    uint8_t   avcc_pps_data[15];

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
    uint32_t  stscv_first_chunk;
    uint32_t  stscv_samp_per_chunk;
    uint32_t  stscv_samp_desc_id;

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

    // audio track
    uint32_t  traka_size;
    uint32_t  traka_type;

    uint32_t  tkhda_size;
    uint32_t  tkhda_type;
    uint8_t   tkhda_version;
    uint8_t   tkhda_flags[3];
    uint32_t  tkhda_create_time;
    uint32_t  tkhda_modify_time;
    uint32_t  tkhda_trackid;
    uint32_t  tkhda_reserved1;
    uint32_t  tkhda_duration;
    uint32_t  tkhda_reserved2;
    uint32_t  tkhda_reserved3;
    uint16_t  tkhda_layer;
    uint16_t  tkhda_alternate_group;
    uint16_t  tkhda_volume;
    uint16_t  tkhda_reserved4;
    uint32_t  tkhda_matrix[9];
    uint32_t  tkhda_width;
    uint32_t  tkhda_height;

    uint32_t  mdiaa_size;
    uint32_t  mdiaa_type;

    uint32_t  mdhda_size;
    uint32_t  mdhda_type;
    uint8_t   mdhda_version;
    uint8_t   mdhda_flags[3];
    uint32_t  mdhda_create_time;
    uint32_t  mdhda_modify_time;
    uint32_t  mdhda_timescale;
    uint32_t  mdhda_duration;
    uint16_t  mdhda_language;
    uint16_t  mdhda_predefined;

    uint32_t  hdlra_size;
    uint32_t  hdlra_type;
    uint8_t   hdlra_version;
    uint8_t   hdlra_flags[3];
    uint8_t   hdlra_predefined[4];
    uint32_t  hdlra_handler_type;
    uint8_t   hdlra_reserved[12];
    uint8_t   hdlra_name[16];

    uint32_t  minfa_size;
    uint32_t  minfa_type;

    uint32_t  smhda_size;
    uint32_t  smhda_type;
    uint8_t   smhda_version;
    uint8_t   smhda_flags[3];
    uint16_t  smhda_balance;
    uint16_t  smhda_reserved;

    uint32_t  dinfa_size;
    uint32_t  dinfa_type;

    uint32_t  drefa_size;
    uint32_t  drefa_type;
    uint8_t   drefa_version;
    uint8_t   drefa_flags[3];
    uint32_t  drefa_entry_count;

    uint32_t  urla_size;
    uint32_t  urla_type;
    uint8_t   urla_version;
    uint8_t   urla_flags[3];

    uint32_t  stbla_size;
    uint32_t  stbla_type;

    uint32_t  stsda_size;
    uint32_t  stsda_type;
    uint8_t   stsda_version;
    uint8_t   stsda_flags[3];
    uint32_t  stsda_entry_count;

    uint32_t  mp4a_size;
    uint32_t  mp4a_type;
    uint8_t   mp4a_reserved1[6];
    uint16_t  mp4a_data_refidx;
    uint32_t  mp4a_reserved2[2];
    uint16_t  mp4a_channel_num;
    uint16_t  mp4a_sample_size;
    uint16_t  mp4a_predefined1;
    uint16_t  mp4a_reserved3;
    uint32_t  mp4a_sample_rate;

    uint32_t  esds_size;
    uint32_t  esds_type;
    uint8_t   esds_version;
    uint8_t   esds_flags[3];
    uint8_t   esds_esdesc_tag;    // 0x03
    uint16_t  esds_esdesc_len;    // ((25 << 8) | 0x80)
    uint16_t  esds_esdesc_id;     // 0x0200
    uint8_t   esds_esdesc_flags;  // 0x00
    uint8_t   esds_deccfg_tag;    // 0x04
    uint8_t   esds_deccfg_len;    // 17
    uint8_t   esds_deccfg_object; // 0x40 - aac, 0x6B - mp3
    uint8_t   esds_deccfg_stream; // 0x15
    uint8_t   esds_deccfg_buffer_size[3];
    uint32_t  esds_deccfg_max_bitrate;
    uint32_t  esds_deccfg_avg_bitrate;
    //++ if esds_deccfg_object == aac
    uint8_t   esds_decspec_tag;   // 0x05
    uint8_t   esds_decspec_len;   // 2
    uint16_t  esds_decspec_info;
    //-- if esds_deccfg_object == aac
    uint8_t   esds_slcfg_tag;     // 0x06
    uint8_t   esds_slcfg_len;     // 1
    uint8_t   esds_slcfg_reserved;// 0x02

    uint32_t  sttsa_size;
    uint32_t  sttsa_type;
    uint8_t   sttsa_version;
    uint8_t   sttsa_flags[3];
    uint32_t  sttsa_count;

    uint32_t  stsca_size;
    uint32_t  stsca_type;
    uint8_t   stsca_version;
    uint8_t   stsca_flags[3];
    uint32_t  stsca_count;
    uint32_t  stsca_first_chunk;
    uint32_t  stsca_samp_per_chunk;
    uint32_t  stsca_samp_desc_id;

    uint32_t  stsza_size;
    uint32_t  stsza_type;
    uint8_t   stsza_version;
    uint8_t   stsza_flags[3];
    uint32_t  stsza_sample_size;
    uint32_t  stsza_count;

    uint32_t  stcoa_size;
    uint32_t  stcoa_type;
    uint8_t   stcoa_version;
    uint8_t   stcoa_flags[3];
    uint32_t  stcoa_count;

    // mdat box
    uint32_t  mdat_size;
    uint32_t  mdat_type;

    uint8_t   reserved[3];
    FILE     *fp;
    int       frate;
    int       gop;
    int       samprate;
    int       sampnum;

    int       sttsv_off;
    int       stssv_off;
    int       stszv_off;
    int       stcov_off;
    int       sttsv_cur;
    int       stssv_cur;
    int       stszv_cur;
    int       stcov_cur;

    int       sttsa_off;
    int       stsza_off;
    int       stcoa_off;
    int       sttsa_cur;
    int       stsza_cur;
    int       stcoa_cur;

    int       chunk_off;
    uint32_t  vpts_last;
    uint32_t  apts_last;
    int       aframemax;
    int       vframemax;
    int       syncf_max;
    uint32_t *sttsv_buf;
    uint32_t *stssv_buf;
    uint32_t *stszv_buf;
    uint32_t *stcov_buf;
    uint32_t *sttsa_buf;
    uint32_t *stsza_buf;
    uint32_t *stcoa_buf;
} MP4FILE;
#pragma pack()

static void write_fixed_trackv_data(MP4FILE *mp4)
{
    if (mp4->sttsv_buf && mp4->sttsv_cur < ntohl(mp4->sttsv_count)) {
        fseek(mp4->fp, mp4->sttsv_off + 12, SEEK_SET);
        fwrite(&mp4->sttsv_count, 1, sizeof(uint32_t), mp4->fp);
        fseek(mp4->fp, mp4->sttsv_cur * sizeof(uint32_t) * 2, SEEK_CUR);
        fwrite(&mp4->sttsv_buf[mp4->sttsv_cur], 1, (ntohl(mp4->sttsv_count) - mp4->sttsv_cur) * sizeof(uint32_t) * 2, mp4->fp);
        mp4->sttsv_cur = ntohl(mp4->sttsv_count);
    }
    if (mp4->stssv_buf && mp4->stssv_cur < ntohl(mp4->stssv_count)) {
        fseek(mp4->fp, mp4->stssv_off + 12, SEEK_SET);
        fwrite(&mp4->stssv_count, 1, sizeof(uint32_t), mp4->fp);
        fseek(mp4->fp, mp4->stssv_cur * sizeof(uint32_t), SEEK_CUR);
        fwrite(&mp4->stssv_buf[mp4->stssv_cur], 1, (ntohl(mp4->stssv_count) - mp4->stssv_cur) * sizeof(uint32_t), mp4->fp);
        mp4->stssv_cur = ntohl(mp4->stssv_count);
    }
    if (mp4->stszv_buf && mp4->stszv_cur < ntohl(mp4->stszv_count)) {
        fseek(mp4->fp, mp4->stszv_off + 16, SEEK_SET);
        fwrite(&mp4->stszv_count, 1, sizeof(uint32_t), mp4->fp);
        fseek(mp4->fp, mp4->stszv_cur * sizeof(uint32_t), SEEK_CUR);
        fwrite(&mp4->stszv_buf[mp4->stszv_cur], 1, (ntohl(mp4->stszv_count) - mp4->stszv_cur) * sizeof(uint32_t), mp4->fp);
        mp4->stszv_cur = ntohl(mp4->stszv_count);
    }
    if (mp4->stcov_buf && mp4->stcov_cur < ntohl(mp4->stcov_count)) {
        fseek(mp4->fp, mp4->stcov_off + 12, SEEK_SET);
        fwrite(&mp4->stcov_count, 1, sizeof(uint32_t), mp4->fp);
        fseek(mp4->fp, mp4->stcov_cur * sizeof(uint32_t), SEEK_CUR);
        fwrite(&mp4->stcov_buf[mp4->stcov_cur], 1, (ntohl(mp4->stcov_count) - mp4->stcov_cur) * sizeof(uint32_t), mp4->fp);
        mp4->stcov_cur = ntohl(mp4->stcov_count);
    }
    fseek(mp4->fp, ntohl(mp4->ftyp_size) + ntohl(mp4->moov_size), SEEK_SET);
    fwrite(&mp4->mdat_size, 1, sizeof(uint32_t), mp4->fp);
    fseek(mp4->fp, 0, SEEK_END);
}

static void write_fixed_tracka_data(MP4FILE *mp4)
{
    if (mp4->sttsa_buf && mp4->sttsa_cur < ntohl(mp4->sttsa_count)) {
        fseek(mp4->fp, mp4->sttsa_off + 12, SEEK_SET);
        fwrite(&mp4->sttsa_count, 1, sizeof(uint32_t), mp4->fp);
        fseek(mp4->fp, mp4->sttsa_cur * sizeof(uint32_t) * 2, SEEK_CUR);
        fwrite(&mp4->sttsa_buf[mp4->sttsa_cur], 1, (ntohl(mp4->sttsa_count) - mp4->sttsa_cur) * sizeof(uint32_t) * 2, mp4->fp);
        mp4->sttsa_cur = ntohl(mp4->sttsa_count);
    }
    if (mp4->stsza_buf && mp4->stsza_cur < ntohl(mp4->stsza_count)) {
        fseek(mp4->fp, mp4->stsza_off + 16, SEEK_SET);
        fwrite(&mp4->stsza_count, 1, sizeof(uint32_t), mp4->fp);
        fseek(mp4->fp, mp4->stsza_cur * sizeof(uint32_t), SEEK_CUR);
        fwrite(&mp4->stsza_buf[mp4->stsza_cur], 1, (ntohl(mp4->stsza_count) - mp4->stsza_cur) * sizeof(uint32_t), mp4->fp);
        mp4->stsza_cur = ntohl(mp4->stsza_count);
    }
    if (mp4->stcoa_buf && mp4->stcoa_cur < ntohl(mp4->stcoa_count)) {
        fseek(mp4->fp, mp4->stcoa_off + 12, SEEK_SET);
        fwrite(&mp4->stcoa_count, 1, sizeof(uint32_t), mp4->fp);
        fseek(mp4->fp, mp4->stcoa_cur * sizeof(uint32_t), SEEK_CUR);
        fwrite(&mp4->stcoa_buf[mp4->stcoa_cur], 1, (ntohl(mp4->stcoa_count) - mp4->stcoa_cur) * sizeof(uint32_t), mp4->fp);
        mp4->stcoa_cur = ntohl(mp4->stcoa_count);
    }
    fseek(mp4->fp, ntohl(mp4->ftyp_size) + ntohl(mp4->moov_size), SEEK_SET);
    fwrite(&mp4->mdat_size, 1, sizeof(uint32_t), mp4->fp);
    fseek(mp4->fp, 0, SEEK_END);
}

void* mp4muxer_init(char *file, int duration, int w, int h, int frate, int gop, int chnum, int samprate, int sampbits, int sampnum, unsigned char *aacspecinfo)
{
    MP4FILE *mp4 = calloc(1, sizeof(MP4FILE));
    if (!mp4) return NULL;
    mp4->fp      = fopen(file, "wb");
    mp4->frate   = frate;
    mp4->gop     = gop;
    mp4->samprate= samprate;
    mp4->sampnum = sampnum;
    if (!mp4->fp) {
        free(mp4);
        return NULL;
    }

    mp4->ftyp_size           = htonl(offsetof(MP4FILE, moov_size ) - offsetof(MP4FILE, ftyp_size));
    mp4->ftyp_type           = MP4_FOURCC('f', 't', 'y', 'p');
    mp4->ftyp_brand          = MP4_FOURCC('i', 's', 'o', 'm');
    mp4->ftyp_version        = htonl(512);
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

    // video track
    mp4->trakv_size          = offsetof(MP4FILE, mdiav_size) - offsetof(MP4FILE, trakv_size);
    mp4->trakv_type          = MP4_FOURCC('t', 'r', 'a', 'k');
    mp4->tkhdv_size          = htonl(offsetof(MP4FILE, mdiav_size) - offsetof(MP4FILE, tkhdv_size));
    mp4->tkhdv_type          = MP4_FOURCC('t', 'k', 'h', 'd');
    mp4->tkhdv_flags[2]      = 0xF;
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
#if VIDEO_TIMESCALE_BY_FRAME_RATE
    mp4->mdhdv_timescale     = htonl(frate);
    mp4->mdhdv_duration      = htonl(duration * frate / 1000);
#else
    mp4->mdhdv_timescale     = htonl(1000 );
    mp4->mdhdv_duration      = htonl(duration);
#endif
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

    mp4->avcc_size           = offsetof(MP4FILE, sttsv_size) - offsetof(MP4FILE, avcc_size);
    mp4->avcc_type           = MP4_FOURCC('a', 'v', 'c', 'C');
    mp4->avcc_config_ver     = 1;
    mp4->avcc_avc_profile    = mp4->avcc_sps_data[1];
    mp4->avcc_profile_compat = mp4->avcc_sps_data[2];
    mp4->avcc_avc_level      = mp4->avcc_sps_data[3];
    mp4->avcc_nalulen        = 0xFF;
    mp4->avcc_sps_num        = (0x7 << 5) | (1 << 0);
    mp4->avcc_sps_len        = htonl(sizeof(mp4->avcc_sps_data)) >> 16;
    mp4->avcc_pps_num        = 1;
    mp4->avcc_pps_len        = htonl(sizeof(mp4->avcc_pps_data)) >> 16;

    mp4->vframemax           = duration * frate / 1000 + frate / 2;
    mp4->syncf_max           = mp4->vframemax / mp4->gop;

#if VIDEO_TIMESCALE_BY_FRAME_RATE
    mp4->sttsv_size          = 16 + 1 * sizeof(uint32_t) * 2;
#else
    mp4->sttsv_size          = 16 + mp4->vframemax * sizeof(uint32_t) * 2;
#endif
    mp4->sttsv_type          = MP4_FOURCC('s', 't', 't', 's');

    mp4->stssv_size          = 16 + mp4->syncf_max * sizeof(uint32_t) * 1;
    mp4->stssv_type          = MP4_FOURCC('s', 't', 's', 's');

    mp4->stscv_size          = 16 + sizeof(uint32_t) * 3;
    mp4->stscv_type          = MP4_FOURCC('s', 't', 's', 'c');
    mp4->stscv_count         = htonl(1);
    mp4->stscv_first_chunk   = htonl(1);
    mp4->stscv_samp_per_chunk= htonl(1);
    mp4->stscv_samp_desc_id  = htonl(1);

    mp4->stszv_size          = 20 + mp4->vframemax * sizeof(uint32_t) * 1;
    mp4->stszv_type          = MP4_FOURCC('s', 't', 's', 'z');
    mp4->stszv_sample_size   = 0;

    mp4->stcov_size          = 16 + mp4->vframemax * sizeof(uint32_t) * 1;
    mp4->stcov_type          = MP4_FOURCC('s', 't', 'c', 'o');

    mp4->avc1_size          += mp4->avcc_size;
    mp4->stsdv_size         += mp4->avc1_size;
    mp4->stblv_size         += mp4->stsdv_size + mp4->sttsv_size + mp4->stssv_size + mp4->stscv_size + mp4->stszv_size + mp4->stcov_size;
    mp4->minfv_size         += mp4->stblv_size;
    mp4->mdiav_size         += mp4->minfv_size;
    mp4->trakv_size         += mp4->mdiav_size;
    mp4->moov_size          += mp4->trakv_size;

    mp4->sttsv_off           = offsetof(MP4FILE, sttsv_size);
    mp4->stssv_off           = mp4->sttsv_off + mp4->sttsv_size;
    mp4->stszv_off           = mp4->stssv_off + mp4->stssv_size + mp4->stscv_size;
    mp4->stcov_off           = mp4->stszv_off + mp4->stszv_size;

    mp4->sttsv_buf           = calloc(1, mp4->sttsv_size - 16);
    mp4->stssv_buf           = calloc(1, mp4->stssv_size - 16);
    mp4->stszv_buf           = calloc(1, mp4->stszv_size - 20);
    mp4->stcov_buf           = calloc(1, mp4->stcov_size - 16);

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

    // audio track
    mp4->traka_size          = offsetof(MP4FILE, mdiaa_size) - offsetof(MP4FILE, traka_size);
    mp4->traka_type          = MP4_FOURCC('t', 'r', 'a', 'k');
    mp4->tkhda_size          = htonl(offsetof(MP4FILE, mdiaa_size) - offsetof(MP4FILE, tkhda_size));
    mp4->tkhda_type          = MP4_FOURCC('t', 'k', 'h', 'd');
    mp4->tkhda_flags[2]      = 0xF;
    mp4->tkhda_trackid       = htonl(2       );
    mp4->tkhda_duration      = htonl(duration);
    mp4->tkhda_volume        = 0x0100;

    mp4->mdiaa_size          = offsetof(MP4FILE, minfa_size) - offsetof(MP4FILE, mdiaa_size);
    mp4->mdiaa_type          = MP4_FOURCC('m', 'd', 'i', 'a');
    mp4->mdhda_size          = htonl(offsetof(MP4FILE, hdlra_size) - offsetof(MP4FILE, mdhda_size));
    mp4->mdhda_type          = MP4_FOURCC('m', 'd', 'h', 'd');
#if AUDIO_TIMESCALE_BY_SAMPLE_RATE
    mp4->mdhda_timescale     = htonl(samprate);
    mp4->mdhda_duration      = htonl(duration * samprate / 1000);
#else
    mp4->mdhda_timescale     = htonl(1000    );
    mp4->mdhda_duration      = htonl(duration);
#endif
    mp4->hdlra_size          = htonl(offsetof(MP4FILE, minfa_size) - offsetof(MP4FILE, hdlra_size));
    mp4->hdlra_type          = MP4_FOURCC('h', 'd', 'l', 'r');
    mp4->hdlra_handler_type  = MP4_FOURCC('s', 'o', 'u', 'n');;
    strcpy((char*)mp4->hdlra_name, "SoundHandler");

    mp4->minfa_size          = offsetof(MP4FILE, stbla_size) - offsetof(MP4FILE, minfa_size);
    mp4->minfa_type          = MP4_FOURCC('m', 'i', 'n', 'f');
    mp4->smhda_size          = htonl(offsetof(MP4FILE, dinfa_size) - offsetof(MP4FILE, smhda_size));
    mp4->smhda_type          = MP4_FOURCC('s', 'm', 'h', 'd');

    mp4->dinfa_size          = htonl(offsetof(MP4FILE, stbla_size) - offsetof(MP4FILE, dinfa_size));
    mp4->dinfa_type          = MP4_FOURCC('d', 'i', 'n', 'f');
    mp4->drefa_size          = htonl(offsetof(MP4FILE, stbla_size) - offsetof(MP4FILE, drefa_size));
    mp4->drefa_type          = MP4_FOURCC('d', 'r', 'e', 'f');
    mp4->drefa_entry_count   = htonl(1);
    mp4->urla_size           = htonl(offsetof(MP4FILE, stbla_size) - offsetof(MP4FILE, urla_size ));
    mp4->urla_type           = MP4_FOURCC('u', 'r', 'l', ' ');
    mp4->urla_flags[2]       = 1;

    mp4->stbla_size          = offsetof(MP4FILE, stsda_size) - offsetof(MP4FILE, stbla_size);
    mp4->stbla_type          = MP4_FOURCC('s', 't', 'b', 'l');
    mp4->stsda_size          = offsetof(MP4FILE, mp4a_size ) - offsetof(MP4FILE, stsda_size);
    mp4->stsda_type          = MP4_FOURCC('s', 't', 's', 'd');
    mp4->stsda_entry_count   = htonl(1);

    mp4->mp4a_size           = offsetof(MP4FILE, esds_size) - offsetof(MP4FILE, mp4a_size);
    mp4->mp4a_type           = MP4_FOURCC('m', 'p', '4', 'a');
    mp4->mp4a_data_refidx    = htonl(1       ) >> 16;
    mp4->mp4a_channel_num    = htonl(chnum   ) >> 16;
    mp4->mp4a_sample_size    = htonl(sampbits) >> 16;
    mp4->mp4a_sample_rate    = htonl(samprate << 16);

    mp4->esds_size           = offsetof(MP4FILE, sttsa_size) - offsetof(MP4FILE, esds_size);
    mp4->esds_type           = MP4_FOURCC('e', 's', 'd', 's');

    mp4->esds_esdesc_tag     = 0x03;
    mp4->esds_esdesc_len     = ((25 << 8) | 0x80);
    mp4->esds_esdesc_id      = 0x0200;
    mp4->esds_esdesc_flags   = 0x00;
    mp4->esds_deccfg_tag     = 0x04;
    mp4->esds_deccfg_len     = 17;
    mp4->esds_deccfg_object  = 0x40;
    mp4->esds_deccfg_stream  = 0x15;
    mp4->esds_decspec_tag    = 0x05;
    mp4->esds_decspec_len    = 2;
    mp4->esds_decspec_info   = aacspecinfo ? (aacspecinfo[1] << 8) | (aacspecinfo[0] << 0) : 0;
    mp4->esds_slcfg_tag      = 0x06;
    mp4->esds_slcfg_len      = 1;
    mp4->esds_slcfg_reserved = 0x02;

    mp4->aframemax           = duration * samprate / 1000 / sampnum + samprate / sampnum / 2;
#if AUDIO_TIMESCALE_BY_SAMPLE_RATE
    mp4->sttsa_size          = 16 + 1 * sizeof(uint32_t) * 2;
#else
    mp4->sttsa_size          = 16 + mp4->aframemax * sizeof(uint32_t) * 2;
#endif
    mp4->sttsa_type          = MP4_FOURCC('s', 't', 't', 's');

    mp4->stsca_size          = 16 + sizeof(uint32_t) * 3;
    mp4->stsca_type          = MP4_FOURCC('s', 't', 's', 'c');
    mp4->stsca_count         = htonl(1);
    mp4->stsca_first_chunk   = htonl(1);
    mp4->stsca_samp_per_chunk= htonl(1);
    mp4->stsca_samp_desc_id  = htonl(1);

    mp4->stsza_size          = 20 + mp4->aframemax * sizeof(uint32_t) * 1;
    mp4->stsza_type          = MP4_FOURCC('s', 't', 's', 'z');
    mp4->stsza_sample_size   = 0;

    mp4->stcoa_size          = 16 + mp4->aframemax * sizeof(uint32_t) * 1;
    mp4->stcoa_type          = MP4_FOURCC('s', 't', 'c', 'o');

    mp4->mp4a_size          += mp4->esds_size;
    mp4->stsda_size         += mp4->mp4a_size;
    mp4->stbla_size         += mp4->stsda_size + mp4->sttsa_size + mp4->stsca_size + mp4->stsza_size + mp4->stcoa_size;
    mp4->minfa_size         += mp4->stbla_size;
    mp4->mdiaa_size         += mp4->minfa_size;
    mp4->traka_size         += mp4->mdiaa_size;
    mp4->moov_size          += mp4->traka_size;

    mp4->sttsa_off           = offsetof(MP4FILE, trakv_size) + ntohl(mp4->trakv_size) + offsetof(MP4FILE, sttsa_size) - offsetof(MP4FILE, traka_size);
    mp4->stsza_off           = mp4->sttsa_off + mp4->sttsa_size + mp4->stsca_size;
    mp4->stcoa_off           = mp4->stsza_off + mp4->stsza_size;

    mp4->sttsa_buf           = calloc(1, mp4->sttsa_size - 16);
    mp4->stsza_buf           = calloc(1, mp4->stsza_size - 20);
    mp4->stcoa_buf           = calloc(1, mp4->stcoa_size - 16);

    mp4->sttsa_size          = htonl(mp4->sttsa_size);
    mp4->stsca_size          = htonl(mp4->stsca_size);
    mp4->stsza_size          = htonl(mp4->stsza_size);
    mp4->stcoa_size          = htonl(mp4->stcoa_size);

    mp4->esds_size           = htonl(mp4->esds_size );
    mp4->mp4a_size           = htonl(mp4->mp4a_size );
    mp4->stsda_size          = htonl(mp4->stsda_size);
    mp4->stbla_size          = htonl(mp4->stbla_size);
    mp4->minfa_size          = htonl(mp4->minfa_size);
    mp4->mdiaa_size          = htonl(mp4->mdiaa_size);
    mp4->traka_size          = htonl(mp4->traka_size);

    mp4->moov_size           = htonl(mp4->moov_size );
    mp4->mdat_size           = htonl(8);
    mp4->mdat_type           = MP4_FOURCC('m', 'd', 'a', 't');

    fwrite(mp4, 1, offsetof(MP4FILE, sttsv_size), mp4->fp);
    fwrite(&mp4->sttsv_size, 1, 16, mp4->fp); fseek(mp4->fp, ntohl(mp4->sttsv_size) - 16, SEEK_CUR);
    fwrite(&mp4->stssv_size, 1, 16, mp4->fp); fseek(mp4->fp, ntohl(mp4->stssv_size) - 16, SEEK_CUR);
    fwrite(&mp4->stscv_size, 1, ntohl(mp4->stscv_size), mp4->fp);
    fwrite(&mp4->stszv_size, 1, 20, mp4->fp); fseek(mp4->fp, ntohl(mp4->stszv_size) - 20, SEEK_CUR);
    fwrite(&mp4->stcov_size, 1, 16, mp4->fp); fseek(mp4->fp, ntohl(mp4->stcov_size) - 16, SEEK_CUR);
    fwrite(&mp4->traka_size, 1, offsetof(MP4FILE, sttsa_size) - offsetof(MP4FILE, traka_size), mp4->fp);
    fwrite(&mp4->sttsa_size, 1, 16, mp4->fp); fseek(mp4->fp, ntohl(mp4->sttsa_size) - 16, SEEK_CUR);
    fwrite(&mp4->stsca_size, 1, ntohl(mp4->stsca_size), mp4->fp);
    fwrite(&mp4->stsza_size, 1, 20, mp4->fp); fseek(mp4->fp, ntohl(mp4->stsza_size) - 20, SEEK_CUR);
    fwrite(&mp4->stcoa_size, 1, 16, mp4->fp); fseek(mp4->fp, ntohl(mp4->stcoa_size) - 16, SEEK_CUR);
    fwrite(&mp4->mdat_size , 1, ntohl(mp4->mdat_size), mp4->fp);

    mp4->chunk_off    = ntohl(mp4->ftyp_size) + ntohl(mp4->moov_size) + ntohl(mp4->mdat_size);
    mp4->avcc_sps_len = 0;
    mp4->avcc_pps_len = 0;
    return mp4;
}

void mp4muxer_exit(void *ctx)
{
    MP4FILE *mp4 = (MP4FILE*)ctx;
    if (mp4) {
        write_fixed_trackv_data(mp4);
        write_fixed_tracka_data(mp4);
        fclose(mp4->fp);
        if (mp4->sttsv_buf) free(mp4->sttsv_buf);
        if (mp4->stssv_buf) free(mp4->stssv_buf);
        if (mp4->stszv_buf) free(mp4->stszv_buf);
        if (mp4->stcov_buf) free(mp4->stcov_buf);
        if (mp4->sttsa_buf) free(mp4->sttsa_buf);
        if (mp4->stsza_buf) free(mp4->stsza_buf);
        if (mp4->stcoa_buf) free(mp4->stcoa_buf);
        free(mp4);
    }
}

void mp4muxer_video(void *ctx, unsigned char *buf, int len, unsigned pts)
{
    MP4FILE *mp4 = (MP4FILE*)ctx;
    int      key = 0, fsize;
    uint8_t *spsbuf, *ppsbuf;
    int      spslen,  ppslen;
    if (!ctx) return;

    fsize = h264_parse_key_sps_pps(buf, len, &key, &spsbuf, &spslen, &ppsbuf, &ppslen);
    if (!fsize) return;
    buf +=(len - fsize);
    len  = fsize;
    fsize= htonl(fsize);

    if (!mp4->avcc_sps_len && spslen) mp4muxer_spspps(mp4, spsbuf, spslen, NULL, 0);
    if (!mp4->avcc_pps_len && ppslen) mp4muxer_spspps(mp4, NULL, 0, ppsbuf, ppslen);

    if (mp4->stszv_buf && ntohl(mp4->stszv_count) < mp4->vframemax) {
        mp4->stszv_buf[ntohl(mp4->stszv_count)] = htonl(len + sizeof(uint32_t));
        mp4->stszv_count = htonl(ntohl(mp4->stszv_count) + 1);
    }

    if (mp4->stssv_buf && ntohl(mp4->stssv_count) < mp4->syncf_max && key) {
        mp4->stssv_buf[ntohl(mp4->stssv_count)] = mp4->stszv_count;
        mp4->stssv_count = htonl(ntohl(mp4->stssv_count) + 1);
    }

#if VIDEO_TIMESCALE_BY_FRAME_RATE
    if (mp4->sttsv_buf) {
        mp4->sttsv_buf[0] = mp4->stszv_count;
        mp4->sttsv_buf[1] = htonl(1);
        mp4->sttsv_count  = htonl(1);
    }
#else
    if (mp4->sttsv_buf && ntohl(mp4->sttsv_count) < mp4->vframemax) {
        mp4->sttsv_buf[ntohl(mp4->sttsv_count) * 2 + 0] = htonl(1);
        mp4->sttsv_buf[ntohl(mp4->sttsv_count) * 2 + 1] = htonl(mp4->vpts_last ? pts - mp4->vpts_last : 1000 / mp4->frate);
        mp4->sttsv_count = htonl(ntohl(mp4->sttsv_count) + 1);
        mp4->vpts_last = pts;
    }
#endif

    if (mp4->stcov_buf && ntohl(mp4->stcov_count) < mp4->vframemax) {
        mp4->stcov_buf[ntohl(mp4->stcov_count)] = htonl(mp4->chunk_off);
        mp4->stcov_count = htonl(ntohl(mp4->stcov_count) + 1);
    }

    mp4->mdat_size = htonl(ntohl(mp4->mdat_size) + len + sizeof(uint32_t));
    mp4->chunk_off+= len + sizeof(uint32_t);
    fwrite(&fsize, 1, sizeof(uint32_t), mp4->fp);
    fwrite(buf, 1, len, mp4->fp);

    if (ntohl(mp4->stszv_count) % (mp4->frate * 5) == 0) {
        write_fixed_trackv_data(mp4);
        write_fixed_tracka_data(mp4);
    }
}

void mp4muxer_audio(void *ctx, unsigned char *buf, int len, unsigned pts)
{
    MP4FILE *mp4 = (MP4FILE*)ctx;
    if (!ctx) return;

    if (mp4->stsza_buf && ntohl(mp4->stsza_count) < mp4->aframemax) {
        mp4->stsza_buf[ntohl(mp4->stsza_count)] = htonl(len);
        mp4->stsza_count = htonl(ntohl(mp4->stsza_count) + 1);
    }

#if AUDIO_TIMESCALE_BY_SAMPLE_RATE
    if (mp4->sttsa_buf) {
        mp4->sttsa_buf[0] = mp4->stsza_count;
        mp4->sttsa_buf[1] = htonl(mp4->sampnum);
        mp4->sttsa_count  = htonl(1);
    }
#else
    if (mp4->sttsa_buf && ntohl(mp4->sttsa_count) < mp4->aframemax) {
        mp4->sttsa_buf[ntohl(mp4->sttsa_count) * 2 + 0] = htonl(1);
        mp4->sttsa_buf[ntohl(mp4->sttsa_count) * 2 + 1] = htonl(mp4->apts_last ? pts - mp4->apts_last : 1000 * mp4->sampnum / mp4->samprate);
        mp4->sttsa_count = htonl(ntohl(mp4->sttsa_count) + 1);
        mp4->apts_last = pts;
    }
#endif

    if (mp4->stcoa_buf && ntohl(mp4->stcoa_count) < mp4->aframemax) {
        mp4->stcoa_buf[ntohl(mp4->stcoa_count)] = htonl(mp4->chunk_off);
        mp4->stcoa_count = htonl(ntohl(mp4->stcoa_count) + 1);
    }

    mp4->mdat_size = htonl(ntohl(mp4->mdat_size) + len);
    mp4->chunk_off+= len;
    fwrite(buf, 1, len, mp4->fp);
}

void mp4muxer_spspps(void *ctx, uint8_t *spsbuf, int spslen, uint8_t *ppsbuf, int ppslen)
{
    MP4FILE *mp4 = (MP4FILE*)ctx;
    if (!ctx) return;
    if (spsbuf && spslen) {
        mp4->avcc_sps_len = spslen < sizeof(mp4->avcc_sps_data) ? spslen : sizeof(mp4->avcc_sps_data);
        memcpy(mp4->avcc_sps_data, spsbuf, mp4->avcc_sps_len);
        mp4->avcc_sps_len = htonl(mp4->avcc_sps_len) >> 16;
        mp4->avcc_avc_profile    = mp4->avcc_sps_data[1];
        mp4->avcc_profile_compat = mp4->avcc_sps_data[2];
        mp4->avcc_avc_level      = mp4->avcc_sps_data[3];
        fseek(mp4->fp, offsetof(MP4FILE, avcc_avc_profile), SEEK_SET);
        fwrite(&mp4->avcc_avc_profile, 1, 3, mp4->fp);
        fseek(mp4->fp, offsetof(MP4FILE, avcc_sps_len), SEEK_SET);
        fwrite(&mp4->avcc_sps_len, 1, 2 , mp4->fp); fwrite(mp4->avcc_sps_data, 1, ntohl(mp4->avcc_sps_len << 16), mp4->fp);
        fseek(mp4->fp, 0, SEEK_END);
    }
    if (ppsbuf && ppslen) {
        mp4->avcc_pps_len = ppslen < sizeof(mp4->avcc_pps_data) ? ppslen : sizeof(mp4->avcc_pps_data);
        memcpy(mp4->avcc_pps_data, ppsbuf, mp4->avcc_pps_len);
        mp4->avcc_pps_len = htonl(mp4->avcc_pps_len) >> 16;
        fseek(mp4->fp, offsetof(MP4FILE, avcc_sps_data) + ntohl(mp4->avcc_sps_len << 16), SEEK_SET);
        fwrite(&mp4->avcc_pps_num, 1, 3 , mp4->fp); fwrite(mp4->avcc_pps_data, 1, ntohl(mp4->avcc_pps_len << 16), mp4->fp);
        fseek(mp4->fp, 0, SEEK_END);
    }
}

void mp4muxer_aacdecspecinfo(void *ctx, unsigned char *specinfo)
{
    MP4FILE *mp4 = (MP4FILE*)ctx;
    if (!ctx || !specinfo) return;
    mp4->esds_decspec_info = (specinfo[1] << 8) | (specinfo[0] << 0);
    fseek(mp4->fp, mp4->sttsa_off - offsetof(MP4FILE, sttsa_size) + offsetof(MP4FILE, esds_decspec_info), SEEK_SET);
    fwrite(&mp4->esds_decspec_info, 1, sizeof(mp4->esds_decspec_info), mp4->fp);
    fseek(mp4->fp, 0, SEEK_END);
}

#ifdef _TEST_
int main(void)
{
    uint8_t sps[] = { 103,77,0,40,229,64,60,1,19,242,160 };
    uint8_t pps[] = { 104,238,49,18 };
    uint8_t dsi[] = { 0x12, 0x34 };
    void *mp4 = mp4muxer_init("test.mp4", 60000, 1920, 1080, 25, 50, 1, 8000, 16, 1024, NULL);
    mp4muxer_spspps(mp4, sps, sizeof(sps), pps, sizeof(pps));
    mp4muxer_aacdecspecinfo(mp4, dsi);
    mp4muxer_exit(mp4);
    return 0;
}
#endif

