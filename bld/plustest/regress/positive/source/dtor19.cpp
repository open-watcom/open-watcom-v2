#include "fail.h"

struct CD {
    CD();
    ~CD();
};

int ctors;

CD::CD() {
    ++ctors;
}

CD::~CD() {
    --ctors;
}

int test_default( int x )
{
    CD object;

    switch( x ) {
    case -1 : return 1;
    case -2 : return 2;
    }
    return -x;
}

int test( int x )
{
    CD object;

    switch( x ) {
    case -3 : return 3;
    case -1 : return 1;
    }
    return -x;
}

int main()
{
    if( test( -3 ) != -( -3 ) ) fail(__LINE__);
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    if( test( -2 ) != -( -2 ) ) fail(__LINE__);
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    if( test( -1 ) != -( -1 ) ) fail(__LINE__);
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    if( test( 0 ) != -( 0 ) ) fail(__LINE__);
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    if( test_default( -3 ) != -( -3 ) ) fail(__LINE__);
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    if( test_default( -2 ) != -( -2 ) ) fail(__LINE__);
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    if( test_default( -1 ) != -( -1 ) ) fail(__LINE__);
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    if( test_default( 0 ) != -( 0 ) ) fail(__LINE__);
    if( ctors != 0 ) fail(__LINE__);
    ctors = 0;
    _PASS;
}
