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

#ifndef OE_H_INCLUDED
#define OE_H_INCLUDED

#include <stdio.h>

typedef unsigned char Bytef;

typedef struct {
  int mode;
  FILE *out;
  int textpos;
  unsigned long a85_value;
  int      a85_consumed;
  unsigned long a85_4;
  unsigned long a85_3;
  unsigned long a85_2;
  unsigned long a85_1;
  unsigned long a85_0;
  int rl_lastbyte;
  int *rl_buffer;
  int rl_bufused;
  int rl_state;
  int bit_value;
  int bit_consumed;
  Bytef *fl_i_buffer;
  Bytef *fl_o_buffer;
#if 0
  z_stream flate_stream;
  uLong  fl_i_size;
  uLong  fl_o_size;
  uLong  fl_i_used;
  int    flate_rate;
#endif
} Output_Encoder;

#ifdef __cplusplus
extern "C" {
#endif

void oe_init(Output_Encoder *o, FILE *out, int mode, int rate, int *buf
//!!!  Bytef *flib, size_t flis, Bytef *flob, size_t flos
);
void oe_byte_add(Output_Encoder *o, int b);
void oe_byte_flush(Output_Encoder *o);
void oe_bit_add(Output_Encoder *o, int b);
void oe_bit_flush(Output_Encoder *o);

#ifdef __cplusplus
}
#endif



#define OE_ASC85 1
#define OE_FLATE 2
#define OE_RL    4

#endif

