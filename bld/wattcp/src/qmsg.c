/*
 * debugging messages... can be used inside interrupts
 */

#include <stdio.h>
#include <dos.h>
#include "wattcp.h"

/*
 * Disable stack-checking here cause these functions are handy
 * in interrupt handlers etc.
 */
#if defined(__HIGHC__) || defined(__WATCOMC__)
#pragma off(check_stack)
#endif

#if (defined(__TURBOC__) || defined(__BORLANDC__)) && !defined(OLD_TURBOC)
#pragma option -N-
#endif

WORD dbg_color = 0x700;

void dputch (char x)
{
  static WORD xy = 0;
  if (x == '\n')
      xy = 0;
  else
  {
    pokew (0xB800,xy,(x & 0xFF) | dbg_color);
    xy += 2;
  }
}

void dmsg (char *s)
{
  dputch ('\n');
  while (*s)
    dputch (*s++);
}

void dhex1int (int x)
{
  x &= 0x0F;
  if (x > 9)
       x = 'A' + x - 0x0A;
  else x += '0';
  dputch ((char)x);
}

void dhex2int (int x)
{
  dhex1int (x >> 4);
  dhex1int (x);
  dputch (' ');
}

void dhex4int (int x)
{
  dhex2int (x >> 8);
  dhex2int (x);
}

