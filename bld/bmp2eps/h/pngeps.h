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

#ifndef PNGEPS_INC
#define PNGEPS_INC 1

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int bmeps_png(FILE *out, FILE *in, char *name);
int bmeps_png_bb(FILE *out, FILE *in, char *name);
int bmeps_png_wh(FILE *in, unsigned long *w, unsigned long *h);

#ifdef __cplusplus
}
#endif
#endif
/* ifndef PNGEPS_INC */

