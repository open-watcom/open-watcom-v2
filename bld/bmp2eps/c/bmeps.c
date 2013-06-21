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
#include "bmepsoe.h"

#include <ctype.h>

#include "bmpeps.h"
#ifdef HAVE_PNG_H
#include "pngeps.h"
#endif
#define BMEPS_C 1
#include "bmeps.h"
#include "clibext.h"

static char the_version_number[] = { VERSNUMB };


/* Postscript level */
static int bmeps_pslevel = 2;

/* colored (1) or grayscaled (0) output */
static int bmeps_color   = 0;

/* use ASCII85 encoding */
static int bmeps_enc_a85 = 1;

/* use runlength encoding */
static int bmeps_enc_rl  = 1;

/* use flate encoding */
static int bmeps_enc_fl  = 0;

/* draft only */
static int bmeps_is_draft   = 0;

/* print DSC comments */
static int show_dsc_comments = 0;


static Output_Encoder bmepsoe;


static int bmeps_alpha = 0;     /* use alpha channel */
static int bmeps_trans = 0;     /* alpha channel is transparency */
static int bmeps_altrig = 0;    /* 0/1 trigger level alternated */
static int bmeps_mix = 0;       /* mix foreground with background */
static int bmeps_specbg = 0;    /* specify your own background color */
static int bmeps_bg_red = 255;
static int bmeps_bg_green = 255;
static int bmeps_bg_blue = 255;

int bmeps_get_mix(void) { return bmeps_mix ; }
int bmeps_get_specbg(void) { return bmeps_specbg; }
int bmeps_get_bg_red(void) { return bmeps_bg_red; }
int bmeps_get_bg_green(void) { return bmeps_bg_green; }
int bmeps_get_bg_blue(void) { return bmeps_bg_blue; }
int bmeps_get_dsc_comm(void) { return show_dsc_comments; }

static int use_trans = 0;

void bmeps_add_trans(int x) {
  int val;

  val = x;
  if(use_trans) {
    if(bmeps_trans) {
      val = 255 - val;
    }
    /* if(bmeps_aldual) { */
      if(bmeps_altrig) {
        val = (val ? 255 : 0);
      } else {
        val = ((val == 255) ? 255 : 0);
      }
    /* } */

    oe_byte_add(&bmepsoe, val);
  }

}



static int is_configured = 0;

#define correct(x) x = ((x > 255) ? (255) : ((x < 0) ? (0) : (x)))

void bmeps_setup(int psl, int col, int a85, int rl, int fl,
  int alpha, int trans, int altrig,
  int mix, int specbg, int bg_red, int bg_green, int bg_blue, int dsc_show
)
{
  is_configured = 1;
  bmeps_color = bmeps_enc_a85 = bmeps_enc_rl = bmeps_enc_fl = 0;
  bmeps_alpha = bmeps_trans = bmeps_altrig = 0;
  bmeps_mix = bmeps_specbg = 0;
  bmeps_bg_red = bmeps_bg_green = bmeps_bg_blue = 255;
  bmeps_pslevel = psl;
  bmeps_mix = (mix ? 1 : 0);
  bmeps_specbg = (specbg ? 1 : 0);
  bmeps_bg_red = bg_red;
  bmeps_bg_green = bg_green;
  bmeps_bg_blue = bg_blue;
  show_dsc_comments = dsc_show;
  correct(bmeps_bg_red);
  correct(bmeps_bg_green);
  correct(bmeps_bg_blue);
  if(bmeps_pslevel < 1) bmeps_pslevel = 1;
  if(bmeps_pslevel > MAXPSLEVEL) bmeps_pslevel = MAXPSLEVEL;
  if(bmeps_pslevel > 1) {
    bmeps_color = (col ? 1 : 0);
    bmeps_enc_a85 = (a85 ? 1 : 0);
    bmeps_enc_rl  = (rl  ? 1 : 0);
    if(bmeps_pslevel > 2) {
      bmeps_enc_fl = ( fl ? 1 : 0 );
      if(alpha) {
        bmeps_alpha = 1;
        bmeps_trans = (trans ? 1 : 0); bmeps_altrig = (altrig ? 1 : 0);
      }
    }
  }
}

static char *options = NULL;

void bmeps_cfg(char *o)
{
  options = o;
}

void bmeps_set_draft(int dr)
{
  bmeps_is_draft = dr;
}

static int digit_for_char(char c)
{
  int back = 0;
  switch(c) {
          case '0': back = 0; break;
          case '1': back = 1; break;
          case '2': back = 2; break;
          case '3': back = 3; break;
          case '4': back = 4; break;
          case '5': back = 5; break;
          case '6': back = 6; break;
          case '7': back = 7; break;
          case '8': back = 8; break;
          case '9': back = 9; break;
  }
  return back;
}

void bmeps_configure(void)
{
  char *ptr;
  if(!is_configured) {
    is_configured = 1;
    ptr = options;
    if(!ptr) {
      ptr = getenv("EPSOUTPUT");
    }
    if(ptr) {
      int psl;
      int col;
      int a85;
      int rl;
      int fl;
      int dr;
      int f;
      int alpha;
      int trans;
      int altrig;
      int mix;
      int specbg;
      int bg_red;
      int bg_green;
      int bg_blue;
      int digit;
      int digstate;
      int dsc_show;

      digstate = 0;
      dsc_show = show_dsc_comments;
      bg_red = bmeps_bg_red;
      bg_green = bmeps_bg_green;
      bg_blue = bmeps_bg_blue;
      specbg = bmeps_specbg;
      mix = bmeps_mix;
      psl = bmeps_pslevel;
      col = bmeps_color;
      a85 = bmeps_enc_a85;
      rl  = bmeps_enc_rl;
      fl  = bmeps_enc_fl;
      dr  = bmeps_is_draft;
      alpha = bmeps_alpha;
      trans = bmeps_trans;
      altrig = bmeps_altrig;
      f   = 1;
      while(*ptr) {
        switch(*ptr) {
          case 'x' : dsc_show = 1; break;
          case 'c' : col = 1; break;
          case 'g' : col = 0; break;
          case 'r' : if(f) { f = a85 = fl = rl = 0; } rl  = 1; break;
          case 'f' : if(f) { f = a85 = fl = rl = 0; } fl  = 1; break;
          case 'd' : dr  = 1; break;
          case 'p' : dr  = 0; break;
          case 'a' : {
            alpha = 1;
          } break;
          case 'b' : alpha = 0; break;
          case 't' : trans = 1; break;
          case 'o' : trans = 0; break;
          case 'l' : altrig = 1; break;
          case 'm' : mix = 1; break;
          case 's' : specbg = 1; break;
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9': {
            digit = digit_for_char(*ptr);
            switch(digstate) {
              case 0: {
                if(alpha || mix) {
                  bg_green = bg_blue = 0;
                  digstate = 1;
                  bg_red = digit;
                } else {
                  if((digit >= 1) && (digit <=3)) { psl = digit; }
                  if(digit == 8) { if(f) { f = a85 = fl = rl = 0; } a85 = 1; }
                }
              } break;
              case 1: bg_red = 10 * bg_red + digit; break;
              case 2: bg_green = 10 * bg_green + digit; break;
              case 3: bg_blue = 10 * bg_blue + digit; break;
            }
          } break;
          case ',' : {
            if(digstate == 0) {
              bg_red = bg_green = bg_blue = 0;
            }
            digstate += 1;
          } break;
        }
        ptr++;
      }
      bmeps_setup(
        psl,col,a85,rl,fl,
        alpha,trans,altrig,mix,specbg,bg_red,bg_green,bg_blue,dsc_show
      );
      bmeps_set_draft(dr);
    }
  }
}

int  bmeps_get_pslevel(void)
{ return bmeps_pslevel; }

int  bmeps_get_colored(void)
{ return bmeps_color; }

int  bmeps_get_enc_a85(void)
{ return bmeps_enc_a85; }

int  bmeps_get_enc_rl(void)
{ return bmeps_enc_rl; }

int  bmeps_get_enc_fl(void)
{ return bmeps_enc_fl; }

int  bmeps_get_draft(void)
{ return bmeps_is_draft; }

int  bmeps_get_alpha(void)
{ return bmeps_alpha; }

int  bmeps_get_trans(void)
{ return bmeps_trans; }


int bmeps_get_altrig(void)
{ return bmeps_altrig; }

void bmeps_bb(FILE *out, unsigned long w, unsigned long h)
{
  fprintf(out, "%%%%BoundingBox: 0 0 %lu %lu\n", w, h);
}

static int version_not_yet_printed = 1;

void bmeps_header(FILE *out, char *name, unsigned long w, unsigned long h)
{
  char *ptr;
  use_trans = 0;
  fprintf(out, "%%!PS-Adobe-%d.0 EPSF-%d.0\n",
    bmeps_pslevel, bmeps_pslevel
  );
  bmeps_bb(out, w, h);
  if(show_dsc_comments) {
    fprintf(out, "%%%%Creator: bmp2eps\n");
    fprintf(out, "%%%%Title: ");
    if(name) {
      ptr = name;
      while(*ptr) {
        switch(*ptr) {
          case '\r':
          case '\n':
          break;
          case '\\':
            fputc('/', out);
          break;
          default:
            fputc(*ptr, out);
          break;
        }
        ptr++;
      }
    } else {
      fprintf(out, "Bitmap image");
    }
    fprintf(out, "\n");
  }
  if(show_dsc_comments) {
    fprintf(out, "%%%%Pages: 1\n");
    fprintf(out, "%%%%EndComments\n");
  }
}

void bmeps_footer(FILE *out)
{
  if(show_dsc_comments) {
    fprintf(out, "%%%%Trailer\n");
  }
}

static int rlbuffer[129];
static int state = 0;

#define NTSC(r,g,b) (((30 * (r)) + (59 * (g)) + (11 * (b))) / 100)

void bmeps_add_rgb(int red, int green, int blue)
{
  if(state > 0) {
    int r, g, b;
    r = red; g = green; b = blue;
    if(r < 0) r = 256 + r;
    if(g < 0) g = 256 + g;
    if(b < 0) b = 256 + b;
    if(bmeps_color) {
      oe_byte_add(&bmepsoe, r);
      oe_byte_add(&bmepsoe, g);
      oe_byte_add(&bmepsoe, b);
    } else {
      oe_byte_add(&bmepsoe, NTSC(r,g,b) );
    }
    state = 2;
  }
}

void bmeps_add_gray(int gray)
{
  if(state > 0) {
    int g;
    g = gray; if(g < 0) g = g + 256;
    oe_byte_add(&bmepsoe, g);
    if(bmeps_color) {
      oe_byte_add(&bmepsoe, g);
      oe_byte_add(&bmepsoe, g);
    }
    state = 2;
  }
}

void bmeps_set_trans(int x)
{
  if(state == 0) {
  if(bmeps_pslevel > 2) {
    if(bmeps_alpha) {
      use_trans = (x ? 1 : 0);
    }
  } else {
    use_trans = 0;
  }
  }
}

static int imageno = 1;

void bmeps_begin_image(FILE *out, unsigned long w, unsigned long h)
{
  if(out) {
    if(!(use_trans && (bmeps_pslevel >  2))) {
      if(show_dsc_comments) {
        fprintf(out, "%%%%BeginProlog\n");
        fprintf(out, "%%%%BeginResource: (bmeps procedures) 1.0 1\n");
      }
      fprintf(out, "/pstr\n  %lu string\ndef\n", w);
      if(bmeps_pslevel > 1) {
        fprintf(out, "/inputf\n  currentfile\n");
        if(bmeps_enc_a85) {
          fprintf(out, "  /ASCII85Decode   filter\n");
        } else {
          fprintf(out, "  /ASCIIHexDecode  filter\n");
        }
        if(bmeps_enc_fl) {
          fprintf(out, "  /FlateDecode     filter\n");
        }
        if(bmeps_enc_rl) {
          fprintf(out, "  /RunLengthDecode filter\n");
        }
        fprintf(out, "def\n");
      }
      if(show_dsc_comments) {
        fprintf(out, "%%%%EndResource\n");
        fprintf(out, "%%%%EndProlog\n");
        fprintf(out, "%%%%BeginSetup\n");
        fprintf(out, "%%%%EndSetup\n");
      }
    } else {
      if(show_dsc_comments) {
        fprintf(out, "%%%%BeginSetup\n");
      }
      if(bmeps_color) {
        fprintf(out, "/DeviceRGB setcolorspace\n");
      } else {
        fprintf(out, "/DeviceGray setcolorspace\n");
      }
      if(show_dsc_comments) {
        fprintf(out, "%%%%EndSetup\n");
      }
    }
    if(show_dsc_comments) {
      fprintf(out, "%%%%Page: Image%d 1\n", imageno++);
    }

    if(version_not_yet_printed) {
      version_not_yet_printed = 0;
      fprintf(out,
        "%%\n%% created by bmp2eps %s \n%%\n",
        the_version_number
      );
    }

    fprintf(out, "gsave\n");
    fprintf(out, "0 %lu translate\n", h);
    fprintf(out, "%lu %lu scale\n", w, h);
    if(use_trans && (bmeps_pslevel >  2)) {
      fprintf(out, "<<\n");
      fprintf(out, "  /ImageType 3\n");
      fprintf(out, "  /DataDict\n  <<\n");
      fprintf(out, "    /ImageType 1\n");
      fprintf(out, "    /Width %lu\n", w);
      fprintf(out, "    /Height %lu\n", h);
      fprintf(out, "    /ImageMatrix [%lu 0 0 -%lu 0 0]\n", w, h);
      fprintf(out, "    /MultipleDataSources false\n");
      fprintf(out, "    /DataSource\n      currentfile");
      if(bmeps_enc_a85) {
        fprintf(out, "\n      /ASCII85Decode filter");
      } else {
        fprintf(out, "\n      /ASCIIHexDecode filter");
      }
      if(bmeps_enc_fl) {
        fprintf(out, "\n      /FlateDecode filter");
      }
      if(bmeps_enc_rl) {
        fprintf(out, "\n      /RunLengthDecode filter");
      }
      fprintf(out, "\n");
      fprintf(out, "    /BitsPerComponent 8\n");
      if(bmeps_color) {
      fprintf(out, "    /Decode [ 0 1 0 1 0 1 ]\n");
      } else {
      fprintf(out, "    /Decode [ 0 1 ]\n");
      }
      fprintf(out, "  >>\n");
      fprintf(out, "  /MaskDict\n  <<\n");
      fprintf(out, "    /ImageType 1\n");
      fprintf(out, "    /Width %lu\n", w);
      fprintf(out, "    /Height %lu\n", h);
      fprintf(out, "    /ImageMatrix [%lu 0 0 -%lu 0 0]\n", w, h);
      fprintf(out, "    /BitsPerComponent 8\n");
      fprintf(out, "    /Decode [ 0 1 ]\n");
      fprintf(out, "  >>\n");
      fprintf(out, "  /InterleaveType 1\n");
      fprintf(out, ">>\nimage\n");
    } else {
      fprintf(out, "%lu %lu 8 [%lu 0 0 -%lu 0 0]\n",
            w, h, w, h
      );
      if(bmeps_pslevel > 1) {
        if(bmeps_color) {
          fprintf(out, "{ inputf pstr readstring pop }\n");
          fprintf(out, "false\n3\ncolorimage\n");
        } else {
          fprintf(out, "{ inputf pstr readstring pop }\n");
          fprintf(out, "image\n");
        }
      } else {
        fprintf(out, "{ currentfile pstr readhexstring pop }\n");
        fprintf(out, "image\n");
      }
    }
    {
      int mode;
      mode = 0;
      if(bmeps_enc_a85) {
        mode |= OE_ASC85;
      }
      if(bmeps_enc_rl) {
        mode |= OE_RL;
      }
      oe_init(&bmepsoe, out, mode, 9, rlbuffer);
      state = 1;
    }
  }
}

void bmeps_end_image(FILE *out)
{
  if(out) {
    if(state > 1) {
      oe_byte_flush(&bmepsoe);
      state = 0;
    }
    if(bmeps_pslevel > 1) {
      if(bmeps_enc_a85) {
          fprintf(out, "%s\n", "~>");
      } else {
          fprintf(out, "%s\n", ">");
      }
    }
    fprintf(out, "grestore\n");
    fprintf(out, "showpage\n");
  }
}

void bmeps_draft(FILE *out, unsigned long w, unsigned long h)
{
    if(show_dsc_comments) {
      fprintf(out, "%%%%EndProlog\n");
      fprintf(out, "%%%%Page: 1 1\n");
    }
    fprintf(out, "gsave\n");
    fprintf(out, "0.75 setgray\n");
    fprintf(out, "newpath\n");
    fprintf(out, "0 0 moveto\n");
    fprintf(out, "%lu 0 lineto\n", w);
    fprintf(out, "%lu %lu lineto\n", w, h);
    fprintf(out, "0 %lu lineto\n", h);
    fprintf(out, "closepath\n");
    fprintf(out, "fill\n");
    fprintf(out, "1 setlinewidth\n");
    fprintf(out, "0 setgray\n");
    fprintf(out, "newpath\n");
    fprintf(out, "1 1 moveto\n");
    fprintf(out, "%lu 1 lineto\n", (w - 1UL));
    fprintf(out, "%lu %lu lineto\n", (w - 1UL), (h - 1UL));
    fprintf(out, "1 %lu lineto\n", (h - 1UL));
    fprintf(out, "closepath\n");
    fprintf(out, "stroke\n");
    fprintf(out, "newpath\n");
    fprintf(out, "1 1 moveto\n");
    fprintf(out, "%lu %lu lineto\n", (w - 1UL), (h - 1UL));
    fprintf(out, "stroke\n");
    fprintf(out, "newpath\n");
    fprintf(out, "1 %lu moveto\n", (h - 1UL));
    fprintf(out, "%lu 1 lineto\n", (w - 1UL));
    fprintf(out, "stroke\n");
    fprintf(out, "grestore\n");
    fprintf(out, "showpage\n");
}

typedef int FCT(FILE *out, FILE *in, char *name);
typedef int FCTWH(FILE *in, unsigned long *w, unsigned long *h);

typedef struct {
  char *suffix;
  FCT *fctrun;
  FCT *fctbb;
  FCTWH *wh;
  char *xsuffix;        /* for version printing */
}
HANDLERENTRY;

HANDLERENTRY handlers[] = {
  { "bmp", bmeps_bmp, bmeps_bmp_bb, bmeps_bmp_wh , "BMP" },
#ifdef HAVE_PNG_H
  { "png", bmeps_png, bmeps_png_bb, bmeps_png_wh , "PNG" },
#endif
  { NULL, NULL, NULL, NULL },
};

static char *get_suffix(char *name)
{
  char *back = NULL;
  char *ptr;
  if(name) {
    ptr = name;
    while(*ptr) {
      switch(*ptr) {
        case '.': back = ptr; break;
        case '/': back = NULL; break;
        case '\\': back = NULL; break;
      }
      ptr++;
    }
  }
  if(back) { back++; }
  return back;
}

#define STRCASECMP(a,b) stricmp(a,b)

FCT *get_run_handler(char *name)
{
  FCT *back = NULL;
  if(name) {
    char *suffix;
    suffix = get_suffix(name);
    if(suffix) {
      HANDLERENTRY *he;
      he = handlers;
      while((he->suffix) && (back == NULL)) {
        if( STRCASECMP(suffix,(he->suffix)) == 0 ) {
          back = he->fctrun;
        }
        he++;
      }
    }
  }
  return back;
}


FCT *get_bb_handler(char *name)
{
  FCT *back = NULL;
  if(name) {
    char *suffix;
    suffix = get_suffix(name);
    if(suffix) {
      HANDLERENTRY *he;
      he = handlers;
      while((he->suffix) && (back == NULL)) {
        if( STRCASECMP(suffix,(he->suffix)) == 0 ) {
          back = he->fctbb;
        }
        he++;
      }
    }
  }
  return back;
}


FCTWH *get_wh_handler(char *name)
{
  FCTWH *back = NULL;
  if(name) {
    char *suffix;
    suffix = get_suffix(name);
    if(suffix) {
      HANDLERENTRY *he;
      he = handlers;
      while((he->suffix) && (back == NULL)) {
        if( STRCASECMP(suffix,(he->suffix)) == 0 ) {
          back = he->wh;
        }
        he++;
      }
    }
  }
  return back;
}


int bmeps_run(FILE *out, FILE *in, char *name)
{
  int back = 0;
  FCT *fct;
  bmeps_configure();
  fct = get_run_handler(name);
  if(fct) {
    back = (*fct)(out, in, name);
  }
  return back;
}

int bmeps_can_handle(char *name)
{
  int back = 0;
  FCT *fct;
  fct = get_run_handler(name);
  if(fct) {
    back = 1;
  }
  return back;
}

int bmeps_run_bb(FILE *out, FILE *in, char *name)
{
  int back = 0;
  FCT *fct;
  fct = get_bb_handler(name);
  if(fct) {
    back = (*fct)(out, in, name);
  }
  return back;
}

int bmeps_wh(FILE *in, char *name, unsigned long *w, unsigned long *h)
{
  int back = 0;
  FCTWH *fct;
  fct = get_wh_handler(name);
  if(fct) {
    back = (*fct)(in, w, h);
  }
  return back;
}


void bmeps_version(FILE *out)
{
  HANDLERENTRY *he;
  char c, *p;
  int issecond;
  if(out) {
    fprintf(out,
      "%% libbmeps\tEPS output and bitmap conversion library %s %s\n",
      the_version_number,
      "(SCCS=1.65)"
    );
    fprintf(out, "%%\t\t(");
    he = handlers; issecond = 0;
    while(he->suffix) {
      if(he->xsuffix) {
        if(issecond) {
          fprintf(out, ", ");
        } else {
          issecond = 1;
        }
        p = he->xsuffix;
        while(*p) {
          c = *p;
          fprintf(out, "%c", ((islower(c)) ? toupper(c) : c));
          p++;
        }
      }
      he++;
    }
    fprintf(out, ")\n");
  }
}

void bmeps_get_cfg(BmepsConfiguration *bcp)
{
  if(bcp) {
    bcp->psl = bmeps_get_pslevel();
    bcp->col = bmeps_get_colored();
    bcp->a85 = bmeps_get_enc_a85();
    bcp->rl  = bmeps_get_enc_rl();
    bcp->fl  = bmeps_get_enc_fl();
    bcp->alpha = bmeps_get_alpha();
    bcp->trans = bmeps_get_trans();
    bcp->altrig = bmeps_get_altrig();
    bcp->mix = bmeps_get_mix();
    bcp->specbg = bmeps_get_specbg();
    bcp->bg_red = bmeps_get_bg_red();
    bcp->bg_green =  bmeps_get_bg_green();
    bcp->bg_blue = bmeps_get_bg_blue();
    bcp->dsc_show = show_dsc_comments;
  }
}

void bmeps_set_cfg(BmepsConfiguration *bcp)
{
  if(bcp) {
    bmeps_setup(
      bcp->psl,
      bcp->col,
      bcp->a85,
      bcp->rl,
      bcp->fl,
      bcp->alpha,
      bcp->trans,
      bcp->altrig,
      bcp->mix,
      bcp->specbg,
      bcp->bg_red,
      bcp->bg_green,
      bcp->bg_blue,
      bcp->dsc_show
    );
  }
}

void bmeps_copy_cfg(BmepsConfiguration *d, BmepsConfiguration *s)
{
  if(d && s) {
    d->psl = s->psl;
    d->col = s->col;
    d->a85 = s->a85;
    d->rl = s->rl;
    d->fl = s->fl;
    d->alpha = s->alpha;
    d->trans = s->trans;
    d->altrig = s->altrig;
    d->mix = s->mix;
    d->specbg = s->specbg;
    d->bg_red = s->bg_red;
    d->bg_green = s->bg_green;
    d->bg_blue = s->bg_blue;
    d->dsc_show = s->dsc_show;
  }
}

char bmeps_getpid_tmpname[32];
char bmeps_default_tmpname[] = { "BMP2EPS.TMP" };
static int configured_tmpname = 0;

char *bmeps_tmpname(void)
{
  char *back = NULL;
  back = bmeps_default_tmpname;
  if(!configured_tmpname) {
    configured_tmpname = 1;
    sprintf(bmeps_getpid_tmpname, "BMP2EPS.%lu.TMP", ((unsigned long)getpid()));
  }
  back = bmeps_getpid_tmpname;
  return back;
}

void bmeps_delete(char *filename)
{
  if(filename) {
    unlink(filename);
  }
}

