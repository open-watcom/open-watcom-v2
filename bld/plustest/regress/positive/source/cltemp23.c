#include "fail.h"

/* part of bug 514 - compiler wasn't matching int and signed int */
template < class T >
struct X{
    static int const v = 1;
};

template <>
struct X< int >{
    static int const v = 2;
};

template <>
struct X< unsigned long >{
    static int const v = 3;
};

template <>
struct X< signed long >{
    static int const v = 4;
};

int main( void )
{
    if( X<char>::v != 1 ) fail( __LINE__ );
    if( X<int>::v != 2 ) fail( __LINE__ );
    if( X<signed int>::v != 2 ) fail( __LINE__ );
    if( X<unsigned int>::v != 1 ) fail( __LINE__ );
    if( X<unsigned long>::v != 3 ) fail( __LINE__ );
    if( X<signed long>::v != 4 ) fail( __LINE__ );
    if( X<long>::v != 4 ) fail( __LINE__ );
    
    _PASS
};




