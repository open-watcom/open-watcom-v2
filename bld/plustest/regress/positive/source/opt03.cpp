#include "fail.h"
#include <stdio.h>

int foo( int &x )
{
    return !&x;
}
int foo( int *x )
{
    int &r = *x;
    return &r == 0;
}
int foo( char *x )
{
    char &r = *x;
    return &r != 0;
}
struct S {
    int foo();
    int test_this();
    int use_this();
};
int S::foo()
{
    return this == 0;
}
int S::test_this()
{
    if( this ) {
	return 2;
    }
    return this != 0 ? 3 : 1;
}
int S::use_this()
{
    if( !this ) {
	return 1;
    }
    return this ? 4 : 5;
}

S *p;
int *q;
char *x;

int main()
{
    if( ! foo( q ) ) fail(__LINE__);
    if( ! foo( *q ) ) fail(__LINE__);
    if( foo( x ) ) fail(__LINE__);
    if( ! p->foo() ) fail(__LINE__);
    if( p->test_this() != 1 ) fail(__LINE__);
    if( p->use_this() != 1 ) fail(__LINE__);
    _PASS;
}
