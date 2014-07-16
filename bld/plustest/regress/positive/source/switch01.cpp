#include "fail.h"
#include <stdio.h> 
#include <stdlib.h>
#include <limits.h>

int test(int x) 
{ 
    switch (x) 
    { 
      case  1: 
      case  0: 
      case -1: return 1; 
      default: return 0; 
    } 
} 
 
int u_test(unsigned x)
{ 
    switch (x) 
    { 
      case  1: 
      case  0: 
      case -1: return 1; 
      default: return 0; 
    } 
} 
 
int a_test(int x)
{ 
    switch (x) 
    { 
      case  -2:
      case  -1:
      case 0: return 1; 
      default: return 0; 
    } 
} 
 
int b_test(int x)
{ 
    switch (x) 
    { 
      case INT_MIN+1:
      case INT_MIN:
      case INT_MAX: return 1;
      default: return 0; 
    } 
} 
 
int c_test(int x)
{ 
    switch (x) 
    { 
      case INT_MAX-1:
      case INT_MAX:
      case INT_MIN: return 1;
      default: return 0; 
    } 
} 
 
int d_test(int x)
{ 
    switch (x) 
    { 
      case INT_MAX-1:
      case INT_MAX:
      case 2: return 1;
      default: return 0; 
    } 
} 
 
int e_test(int x)
{ 
    switch (x) 
    { 
      case INT_MIN:
      case INT_MIN+1:
      case INT_MAX: return 1;
      default: return 0; 
    } 
} 
 
int f_test(int x)
{ 
    switch (x) 
    { 
      case INT_MAX:
      case INT_MAX-1:
      case INT_MIN: return 1;
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
    if( b_test(INT_MIN+1) != 1 ) fail( __LINE__ );
    if( b_test(INT_MIN) != 1 ) fail( __LINE__ );
    if( b_test(INT_MAX) != 1 ) fail( __LINE__ );
    if( c_test(INT_MAX-1) != 1 ) fail( __LINE__ );
    if( c_test(INT_MAX) != 1 ) fail( __LINE__ );
    if( c_test(INT_MIN) != 1 ) fail( __LINE__ );
    if( d_test(INT_MAX-1) != 1 ) fail( __LINE__ );
    if( d_test(INT_MAX) != 1 ) fail( __LINE__ );
    if( d_test(2) != 1 ) fail( __LINE__ );
    if( e_test(INT_MIN+1) != 1 ) fail( __LINE__ );
    if( e_test(INT_MIN) != 1 ) fail( __LINE__ );
    if( e_test(INT_MAX) != 1 ) fail( __LINE__ );
    if( f_test(INT_MAX-1) != 1 ) fail( __LINE__ );
    if( f_test(INT_MAX) != 1 ) fail( __LINE__ );
    if( f_test(INT_MIN) != 1 ) fail( __LINE__ );
    _PASS;
} 
