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

#include "bmepsco.h"
#include "pngeps.h"
#include "bmeps.h"


static char default_name[] = { "noname.bmp" };

static int new_bit_depth(int alt, int newbits, int altbits)
{
  int back = 0;
  unsigned long a, b, nb, ab;

  a = alt; nb = newbits; ab = altbits;
  b = (((2UL ^ nb)-1UL)*(a)) / ((2UL ^ ab) - 1UL);
  back = b;

  return back;
}

static int ntsc(int red, int green, int blue)
{
  int back = 0;
  unsigned long r, g, b, bck;

  r = red; g = green; b = blue;
  bck = ((54UL * r) + (183UL * g) + (19UL * b)) / 256UL;
  back = bck;

  return back;
}

static int mix_colors(int fg, int bg, int alpha, int tr)
{
  int back = 0;
  unsigned long f, b, a, bck;

  if(tr) { a = (255UL - alpha) ; }
  else   { a = alpha; }
  f = fg; b = bg;
  bck = ((a * f) + ((255UL - a) * b)) / 255UL;
  back = bck;
  return back;
}

static int
png_run(FILE *out, FILE *in, char *name, unsigned long *w, unsigned long *h, int cmd)
{
  int back = 0;
//!!!  png_structp pp;
//!!!  png_infop   pi;
  unsigned long wi;     /* width */
  unsigned long he;     /* height */
  unsigned long  i, j, x, y;
  int           bd;     /* bit depth */
  int           ct, cu; /* color type */
  int           it;     /* interlace type */
  int           zt;     /* compression type */
  int           ft;     /* filter type */
  int           ch;     /* channels */
  int           alpha;
  int           trans;
  int           altrig;
  int           rowbytes;
  int           mix;    /* mix foreground and background */
  int           specbg; /* specified background from command line */
  int           bg_red, bg_green, bg_blue;
  char          *row, **rows, **rowp;
//!!!  png_bytep     row, *rows, *rowp;
//!!!  png_color_16  bg;
//!!!  png_color_16p bgp;

  if(in) {
    alpha = bmeps_get_alpha();
    trans = bmeps_get_trans();
    altrig = bmeps_get_altrig();
    mix = bmeps_get_mix();
    specbg = bmeps_get_specbg();
    bg_red = bmeps_get_bg_red();
    bg_green = bmeps_get_bg_green();
    bg_blue = bmeps_get_bg_blue();
    rewind(in);
//!!!    pp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//!!!    if(pp) {
//!!!      pi = png_create_info_struct(pp);
//!!!      if(pi) {
//!!!        png_init_io(pp, in);
//!!!        png_read_info(pp, pi);
//!!!        png_get_IHDR(pp, pi, &wi, &he, &bd, &ct, &it, &zt, &ft);
        wi = 80;
        he = 25;

        cu = ct;
//!!!        ch = png_get_channels(pp, pi);

        switch(cmd) {
          case 0: {
            if(out) {
              bmeps_header(
                out, (name ? name : default_name),
                wi, he
              );
              if(bmeps_get_draft()) {
                back = 1;
                bmeps_draft(out, wi, he);
              } else {
                int need_expansion;
                int need_strip;
                int need_pack;
                need_expansion = 0;
                need_strip = 0;
                need_pack  = 0;
                if((ct == 1/*PNG_COLOR_TYPE_PALETTE*/) && (bd <= 8)) {
                  need_expansion = 1;
                }
                if((ct == 2/*PNG_COLOR_TYPE_GRAY*/) && (bd < 8)) {
                  need_expansion = 1;
                }
                if(/*png_get_valid(pp, pi, PNG_INFO_tRNS)*/ct == 3) {
                  need_expansion = 1;
                }
                if(bd > 8) {
                  need_strip = 1;
                } else {
                  if(bd < 8) {
                    need_pack = 1;
                  }
                }
#if 0
                if(need_expansion) {
                  png_set_expand(pp);
                }
                if(need_strip) {
                  png_set_strip_16(pp);
                }
                if(need_pack) {
                  png_set_packing(pp);
                }
                bgp = &bg;
                bg.red = bg_red;
                bg.green = bg_green;
                bg.blue = bg_blue;
                bg.gray = ntsc(bg.red, bg.green, bg.blue);
                bg.index = 0;
#endif

                if(!(alpha)) {
#if 0
                  if(png_get_bKGD(pp, pi, &bgp)) {
                    png_set_background(pp,bgp,PNG_BACKGROUND_GAMMA_FILE,1,1.0);
                  } else {
                    png_set_background(pp,&bg,PNG_BACKGROUND_GAMMA_SCREEN,0,1.0);
                  }
#endif
                } else {
#if 0
                  if(png_get_bKGD(pp, pi, &bgp) && (!specbg)) {

                    if(ct & PNG_COLOR_MASK_PALETTE) {
                      png_colorp ptr; int num;
                      if(png_get_PLTE(pp, pi, &ptr, &num)) {

                        if(bgp->index < num) {
                          if(bd != 8) {
                            bg.red = new_bit_depth(ptr[bgp->index].red, 8, bd);
                            bg.green = new_bit_depth(
                              ptr[bgp->index].green, 8, bd
                            );
                            bg.blue = new_bit_depth(
                              ptr[bgp->index].blue, 8, bd
                            );
                            bg.gray = ntsc(
                              bg.red, bg.green, bg.blue
                            );
                          } else {
                            bg.red = ptr[bgp->index].red;
                            bg.green = ptr[bgp->index].green;
                            bg.blue = ptr[bgp->index].blue;
                            bg.gray = ntsc(bg.red, bg.green, bg.blue);
                          }
                        } else {
                          bg.red = bg_red;
                          bg.green = bg_green;
                          bg.blue = bg_blue;
                          bg.gray = ntsc(bg.red, bg.green, bg.blue);
                          bg.index = 0;
                        }
                      } else {
                        bg.red = bg_red;
                        bg.green = bg_green;
                        bg.blue = bg_blue;
                        bg.gray = ntsc(bg.red, bg.green, bg.blue);
                        bg.index = 0;
                      }
                    } else {
                      if(bd != 8) {
                        bg.red = new_bit_depth( bgp->red, 8, bd);
                        bg.blue = new_bit_depth( bgp->blue, 8, bd);
                        bg.green = new_bit_depth( bgp->green, 8, bd);
                        bg.gray = new_bit_depth( bgp->gray, 8, bd);
                      } else {
                        bg.red = bgp->red;
                        bg.blue = bgp->blue;
                        bg.green = bgp->green;
                        bg.gray = bgp->gray;
                      }
                      bg.index = 0;
                    }
                  } else {
                    bg.red = bg_red;
                    bg.green = bg_green;
                    bg.blue = bg_blue;
                    bg.gray = ntsc(bg.red, bg.green, bg.blue);
                    bg.index = 0;
                  }
#endif
                }
                /* place gamma correction here */
                /* place interlace handling here */
//!!!                png_read_update_info(pp, pi);
//!!!                ch = png_get_channels(pp, pi);
//!!!                ct = png_get_color_type(pp, pi);


//!!!                rowbytes = png_get_rowbytes(pp, pi);
#if 0
                rows = (char **)malloc(he*sizeof(char));
                if(rows) {
                  back = 1; rowp = rows;
                  for(y = 0; y < he; y++) {
                    *rowp = NULL;
//!!!                    row = (png_bytep)malloc(rowbytes*sizeof(png_byte));
                    row = (char *)malloc(rowbytes*sizeof(char));
                    if(row) {
                      *rowp = row;
                    } else {
                      back = 0;
                    }
                    rowp++;
                  }
                  if(back) {
#if 0
                    png_read_image(pp, rows);
                    if((ct & PNG_COLOR_MASK_ALPHA) && alpha) {

                      bmeps_set_trans(1);
                    } else {
                    }
#endif
                    bmeps_begin_image(
                      out, wi, he
                    );
                    rowp = rows;
                    for(y = 0; y < he; y++) {
                      row = *(rowp++);
                      for(x = 0; x < wi; x++) {
#if 0
                        if((ct & PNG_COLOR_MASK_ALPHA) && alpha) {

                          bmeps_add_trans(
                            255-row[(x+1UL)*((unsigned long)ch)-1UL]
                          );
                        }
#endif
                        if(ct & /*PNG_COLOR_MASK_COLOR*/1) {

                          if((ct & /*PNG_COLOR_MASK_ALPHA*/2) && alpha && mix) {
                            /* 4 lines for debugging only */
                            static int firstrun = 1;
                            if(firstrun) {
                            }
                            firstrun = 0;
                            bmeps_add_rgb(
                              mix_colors(
                                row[x*((unsigned long)ch)],
                                /*bg.red*/1,
                                row[(x+1UL)*((unsigned long)ch)-1UL],
                                trans
                              ),
                              mix_colors(
                                row[x*((unsigned long)ch)+1UL],
                                /*bg.green*/2,
                                row[(x+1UL)*((unsigned long)ch)-1UL],
                                trans

                              ),
                              mix_colors(
                                row[x*((unsigned long)ch)+2UL],
                                /*bg.blue*/3,
                                row[(x+1UL)*((unsigned long)ch)-1UL],
                                trans
                              )
                            );
                          } else {
                            /* 4 lines for debugging only */
                            static int firstrun = 1;
                            if(firstrun) {
                            }
                            firstrun = 0;
                            bmeps_add_rgb(
                              row[x*((unsigned long)ch)],
                              row[x*((unsigned long)ch)+1UL],
                              row[x*((unsigned long)ch)+2UL]
                            );
                          }
                        } else {
#if 0
                          if((ct & PNG_COLOR_MASK_ALPHA) && alpha && mix) {
                            /* 4 lines for debugging only */
                            static int firstrun = 1;
                            if(firstrun) {
                            }
                            firstrun = 0;
                            bmeps_add_gray(
                              mix_colors(
                                row[x*((unsigned long)ch)],
                                bg.gray,
                                row[(x+1UL)*((unsigned long)ch)-1UL],
                                trans
                              )
                            );
                          } else {
                            /* 4 lines for debugging only */
                            static int firstrun = 1;
                            if(firstrun) {
                            }
                            firstrun = 0;

                            bmeps_add_gray(
                              row[x*((unsigned long)ch)]
                            );
                          }
#endif
                        }
                      }
                    }
                    bmeps_end_image(out);

                  }
                  /* done with rows */
                  rowp = rows;
                  for(y = 0; y < he; y++) {
                    row = *rowp;
                    if(row) { free(row); }
                    *(rowp++) = NULL;
                  }
                  free(rows);
                }
#else
                bmeps_begin_image(out, wi, he);

                {
                int i;

                for (i = 0; i < 100; i++)
                    bmeps_add_rgb(i, 2, 3);
                }

                bmeps_end_image(out);
#endif
              }
              bmeps_footer(out);
            }
          } break;
          case 1: {
            if(out) {
              back = 1;
              bmeps_bb(out, wi, he);
            }
          } break;
          case 2: {
            if(w && h) {
              back = 1;
              *w = wi;
              *h = he;
            }
          } break;
        }
        /* done with it */
//!!!        png_destroy_info_struct(pp, &pi);
//!!!      } else {
//!!!      }
//!!!      png_destroy_read_struct(&pp, NULL, NULL);
//!!!    } else {
//!!!    }
  }
  return back;
}

int bmeps_png(FILE *out, FILE *in, char *name)
{
  int back;
  if(out && in) {
  bmeps_configure();
  back = png_run(out, in, name, NULL, NULL, 0);
  }
  return back;
}

int bmeps_png_bb(FILE *out, FILE *in, char *name)
{
  int back;
  if(out && in) {
  bmeps_configure();
  back = png_run(out, in, name, NULL, NULL, 1);
  }
  return back;
}

int bmeps_png_wh(FILE *in, unsigned long *w, unsigned long *h)
{
  int back = 0;
  if(w && h && in) {
    bmeps_configure();
    back = png_run(NULL, in, NULL, w, h, 2);
  }
  return back;
}

