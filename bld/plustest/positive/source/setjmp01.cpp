#include "fail.h"
#include <setjmp.h>

// SETJMP01 -- test basic longjmp support
//
// 94/01/19 -- J.W.Welch        -- defined


int ctored;
int dtored;
int jumped;

#define verify( s )	( puts( s ), fail(__LINE__) )

struct S
{
    S()     { ctored = 1; }
    ~S()    { dtored = 1; }
};


static void checkStatus()
{
    if( ! ctored ) verify( "missed ctoring" );
#ifndef __SW_XD 
    if( ! dtored ) verify( "missed dtoring" );
#endif
    if( ! jumped ) verify( "missed dtoring longjmp" );

    ctored = 0;
    dtored = 0;
    jumped = 0;
}

int main()
{
    jmp_buf buf;
    int sv;

    sv = setjmp( buf );
    if( sv == 0 ) {
        S s;
        longjmp( buf, 767 );
    } else if( sv == 767 ) {
        jumped = 1;
    } else {
        verify( "wrong setjmp return value" );
    }
    checkStatus();

    switch( setjmp( buf ) ) {
      case 0:
      {
          S s;
          longjmp( buf, 1 );
      }
      default :
        break;
      case 1:
        jumped = 1;
        break;
    }
    checkStatus();

    switch( setjmp( buf ) ) {
      case 0 :
        longjmp( buf, 31459 );
      case 31459 :
        jumped = 1;
        break;
    }
    if( ! jumped ) verify( "missed simple longjmp" );

    _PASS;
}
