#include "fail.h"
#include <stdio.h>
#include <stdlib.h>

int count;

struct S {
    int a;
    S(int x):a(x|1) {
	if( count != x ) {
	    fail( __LINE__ );
	}
	++count;
    };
    ~S() {
	if(( a & 1 ) == 0 || (( count - 1 ) | 1 ) != a ) {
	    /* a was not initialized */
	    fail( __LINE__ );
	}
	--count;
	if( count < 0 ) {
	    /* too many destructions */
	    fail( __LINE__ );
	}
    }
};

void sam()
{
    static S x0(0);
    static S x1(1);
    static S x2(2);
    static S x3(3);
    static S x4(4);
    static S x5(5);
    static S x6(6);
    static S x7(7);
    static S x8(8);

    if( x0.a != 1 ) fail( __LINE__ );
    if( x1.a != 1 ) fail( __LINE__ );
    if( x2.a != 3 ) fail( __LINE__ );
    if( x3.a != 3 ) fail( __LINE__ );
    if( x4.a != 5 ) fail( __LINE__ );
    if( x5.a != 5 ) fail( __LINE__ );
    if( x6.a != 7 ) fail( __LINE__ );
    if( x7.a != 7 ) fail( __LINE__ );
    if( x8.a != 9 ) fail( __LINE__ );
}

void foo()
{
    static S x0(18);
    static S x1(19);
    static S x2(20);
    static S x3(21);
    static S x4(22);
    static S x5(23);
    static S x6(24);
    static S x7(25);
    static S x8(26);

    fail( __LINE__ );
}

void bar()
{
    static S x0(9);
    static S x1(10);
    static S x2(11);
    static S x3(12);
    static S x4(13);
    static S x5(14);
    static S x6(15);
    static S x7(16);
    static S x8(17);

    if( x0.a != 9 ) fail( __LINE__ );
    if( x1.a != 11 ) fail( __LINE__ );
    if( x2.a != 11 ) fail( __LINE__ );
    if( x3.a != 13 ) fail( __LINE__ );
    if( x4.a != 13 ) fail( __LINE__ );
    if( x5.a != 15 ) fail( __LINE__ );
    if( x6.a != 15 ) fail( __LINE__ );
    if( x7.a != 17 ) fail( __LINE__ );
    if( x8.a != 17 ) fail( __LINE__ );
}

int main()
{
    sam();
    if( count != 9 ) fail( __LINE__ );
    sam();
    if( count != 9 ) fail( __LINE__ );
    bar();
    if( count != 18 ) fail( __LINE__ );
    bar();
    if( count != 18 ) fail( __LINE__ );
    _PASS;
}
