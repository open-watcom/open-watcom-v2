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
#include "bmeps.h"
#include "pngeps.h"

static char the_version_number[] = { VERSNUMB };
static char progname[] = { "bmp2eps" };

/* filenames when processing stdin */
static char dummy_bmp[] = { "file.bmp" };
#if HAVE_PNG_H
static char dummy_png[] = { "file.png" };
#endif
#if HAVE_JPEGLIB_H
static char dummy_jpg[] = { "file.jpg" };
#endif
#if HAVE_PNM_H
static char dummy_pnm[] = { "file.pnm" };
#endif
#if HAVE_TIFF_H
static char dummy_tif[] = { "file.tif" };
#endif

static char *dummy_filename = NULL;

static char *gpl_header[] = {
"This is free software; you can redistribute it and/or modify it under the",
"terms of the GNU Library General Public License as published by the",
"Free Software Foundation; either version 2 of the License, or (at your",
"option) any later version.   This software is distributed in the hope that",
"it will be useful, but WITHOUT ANY WARRANTY; without even the implied",
"warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the",
"GNU Library General Public License for more details.   You should have",
"received a copy of the GNU Library General Public License along with this",
"software; if not, write to the",
"Free Software Foundation, Inc., 59 Temple Place - Suite 330,",
"Boston, MA 02111-1307, USA.",
NULL
};
static void print_version(void)
{
  char **ptr;
  printf( "%% bmp2eps %s\n", the_version_number );
  /* printf("%s", "% Support for PNG alpha channels is turned on.\n"); */
  printf("%s\n", "%");
  printf("%s", "% Libraries used:\n");
  printf("%s", "% ---------------\n");
  bmeps_version(stdout);
  printf("%s\n", "%");
  ptr = gpl_header;
  while(*ptr) {
    printf("%% %s\n", *(ptr++));
  }
}

static char *usage_text[] = {
  "bmp2eps [options] [ <inputfile> [ <outputfile> ] ]",
  "",
  "      Options",
  "      -------",
  "",
  "      -p <pslevel>           select one of the following",
  "         1                   (PS level 1)",
  "         2                   (PS level 2 = default)",
  "         3                   (PS level 3)",
  ""
  "      -c                     turns color printing on",
  "      -g                     turns color printing off (default)",
  "",
  "      -e <encoding>          combine the following characters to specify",
  "         8                   ASCII-85-encoding instead of ASCII-Hex",
  "         r                   run-length encoding",
//  "         f                   flate encoding",
  "",
  "      -t <filetype>          choose one of the following",
  "         bmp",
#if HAVE_PNG_H
  "         png",
#endif
#if HAVE_JPEGLIB_H
  "         jpg",
#endif
#if HAVE_PNM_H
  "         pnm",
#endif
#if HAVE_TIFF_H
  "         tif",
#endif

  "",
  "      -d                     draft mode only",
  "         Draft mode means the program reads only header information",
  "         from the bitmap file and prints a dummy image.",
  "",
  "      -a <alphaoptions>      ",
  "         o                   alpha channel is opacity, not transparency",
  "         t                   alpha channel is transparency, not opacity",
  "         l                   alternated transparency trigger level",
  "         m                   mix foreground- and background color",
  "         sd,d,d              specify background color i.e. s128,128,255",
  "         PNG supports transparency channels, having values from",
  "         0 ... 2^n-1. EPS supports simple yes/no decisions to mask",
  "         pixels, so the alpha channels value must be mapped to",
  "         \"use pixel\" or \"mask pixel\". The standard behaviour is",
  "         to mask only pixels which have no opacity at all.",
  "         The alternated trigger level uses only pixels having full",
  "         opacity.",
  "",
  "      -s                     print DSC comments",
  "",
  NULL
};

static void print_usage(void)
{
  char **ptr;
  ptr = usage_text;
  while(*ptr) {
    fprintf(stdout, "%s\n", *ptr);
    ptr++;
  }
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

int main(int argc, char *argv[])
{
  int i;
  int exval = 0;
  int pslevel;
  int colored;
  int enc_a85;
  int enc_rl;
  int enc_fl;
  int is_draft;
  int bbonly;
  int show_usage;
  int show_version;
  int alpha;
  int trans;
  int altrig;
  int mix;
  int specbg;
  int bg_red;
  int bg_green;
  int bg_blue;
  int show_dsc;
  char *inname;
  char *outname;
  char *ptr, **lfdptr;

  /*
        Configure from environment
  */
  bmeps_configure();
  /*
        Retrieve defaults
  */
  pslevel = bmeps_get_pslevel();
  colored = bmeps_get_colored();
  enc_a85 = bmeps_get_enc_a85();
  enc_rl  = bmeps_get_enc_rl();
  enc_fl  = bmeps_get_enc_fl();
  is_draft = bmeps_get_draft();
  alpha   = bmeps_get_alpha();
  trans   = bmeps_get_trans();
  altrig  = bmeps_get_altrig();
  mix     = bmeps_get_mix();
  specbg  = bmeps_get_specbg();
  show_dsc = bmeps_get_dsc_comm();
  bg_red  = bmeps_get_bg_red();
  bg_green = bmeps_get_bg_green();
  bg_blue = bmeps_get_bg_blue();
  bbonly = 0;
  show_version = show_usage = 0;
  inname = outname = NULL;
  /*
        Process options
  */
  if(argc > 1) {
    i = 1;
    lfdptr = argv; lfdptr++;
    while(i < argc) {
      ptr = *lfdptr;
      if(*ptr == '-') {
        ptr++;
        switch(*ptr) {
          case 'p': {
            ptr++;
            if(!(*ptr)) {
              lfdptr++; i++;
              if(i < argc) {
                ptr = *lfdptr;
              }
            }
            if(*ptr) {
              switch(*ptr) {
                case '1': pslevel = 1; break;
                case '2': pslevel = 2; break;
                case '3': pslevel = 3; break;
              }
            }
          } break;
          case 'c': {
            colored = 1;
          } break;
          case 'g': {
            colored = 0;
          } break;
          case 'a': {
            alpha = 1;
            ptr++;
            if(!(*ptr)) {
              lfdptr++; i++;
              if(i < argc) {
                ptr = *lfdptr;
              }
            }
            if(*ptr) {
              int digit, digstate;
              digit = digstate = trans = altrig = 0;
              while(*ptr) {
                switch(*ptr) {
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
                    if(digstate == 0) {
                      bg_red = bg_green = bg_blue = 0;
                      digstate = 1;
                    }
                    switch(digstate) {
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
            }
          } break;
          case 'm' : {
            mix = 1;
            ptr++;
            if(!(*ptr)) {
              lfdptr++; i++;
              if(i < argc) {
                ptr = *lfdptr;
              }
            }
            if(*ptr) {
              int digit, digstate;
              digit = digstate = trans = altrig = 0;
              while(*ptr) {
                switch(*ptr) {
                  case 't' : trans = 1; break;
                  case 'o' : trans = 0; break;
                  case 'l' : altrig = 1; break;
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
                    if(digstate == 0) {
                      bg_red = bg_green = bg_blue = 0;
                      digstate = 1;
                    }
                    switch(digstate) {
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
            }
          } break;
          case 'e': {
            ptr++;
            if(!(*ptr)) {
              lfdptr++; i++;
              if(i < argc) {
                ptr = *lfdptr;
              }
            }
            if(*ptr) {
              enc_a85 = enc_fl = enc_rl = 0;
              while(*ptr) {
                switch(*ptr) {
                  case '8': enc_a85 = 1; break;
                  case 'f': enc_fl  = 1; break;
                  case 'r': enc_rl  = 1; break;
                }
                ptr++;
              }
            }
          } break;
          case 'v': {
            show_version = 1;
          } break;
          case 'h': {
            show_usage = 1;
          } break;
          case 's': {
            show_dsc = 1;
          } break;
          case 'd': {
            is_draft = 1;
          } break;
          case 'b': {
            bbonly = 1;
          } break;
          case 't': {
            ptr++;
            if(!(*ptr)) {
              lfdptr++; i++;
              if(i < argc) {
                ptr = *lfdptr;
              }
            }
            if(*ptr) {
              if(strcmp(ptr, "bmp") == 0) {
                dummy_filename = dummy_bmp;
              }
#if HAVE_PNG_H
              if(strcmp(ptr, "png") == 0) {
                dummy_filename = dummy_png;
              }
#endif
#if HAVE_JPEGLIB_H
              if(strcmp(ptr, "jpg") == 0) {
                dummy_filename = dummy_jpg;
              }
              if(strcmp(ptr, "jpeg") == 0) {
                dummy_filename = dummy_jpg;
              }
#endif
#if HAVE_PNM_H
              if(strcmp(ptr, "pnm") == 0) {
                dummy_filename = dummy_pnm;
              }
#endif
#if HAVE_TIFF_H
              if(strcmp(ptr, "tif") == 0) {
                dummy_filename = dummy_tif;
              }
              if(strcmp(ptr, "tiff") == 0) {
                dummy_filename = dummy_tif;
              }
#endif
            }
          } break;
          case '-': { /* Long option */
            size_t lgt, lgt1;
            int done;
            char str_version[] = { "version" };
            char str_help[] = { "help" };
            char str_pslevel[] = { "pslevel=" };
            char str_colored[] = { "color" };
            char str_grayscale[] = { "grayscale" };
            char str_encoding[] = { "encoding=" };
            char str_draft[] = { "draft" };
            char str_bb[] = { "boundingbox" };
            char str_ft[] = { "filetype=" };
            char str_alpho[] = { "alpha=" };
            char str_alpha[] = { "alpha" };

            ptr++; done = 0;
            lgt = strlen(ptr);
            lgt1 = strlen(str_version);
            if(lgt >= lgt1) {
              if(strncmp(ptr, str_version, lgt1) == 0) {
                done = 1;
                show_version = 1;
              }
            }
            if(!done) {
              lgt1 = strlen(str_alpho);
                if(lgt >= lgt1) {
                  if(strncmp(ptr, str_alpho, lgt1) == 0) {
                    alpha = done = 1;
                    ptr = &(ptr[lgt1]);
                    while(*ptr) {
                      switch(*ptr) {
                        case 'o': trans = 0 ; break;
                        case 't': trans = 1 ; break;
                        case 'l': altrig = 1; break;
                      }
                      ptr++;
                    }
                  }
                }

            }
            if(!done) {
              lgt1 = strlen(str_alpha);
                if(lgt >= lgt1) {
                  if(strncmp(ptr, str_alpho, lgt1) == 0) {
                    alpha = 1; done = 1;
                  }
                }
            }
            if(!done) {
            lgt1 = strlen(str_help);
            if(lgt >= lgt1) {
              if(strncmp(ptr, str_help, lgt1) == 0) {
                done = 1;
                show_usage = 1;
              }
            }
            }
            if(!done) {
            lgt1 = strlen(str_pslevel);
            if(lgt >= lgt1) {
              if(strncmp(ptr, str_help, lgt1) == 0) {
                ptr = &(ptr[lgt1]);
                done = 1;
                switch(*ptr) {
                  case '1': pslevel = 1; break;
                  case '2': pslevel = 2; break;
                  case '3': pslevel = 3; break;
                }
              }
            }
            }
            if(!done) {
              lgt1 = strlen(str_colored);
              if(lgt >= lgt1) {
                if(strncmp(ptr, str_colored, lgt1) == 0) {
                  done = 1;
                  colored = 1;
                }
              }
            }
            if(!done) {
              lgt1 = strlen(str_grayscale);
              if(lgt >= lgt1) {
                if(strncmp(ptr, str_grayscale, lgt1) == 0) {
                  done = 1; colored = 0;
                }
              }
            }
            if(!done) {
              lgt1 = strlen(str_encoding);
              if(lgt >= lgt1) {
                if(strncmp(ptr, str_encoding, lgt1) == 0) {
                  done = 1;
                  ptr = &(ptr[lgt1]);
                  enc_a85 = enc_fl = enc_rl = 0;
                  while(*ptr) {
                    switch(*ptr) {
                      case '8': enc_a85 = 1 ; break;
                      case 'r': enc_rl  = 1 ; break;
                      case 'f': enc_fl  = 1 ; break;
                    }
                    ptr++;
                  }
                }
              }
            }
            if(!done) {
              lgt1 = strlen(str_draft);
              if(lgt >= lgt1) {
                if(strncmp(ptr, str_draft, lgt1) == 0) {
                  done = is_draft = 1;
                }
              }
            }
            if(!done) {
              lgt1 = strlen(str_bb);
              if(lgt >= lgt1) {
                if(strncmp(ptr, str_bb, lgt1) == 0) {
                  done = bbonly = 1;
                }
              }
            }
            if(!done) {
              lgt1 = strlen(str_ft);
              if(lgt >= lgt1) {
                if(strncmp(ptr, str_ft, lgt1) == 0) {
                  done = 1;
                  ptr = &(ptr[lgt1]);
#if HAVE_PNG_H
                  if(strcmp(ptr, "png") == 0) {
                    dummy_filename = dummy_png;
                  }
#endif
#if HAVE_JPEGLIB_H
                  if(strcmp(ptr, "jpg") == 0) {
                    dummy_filename = dummy_jpg;
                  }
                  if(strcmp(ptr, "jpeg") == 0) {
                    dummy_filename = dummy_jpg;
                  }
#endif
                }
              }
            }
          } break;
        }
      } else {
        if(inname) {
          if(!outname) {
            outname = ptr;
          } else {
            show_usage = 1;
          }
        } else {
          inname = ptr;
        }
      }
      lfdptr++; i++;
    }
  }
  /*
        Configure bmeps module
  */
  bmeps_setup(
    pslevel, colored, enc_a85, enc_rl, enc_fl,
    alpha, trans, altrig,
    mix, specbg, bg_red, bg_green, bg_blue, show_dsc
  );
  bmeps_set_draft(is_draft);
  /*
        Start work
  */
  if(show_version || show_usage) {
    if(show_version) {
      print_version();
    }
    if(show_usage) {
      print_usage();
    }
  } else {
    if(inname) {
      FILE *infile;
      infile = fopen(inname, "rb");
      if(infile) {
        if(outname) {
          FILE *outfile;
          outfile = fopen(outname, "w");
          if(outfile) {
            if(bbonly) { exval = bmeps_run_bb(outfile,  infile, inname); }
            else       { exval = bmeps_run(outfile,  infile, inname); }
            fclose(outfile);
          } else {
            fprintf(stderr,
              "%s ERROR - Failed to write to file %s\n",
              progname, outname
            );
          }
        } else {
          if(bbonly) { exval = bmeps_run_bb(stdout, infile, inname); }
          else       { exval = bmeps_run(stdout, infile, inname); }
        }
        fclose(infile);
      } else {
        fprintf(stderr,
          "%s ERROR - Failed to read from file %s\n",
          progname, inname
        );
      }
    } else {
      if(dummy_filename) {
#if !defined( __UNIX__ )
        int m, f, r;
        m = f = 0;

/*
  Get the file descriptor number
*/
        f = fileno(stdin);
/*
  Set the mode type.
*/
        m = O_BINARY;
/*
  Change mode type for file descriptor.
*/
        r = setmode(f,m);
        if(r == -1) {
          fprintf(stderr, "setmode(): Failed to establish binary mode for stdin!\n");
        }
#if DEBUG
        else {
          fprintf(stderr, "setmode(): Binary mode established.\n");
        }
#endif
#endif

        /*
          Now the descriptor is set up if necessary.
        */
        if(bbonly) { exval = bmeps_run_bb(stdout, stdin, dummy_filename); }
        else       { exval = bmeps_run(stdout, stdin, dummy_filename); }
      } else {
        fprintf(stderr,
          "%s ERROR - Need a file type when processing standard input\n",
          progname
        );
      }
    }
  }
  /*
        Exit program
  */

  exval = (exval ? 0 : 1);
  return( exval );
}

