#include "fail.h"

template< typename T >
struct is_pointer {
    static const bool result = false;
};

template< typename T >
struct is_pointer< T * > {
    static const bool result = true;
};

template< typename T >
struct is_pointer< T * const > {
    static const bool result = true;
};


template< typename T >
struct is_reference {
    static const bool result = false;
};

template< typename T >
struct is_reference< T & > {
    static const bool result = true;
};


template< typename T >
struct is_const {
    static const bool result = false;
};

template< typename T >
struct is_const< const T > {
    static const bool result = true;
};


template< typename T >
struct is_volatile {
    static const bool result = false;
};

template< typename T >
struct is_volatile< volatile T > {
    static const bool result = true;
};


template< typename T1, typename T2 >
struct is_same_type {
    static const bool result = false;
};

template< typename T >
struct is_same_type< T, T > {
    static const bool result = true;
};


typedef void func();

int main()  {
    if( ! is_pointer< int * >::result ) fail( __LINE__ );
    if( ! is_pointer< const int * >::result ) fail( __LINE__ );
    if( ! is_pointer< const int * const >::result ) fail( __LINE__ );
    if( ! is_pointer< long * >::result ) fail( __LINE__ );
    if( ! is_pointer< func * >::result ) fail( __LINE__ );
    if( ! is_pointer< void (*)() >::result ) fail( __LINE__ );
    if( ! is_pointer< void * >::result ) fail( __LINE__ );
    if( is_pointer< int >::result ) fail( __LINE__ );
    if( is_pointer< const int >::result ) fail( __LINE__ );
    if( is_pointer< int & >::result ) fail( __LINE__ );
    if( is_pointer< const int & >::result ) fail( __LINE__ );
    if( is_pointer< func >::result ) fail( __LINE__ );
    if( is_pointer< func & >::result ) fail( __LINE__ );
    if( is_pointer< func * & >::result ) fail( __LINE__ );

    if( is_reference< int * >::result ) fail( __LINE__ );
    if( is_reference< const int * >::result ) fail( __LINE__ );
    if( is_reference< const int * const >::result ) fail( __LINE__ );
    if( is_reference< long * >::result ) fail( __LINE__ );
    if( is_reference< func * >::result ) fail( __LINE__ );
    if( is_reference< void (*)() >::result ) fail( __LINE__ );
    if( is_reference< void * >::result ) fail( __LINE__ );
    if( is_reference< int >::result ) fail( __LINE__ );
    if( is_reference< const int >::result ) fail( __LINE__ );
    if( ! is_reference< int & >::result ) fail( __LINE__ );
    if( ! is_reference< const int & >::result ) fail( __LINE__ );
    if( is_reference< func >::result ) fail( __LINE__ );
    if( ! is_reference< func & >::result ) fail( __LINE__ );
    if( ! is_reference< func * & >::result ) fail( __LINE__ );

    if( is_const< int * >::result ) fail( __LINE__ );
    if( is_const< const int * >::result ) fail( __LINE__ );
    if( is_const< const int * volatile >::result ) fail( __LINE__ );
    if( ! is_const< const int * const >::result ) fail( __LINE__ );
    if( ! is_const< const int * const volatile >::result ) fail( __LINE__ );
    if( is_const< void (*)() >::result ) fail( __LINE__ );
    if( is_const< void * >::result ) fail( __LINE__ );
    if( is_const< int >::result ) fail( __LINE__ );
    if( ! is_const< const int >::result ) fail( __LINE__ );
    if( ! is_const< const volatile int >::result ) fail( __LINE__ );
    if( is_const< volatile int >::result ) fail( __LINE__ );

    if( is_volatile< int * >::result ) fail( __LINE__ );
    if( is_volatile< volatile int * >::result ) fail( __LINE__ );
    if( is_volatile< volatile int * const >::result ) fail( __LINE__ );
    if( ! is_volatile< const int * volatile >::result ) fail( __LINE__ );
    if( ! is_volatile< const int * const volatile >::result ) fail( __LINE__ );
    if( is_volatile< int >::result ) fail( __LINE__ );
    if( is_volatile< const int >::result ) fail( __LINE__ );
    if( ! is_volatile< volatile int >::result ) fail( __LINE__ );
    if( ! is_volatile< const volatile int >::result ) fail( __LINE__ );

    if( is_same_type< char, unsigned char >::result ) fail( __LINE__ );
    if( is_same_type< char, signed char >::result ) fail( __LINE__ );
    if( ! is_same_type< char, char >::result ) fail( __LINE__ );
    if( is_same_type< char, const char >::result ) fail( __LINE__ );
    if( is_same_type< char *, const char * >::result ) fail( __LINE__ );
    if( ! is_same_type< func, func >::result ) fail( __LINE__ );
    if( ! is_same_type< func *, func * >::result ) fail( __LINE__ );
    if( ! is_same_type< func *, void (*)() >::result ) fail( __LINE__ );
    if( ! is_same_type< char[1], char[1] >::result ) fail( __LINE__ );
    if( is_same_type< char[1], char * >::result ) fail( __LINE__ );
    if( is_same_type< char[1], char * const >::result ) fail( __LINE__ );
    if( is_same_type< func, void (*)() >::result ) fail( __LINE__ );
    if( is_same_type< void (*)( int ), void (*)() >::result ) fail( __LINE__ );
    if( is_same_type< void (*)(), int (*)() >::result ) fail( __LINE__ );

    _PASS;
}
