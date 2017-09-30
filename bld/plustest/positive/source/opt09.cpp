#include "fail.h"

#if defined( _M_IX86 )

#include <i86.h>
#include <stdio.h>
#include <string.h>

typedef unsigned short ushort;
#define ROWS 25
#define COLS 80

static short     minchng [ ROWS ];
static short     maxchng [ ROWS ];

struct  Screen {
    
    short            row;
    short            col;
    
    void Paint();
};

void check( ushort __far *p, unsigned line )
{
    if( FP_SEG( p ) != 0xb800 ) fail( line );
    if( FP_OFF( p ) > (26*80) ) fail( line );
}

void  Screen::Paint()
{
#ifndef __386__
   ushort __far   *ptr = ( ushort __far * ) 0xB8000000L;
#else
   ushort __far   *ptr = (ushort __far *) MK_FP( 0xb800, 0 );
#endif
   ushort __far   *calc;

   for ( int  r = 0 ; r < row ; r++ ) {
      for ( int  c = minchng [ r ] ; c <= maxchng [ r ] ; c++ ) {
          calc = ( ptr + r * col + c );
          check( calc, __LINE__ );
      }
   }
}

int main() {
    Screen s;

    s.row = 10;
    s.col = 80;
    for( int i = 0; i < s.row; ++i ) {
        minchng[i] = 0;
        maxchng[i] = s.col/2;
    }
    s.Paint();
    _PASS;
}


#else

ALWAYS_PASS

#endif
