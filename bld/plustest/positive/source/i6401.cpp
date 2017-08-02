// test for bit fields

#include "fail.h"

#ifndef __WATCOM_INT64__
typedef long __int64;
#endif

struct BF {
    __int64 f1 : 20;
    __int64 f2 : 20;
    __int64 f3 : 20;
    __int64 f4 :  4;
};

BF bf;

int main()
{
    bf.f1 = 10;
    bf.f2 = 20;
    bf.f3 = 30;
    bf.f4 = 4;
    if( 10 != bf.f1 ) fail( __LINE__ );		// problem is here
    if( 20 != bf.f2 ) fail( __LINE__ );
    if( 30 != bf.f3 ) fail( __LINE__ );
    if( 4  != bf.f4 ) fail( __LINE__ );
    _PASS;
}
