#include "fail.h"
#include <stdio.h>
#pragma warning 433 9
typedef struct _123456789012345678901234567890_1 {
} X1;
typedef struct _123456789012345678901234567890_2 {
} X2;
typedef struct _123456789012345678901234567890_3 {
} X3;
typedef struct _123456789012345678901234567890_4 {
} X4;
typedef struct _123456789012345678901234567890_5 {
} X5;
typedef struct _123456789012345678901234567890_6 {
} X6;
typedef struct _123456789012345678901234567890_7 {
} X7;
typedef struct _123456789012345678901234567890_8 {
} X8;
typedef struct _123456789012345678901234567890_9 {
} X9;
typedef struct _123456789012345678901234567890_a {
} Xa;
typedef struct _123456789012345678901234567890_b {
} Xb;

int ok;

void foo( X1*,X2*,X3*,X4*,X5*,X6*,X7*,X8*,X9*,Xa*,Xb*,
	    X1*,X2*,X3*,X4*,X5*,X6*,X7*,X8*,X9*,Xa*,Xb* )
{
    ok = 1;
}

template <
void (*__x)( X1*,X2*,X3*,X4*,X5*,X6*,X7*,X8*,X9*,Xa*,Xb*,
	    X1*,X2*,X3*,X4*,X5*,X6*,X7*,X8*,X9*,Xa*,Xb* ) >
    struct S {
    };

S<foo> xx;

int main()
{
    foo( 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 );
    if( !ok ) fail(__LINE__);
    _PASS;
}
