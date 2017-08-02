#include "fail.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned count;

struct D {
    D() { ++count; }
    ~D() { --count; }
};

struct Base {
    virtual int id() { return 31; }
};

struct IntErr : Base {
    IntErr( int value ) { x[23] = value; }
    virtual int id() { return x[23]; }
    int x[64];
};

struct StrErr : Base {
    StrErr( char *p ) { strcpy( x, p ); }
    virtual int id() { return x[1]; }
    char x[64];
};

void throws_int()
{
    D x;
    {
	D x;
	{
	    D x;
	    {
		D x;
		{
		    throw 6;
		}
	    }
	}
    }
}

void throws_IntErr()
{
    D x;
    {
	D x;
	{
	    D x;
	    {
		D x;
		{
		    throw IntErr(28);
		}
	    }
	}
    }
}

void throws_StrErr()
{
    D x;
    {
	D x;
	{
	    D x;
	    {
		D x;
		{
		    throw StrErr("except");
		}
	    }
	}
    }
}

void test1()
{
    try {
	throws_int();
    } catch( int x ) {
	if( x != 6 ) {
	    fail( __LINE__ );
	}
    } catch( ... ) {
	fail( __LINE__ );
    }
}

void test2()
{
    try {
	throws_IntErr();
    } catch( IntErr &r ) {
	if( r.id() != 28 ) {
	    fail( __LINE__ );
	}
    } catch( int x ) {
	fail( __LINE__ );
    } catch( ... ) {
	fail( __LINE__ );
    }
}

void test3()
{
    try {
	throws_StrErr();
    } catch( Base &r ) {
	if( r.id() != 'x' ) {
	    fail( __LINE__ );
	}
    } catch( int x ) {
	fail( __LINE__ );
    } catch( ... ) {
	fail( __LINE__ );
    }
}

void test4()
{
    try {
	throws_StrErr();
    } catch( IntErr &r ) {
	fail( __LINE__ );
    } catch( Base &r ) {
	if( r.id() != 'x' ) {
	    fail( __LINE__ );
	}
    } catch( int x ) {
	fail( __LINE__ );
    } catch( ... ) {
	fail( __LINE__ );
    }
}

void test6()
{
    try {
	throws_StrErr();
    } catch( IntErr &r ) {
	fail( __LINE__ );
    } catch( int x ) {
	fail( __LINE__ );
    } catch( ... ) {
    }
}

void test5()
{
    try {
	throws_StrErr();
    } catch( StrErr &r ) {
	if( r.id() != 'x' ) {
	    fail( __LINE__ );
	}
    } catch( int x ) {
	fail( __LINE__ );
    } catch( ... ) {
	fail( __LINE__ );
    }
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    if( count != 0 ) {
	fail( __LINE__ );
    }
    _PASS;
}
