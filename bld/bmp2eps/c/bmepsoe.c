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

/*      Usage short:
        ============
        oetest [ -{ar}+ ]

        -a      ASCII85
        -r      Run length
        -ar     algorithms combined
*/

#include "bmepsco.h"
#include "bmeps.h"
#include "pngeps.h"
#include "bmepsoe.h"


#define RL_RUNLENGTH(i) (257 - (i))
#define RL_STRINGLENGTH(i) ((i) - 1)
#define RL_MAXLENGTH (127)

#define FINALOUTPUT(c) fputc((c),o->out)

void oe_init(Output_Encoder *o, FILE *out, int mode, int rate, int *buf)
{
  rate = rate;

  o->out = out;
  o->mode = mode;
  o->textpos = 0;
  o->a85_value = 0UL; o->a85_consumed = 0;
  o->a85_0 = 1UL;
  o->a85_1 = 85UL;
  o->a85_2 = 85UL * 85UL;
  o->a85_3 = 85UL * o->a85_2;
  o->a85_4 = 85UL * o->a85_3;
  o->rl_lastbyte = 0;
  o->rl_buffer = buf;
  o->rl_bufused = 0;
  o->rl_state = 0;
  o->bit_value = 0;
  o->bit_consumed = 0;
}

static char hexdigits[] = {
  "0123456789ABCDEF"
};

static void asciihex_add(Output_Encoder *o, int b)
{

  FINALOUTPUT(hexdigits[(b/16)]) ;
  FINALOUTPUT(hexdigits[(b%16)]) ;
  o->textpos = o->textpos + 2;
  if(o->textpos >= 64) {
    FINALOUTPUT('\n') ;
    o->textpos = 0;
  }

}

static void asciihex_flush(Output_Encoder *o)
{

  if(o->textpos > 0) {
    FINALOUTPUT('\n') ;
    o->textpos = 0;
  }

}

static char ascii85_char(unsigned long x)
{
  unsigned u;
  int      i;
  char back;
  back = (char)0;
  u = (unsigned)x;
  i = (int)u;
  i += 33;
  back = (char)i;
  return back;
}

static void ascii85_output(Output_Encoder *o)
{
  int i;
  char buffer[6], c, *ptr;
  unsigned long value;
  value = o->a85_value;
  buffer[0] = ascii85_char(value / (o->a85_4));
  value = value % (o->a85_4);
  buffer[1] = ascii85_char(value / (o->a85_3));
  value = value % (o->a85_3);
  buffer[2] = ascii85_char(value / (o->a85_2));
  value = value % (o->a85_2);
  buffer[3] = ascii85_char(value / (o->a85_1));
  value = value % (o->a85_1);
  buffer[4] = ascii85_char(value);
  buffer[5] = '\0';
  i = o->a85_consumed + 1;
  ptr = buffer;
  o->textpos = o->textpos + i;
  while(i--) {
    c = *(ptr++);
    FINALOUTPUT(c) ;
  }
  if(o->textpos >= 64) {
    FINALOUTPUT('\n') ;
    o->textpos = 0;
  }
}

static void ascii85_add(Output_Encoder *o, int b)
{
  unsigned u;
  unsigned long ul;

  u = (unsigned)b;
  ul = (unsigned long)u;
  o->a85_value = 256UL * (o->a85_value) + ul;
  o->a85_consumed = o->a85_consumed + 1;
  if(o->a85_consumed >= 4) {
    ascii85_output(o);
    o->a85_value = 0UL;
    o->a85_consumed = 0;
  }

}

static void ascii85_flush(Output_Encoder *o)
{
  int i;

  if(o->a85_consumed > 0) {
    i = o->a85_consumed;
    while(i < 4) {
      o->a85_value = 256UL * o->a85_value;
      i++;
    }
    ascii85_output(o);
    o->a85_value = 0UL;
    o->a85_consumed = 0;
  }
  if(o->textpos > 0) {
    FINALOUTPUT('\n') ;
    o->textpos = 0;
  }

}

static void after_flate_add(Output_Encoder *o, int b)
{

  if(o->mode & OE_ASC85) {
    ascii85_add(o,b);
  } else {
    asciihex_add(o,b);
  }

}

static void after_flate_flush(Output_Encoder *o)
{
    if(o->mode & OE_ASC85) {
        ascii85_flush(o);
    } else {
        asciihex_flush(o);
    }
}

static void rl_add(Output_Encoder *o, int b)
{
  int lgt, i;
  int *buffer;

  buffer = o->rl_buffer;
  lgt = o->rl_bufused;
  if(buffer) {

    if(lgt > 0) {
      if(o->rl_lastbyte == b) {
        switch(o->rl_state) {
          case 2: {
            buffer[lgt++] = b;
            o->rl_bufused = lgt;
            o->rl_state = 2;
            o->rl_lastbyte = b;
            if(lgt >= RL_MAXLENGTH) {
              after_flate_add(o, RL_RUNLENGTH(lgt));
              after_flate_add(o, b);
              o->rl_bufused = 0;
              o->rl_state = 0;
              o->rl_lastbyte = b;
            }
          } break;
          case 1: {
            buffer[lgt++] = b;
            o->rl_bufused = lgt;
            o->rl_state = 2;
            o->rl_lastbyte = b;
            lgt = lgt - 3;
            if(lgt > 0) {
              after_flate_add(o, RL_STRINGLENGTH(lgt));
              for(i = 0; i < lgt; i++) {
                after_flate_add(o, buffer[i]);
              }
              buffer[0] = buffer[1] = buffer[2] = b;
              o->rl_bufused = 3;
              o->rl_state = 2;
              o->rl_lastbyte = b;
            }
          } break;
          default: {
            buffer[lgt++] = b;
            o->rl_bufused = lgt;
            o->rl_state = 1;
            o->rl_lastbyte = b;
            if(lgt >= RL_MAXLENGTH) {
              lgt = lgt - 2;
              after_flate_add(o, RL_STRINGLENGTH(lgt));
              for(i = 0; i < lgt; i++) {
                after_flate_add(o, buffer[i]);
              }
              buffer[0] = buffer[1] = b;
              o->rl_bufused = 2;
              o->rl_state = 1;
              o->rl_lastbyte = b;
            }
          } break;
        }
      } else {
        if(o->rl_state == 2) {
          after_flate_add(o, RL_RUNLENGTH(lgt));
          after_flate_add(o, (o->rl_lastbyte));
          buffer[0] = b; o->rl_bufused = 1; o->rl_lastbyte = b;
          o->rl_state = 0;
        } else {
          buffer[lgt++] = b;
          o->rl_bufused = lgt;
          o->rl_lastbyte = b;
          if(lgt >= RL_MAXLENGTH) {
            after_flate_add(o, RL_STRINGLENGTH(lgt));
            for(i = 0; i < lgt; i++) {
              after_flate_add(o, buffer[i]);
            }
            o->rl_bufused = 0;
          }
          o->rl_state = 0;
        }
      }
    } else {
      buffer[0] = b;
      o->rl_bufused = 1;
      o->rl_lastbyte = b;
    }
    o->rl_lastbyte = b;

  } else {
    after_flate_add(o,0);
    after_flate_add(o,b);
  }

}

static void rl_flush(Output_Encoder *o)
{
  int lgt;
  int *buffer;
  int i;

  buffer = o->rl_buffer;
  lgt = o->rl_bufused;
  if(lgt > 0) {
    if(o->rl_state == 2) {
      i = o->rl_lastbyte;
      after_flate_add(o,RL_RUNLENGTH(lgt));
      after_flate_add(o,i);
    } else {
      after_flate_add(o,RL_STRINGLENGTH(lgt));
      for(i = 0; i < lgt; i++) {
        after_flate_add(o,buffer[i]);
      }
    }
  }
  after_flate_flush(o);
}

static void internal_byte_add(Output_Encoder *o, int b)
{

  if((o->mode) & OE_RL) {
    rl_add(o,b);
  } else {
    after_flate_add(o,b);
  }

}

static void internal_byte_flush(Output_Encoder *o)
{

  if((o->mode) & OE_RL) {
    rl_flush(o);
  } else {
    after_flate_flush(o);
  }

}

void oe_bit_add(Output_Encoder *o, int b)
{

  o->bit_value = 2 * o->bit_value + (b ? 1 : 0);
  o->bit_consumed = o->bit_consumed + 1;
  if(o->bit_consumed >= 8) {
    o->bit_consumed = 0;
    internal_byte_add(o, (o->bit_value));
    o->bit_value = 0;
  }

}

void oe_bit_flush(Output_Encoder *o)
{

  if(o->bit_consumed) {
    int v, i;
    v = o->bit_value;
    i = o->bit_consumed;
    while(i < 8) {
      i++;
      v = v * 2;
    }
    internal_byte_add(o,v);
    o->bit_value = 0;
    o->bit_consumed = 0;
  }
  internal_byte_flush(o);

}

void oe_byte_add(Output_Encoder *o, int b)
{

  if(o->bit_consumed) {
    int testval,i;
    testval = 128;
    for(i = 0; i < 8; i++) {
      if(b & testval) {
        oe_bit_add(o,1);
      } else {
        oe_bit_add(o,0);
      }
      testval = testval / 2;
    }
  } else {
    internal_byte_add(o,b);
  }

}

void oe_byte_flush(Output_Encoder *o)
{
  oe_bit_flush(o);
}


#ifdef OE_TEST_MAIN

int main(int argc, char *argv[])
{
  Output_Encoder o;
  int methods; int number;
  int rlbuffer[129];
  char buffer[512];

  methods = 0;
  if(argc > 1) {
    char *ptr;
    ptr = argv[1];
    while(*ptr) {
      switch(*ptr) {
        case 'a' : {
          methods |= OE_ASC85;
        } break;
        case 'r' : {
          methods |= OE_RL;
        } break;
      }
      ptr++;
    }
  }
  oe_init(&o, stdout, methods, 9, rlbuffer);
  methods = 1;
  while(methods) {
    number = read(0, buffer, sizeof(buffer));
    if(number > 0) {
      char c, *ptr;
      int  i;
      ptr = buffer;
      for(i = 0; i < number; i++) {
        c = *ptr;
        oe_byte_add(&o, c);
        ptr++;
      }
    } else { methods = 0; }
  }
  oe_byte_flush(&o);
  return 0;
}
#endif

