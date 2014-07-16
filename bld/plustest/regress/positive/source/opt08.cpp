#include "fail.h"
#include <float.h>

int is_one( double d )
{
    d -= 1.0;
    if( d < 0 ) {
	d = -d;
    }
    if( d < 2 * DBL_EPSILON ) {
	return 1;
    }
    return 0;
}

double c,d;

void f00800000() {
    c = d / (double)0x00800000;
}
void f01000000() {
    c = d / (double)0x01000000;
}
void f02000000() {
    c = d / (double)0x02000000;
}
void f04000000() {
    c = d / (double)0x04000000;
}
void f08000000() {
    c = d / (double)0x08000000;
}
void f10000000() {
    c = d / (double)0x10000000;
}
void f20000000() {
    c = d / (double)0x20000000;
}
void f40000000() {
    c = d / (double)0x40000000;
}
void f80000000() {
    c = d / (double)0x80000000;
}

int main() {
    d = 0x00800000;
    f00800000();
    if( is_one( c ) == 0 ) fail(__LINE__);
    d = 0x01000000;
    f01000000();
    if( is_one( c ) == 0 ) fail(__LINE__);
    d = 0x02000000;
    f02000000();
    if( is_one( c ) == 0 ) fail(__LINE__);
    d = 0x04000000;
    f04000000();
    if( is_one( c ) == 0 ) fail(__LINE__);
    d = 0x08000000;
    f08000000();
    if( is_one( c ) == 0 ) fail(__LINE__);
    d = 0x10000000;
    f10000000();
    if( is_one( c ) == 0 ) fail(__LINE__);
    d = 0x20000000;
    f20000000();
    if( is_one( c ) == 0 ) fail(__LINE__);
    d = 0x40000000;
    f40000000();
    if( is_one( c ) == 0 ) fail(__LINE__);
    d = 0x80000000;
    f80000000();
    if( is_one( c ) == 0 ) fail(__LINE__);
    _PASS;
}
