/*
 * libbmeps - Bitmap to EPS conversion library
 * Copyright (C) 2000 - Dirk Krause
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * In this package the copy of the GNU Library General Public License
 * is placed in file COPYING.
 */

#ifndef BMEPS_INC
#define BMEPS_INC 1

#include <stdio.h>

typedef struct {
  int psl; int col; int a85; int rl; int fl;
  int alpha; int trans; int altrig;
  int mix; int specbg; int bg_red; int bg_green; int bg_blue; int dsc_show;
} BmepsConfiguration;

#ifdef __cplusplus
extern "C" {
#endif

void bmeps_setup(
  int psl, int col, int a85, int rl, int fl,
  int alpha, int trans, int altrig,
  int mix, int specbg, int bg_red, int bg_green, int bg_blue, int dsc_show
);
void bmeps_cfg(char *options);
void bmeps_set_draft(int dr);
int  bmeps_get_pslevel(void);
int  bmeps_get_colored(void);
int  bmeps_get_enc_a85(void);
int  bmeps_get_enc_rl(void);
int  bmeps_get_enc_fl(void);
int  bmeps_get_draft(void);
int  bmeps_get_alpha(void);
int  bmeps_get_trans(void);
int  bmeps_get_altrig(void);
int  bmeps_get_dsc_comm(void);
void bmeps_configure(void);
void bmeps_version(FILE *out);
int bmeps_get_mix(void);
int bmeps_get_specbg(void);
int bmeps_get_bg_red(void);
int bmeps_get_bg_green(void);
int bmeps_get_bg_blue(void);


/*
        Support function used by handler functions
*/
void bmeps_bb(FILE *out, unsigned long w, unsigned long h);
void bmeps_header(FILE *out, char *name, unsigned long w, unsigned long h);
void bmeps_begin_image(FILE *out, unsigned long w, unsigned long h);
void bmeps_add_rgb(int red, int green, int blue);
void bmeps_add_gray(int gray);
void bmeps_add_trans(int alpha);
void bmeps_end_image(FILE *out);
void bmeps_draft(FILE *out, unsigned long w, unsigned long h);
void bmeps_footer(FILE *out);
void bmeps_set_trans(int flag);

void bmeps_get_cfg(BmepsConfiguration *bcp);
void bmeps_set_cfg(BmepsConfiguration *bcp);
void bmeps_copy_cfg(BmepsConfiguration *d, BmepsConfiguration *s);

char *bmeps_tmpname(void);
void bmeps_delete(char *filename);

/*
        Invocation points
*/
int  bmeps_run(FILE *out, FILE *in, char *name);
int  bmeps_run_bb(FILE *out, FILE *in, char *name);
int  bmeps_wh(FILE *in, char *name, unsigned long *w, unsigned long *h);
int  bmeps_can_handle(char *name);

/*
        Handler functions invoked by the above functions
*/
int  bmeps_png(FILE *out, FILE *in, char *name);
int  bmeps_png_bb(FILE *out, FILE *in, char *name);
int  bmeps_png_wh(FILE *in, unsigned long *w, unsigned long *h);

#ifdef __cplusplus
}
#endif
/* #ifdef __cplusplus */

#define MAXPSLEVEL 3

#endif
/* BMEPS_INC */

