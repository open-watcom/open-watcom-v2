#ifndef __BMPEPS_H__
#define __BMPEPS_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "pushpck1.h"
typedef struct bmp_file_header {
    unsigned short  type;
    unsigned long   size;
    unsigned short  reserved1;
    unsigned short  reserved2;
    unsigned long   off_bits;
} bmp_file_header;

typedef struct bmp_info_header {
    unsigned long   size;
    long            width;
    long            height;
    unsigned short  planes;
    unsigned short  bit_count;
    unsigned long   compression;
    unsigned long   size_image;
    long            x_ppm;
    long            y_ppm;
    unsigned long   clr_used;
    unsigned long   clr_important;
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

