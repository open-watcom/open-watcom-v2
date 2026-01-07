#include "fail.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int test(long long x)
{
    switch (x)
    {
      case  1:
      case  0:
      case -1: return 1;
      default: return 0;
    }
}

int u_test(unsigned long long x)
{
    switch (x)
    {
      case  1:
      case  0:
      case -1: return 1;
      default: return 0;
    }
}

int a_test(long long x)
{
    switch (x)
    {
      case  -2:
      case  -1:
      case 0: return 1;
      default: return 0;
    }
}

int b_test(long long x)
{
    switch (x)
    {
      case LLONG_MIN+1:
      case LLONG_MIN:
      case LLONG_MAX: return 1;
      default: return 0;
    }
}

int c_test(long long x)
{
    switch (x)
    {
      case LLONG_MAX-1:
      case LLONG_MAX:
      case LLONG_MIN: return 1;
      default: return 0;
    }
}

int d_test(long long x)
{
    switch (x)
    {
      case LLONG_MAX-1:
      case LLONG_MAX:
      case 2: return 1;
      default: return 0;
    }
}

int e_test(long long x)
{
    switch (x)
    {
      case LLONG_MIN:
      case LLONG_MIN+1:
      case LLONG_MAX: return 1;
      default: return 0;
    }
}

int f_test(long long x)
{
    switch (x)
    {
      case LLONG_MAX:
      case LLONG_MAX-1:
      case LLONG_MIN: return 1;
      default: return 0;
    }
}

int main(void)
{
    if( test(-1) != 1 ) fail( __LINE__ );
    if( test(0) != 1 ) fail( __LINE__ );
    if( test(1) != 1 ) fail( __LINE__ );
    if( u_test(-1) != 1 ) fail( __LINE__ );
    if( u_test(0) != 1 ) fail( __LINE__ );
    if( u_test(1) != 1 ) fail( __LINE__ );
    if( a_test(-2) != 1 ) fail( __LINE__ );
    if( a_test(-1) != 1 ) fail( __LINE__ );
    if( a_test(0) != 1 ) fail( __LINE__ );
    if( b_test(LLONG_MIN+1) != 1 ) fail( __LINE__ );
    if( b_test(LLONG_MIN) != 1 ) fail( __LINE__ );
    if( b_test(LLONG_MAX) != 1 ) fail( __LINE__ );
    if( c_test(LLONG_MAX-1) != 1 ) fail( __LINE__ );
    if( c_test(LLONG_MAX) != 1 ) fail( __LINE__ );
    if( c_test(LLONG_MIN) != 1 ) fail( __LINE__ );
    if( d_test(LLONG_MAX-1) != 1 ) fail( __LINE__ );
    if( d_test(LLONG_MAX) != 1 ) fail( __LINE__ );
    if( d_test(2) != 1 ) fail( __LINE__ );
    if( e_test(LLONG_MIN+1) != 1 ) fail( __LINE__ );
    if( e_test(LLONG_MIN) != 1 ) fail( __LINE__ );
    if( e_test(LLONG_MAX) != 1 ) fail( __LINE__ );
    if( f_test(LLONG_MAX-1) != 1 ) fail( __LINE__ );
    if( f_test(LLONG_MAX) != 1 ) fail( __LINE__ );
    if( f_test(LLONG_MIN) != 1 ) fail( __LINE__ );
    _PASS;
}
