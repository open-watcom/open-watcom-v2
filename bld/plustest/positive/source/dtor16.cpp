// test proper destruction with goto's

#include "fail.h"

int ctored;
int dtored;

struct A { A(); ~A(); };

A::A() { ++ctored; }
A::~A() { ++dtored; }

void errmsg( const char* msg )
{
    printf( "DTOR16:%s\n", msg );
    ++errors;
}

void check( int exp_ctored, int exp_dtored, const char* msg )
{
    if( exp_ctored != ctored
     || exp_dtored != dtored ) {
        errmsg( msg );
    }
}

// states:
//  0 - start
//  1 - first time thru lab_0
//  2 - branch back from inner block
//  3 - second time into inner block
//  4 - third time into inner block
int main()
{
    int state = 0;
    check( 0, 0, "improper initialization" );
    A a1;
 lab_0:
    ++ state;
    switch( state ) {
      case 1:
        check( 1, 0, "state[1]: after init of a1" );
        goto lab_0;
      case 2:
        check( 1, 0, "state[2]: after init of a1" );
        break;
      case 3:
        check( 3, 2, "state[3]: after init of a1" );
        break;
      case 4:
        check( 6, 5, "state[4]: after init of a1" );
        break;
      default:
        errmsg( "bad state after lab_0" );
        break;
    }
    A a2;
    {
        A a3;
        switch( state ) {
          case 2:
            check( 3, 0, "state[2]: after init of a3" );
            goto lab_0;
          case 3:
          { A a4;
            check( 6, 2, "state[3]: after init of a4" );
            goto lab_0;
          }
          case 4:
          { A a5;
            check( 9, 5, "state[4]: after init of a5" );
            goto lab_1;
          }
          default :
            errmsg( "bad state in first inner block" );
            break;
        }
    }
 lab_1:
    check( 9, 7, "after lab_1" );
    if( ctored ) {
        A a6;
        check( 10, 7, "after init of a6" );
        goto lab_2;
    } else {
      lab_2:
        check( 10, 8, "after lab_2" );
    }
    _PASS;
}
