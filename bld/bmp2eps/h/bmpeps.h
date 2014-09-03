#ifndef __BMPEPS_H__
#define __BMPEPS_H__

#include <stdio.h>
#include "watcom.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "pushpck1.h"
typedef struct bmp_file_header {
    unsigned short  type;
    uint_32         size;
    unsigned short  reserved1;
    unsigned short  reserved2;
    uint_32         off_bits;
} bmp_file_header;

typedef struct bmp_info_header {
    uint_32         size;
    int_32          width;
    int_32          height;
    unsigned short  planes;
    unsigned short  bit_count;
    uint_32         compression;
    uint_32         size_image;
    int_32          x_ppm;
    int_32          y_ppm;
    uint_32         clr_used;
    uint_32         clr_important;
} bmp_info_header;
#include "poppck.h"

typedef struct bmp_rgb_quad {
    unsigned char   blue;
    unsigned char   green;
    unsigned char   red;
    unsigned char   reserved;
} bmp_rgb_quad;

typedef struct bmp_rgb_triplet {
    unsigned char   blue;
    unsigned char   green;
    unsigned char   red;
} bmp_rgb_triplet;

int bmeps_bmp(FILE *out, FILE *in, char *name);
int bmeps_bmp_bb(FILE *out, FILE *in, char *name);
int bmeps_bmp_wh(FILE *in, unsigned long *w, unsigned long *h);

#ifdef __cplusplus
}
#endif

#endif

