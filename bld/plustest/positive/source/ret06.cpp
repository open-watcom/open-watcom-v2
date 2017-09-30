// TEST DIFFERENT STRUCTURE RETURNS

#include <stdarg.h>
#include "fail.h"

#if defined( _M_IX86 )

#pragma aux REVERSED parm reverse;
#define __reversed __pragma( "REVERSED" )

#else

#define __reversed

#endif


struct Base {                   // a PODS
    int value;
};

struct Derived : Base {         // a non-PODS
    Derived( int );
    Derived( const Derived & );
    ~Derived();
    static int count;
};

int Derived::count = 0;

Derived::Derived( int v ) {
    value = v;
    ++count;
}

Derived::Derived( const Derived& src )
    : Base( (const Base&)src )
{
    ++count;
}

Derived::~Derived()
{
    --count;
}

const int val_base_struct = 769;
const int val_derived_struct = 5969;
const int val_ellipsis = 1473;

const int inc_ret_base_normal = 1;
const int inc_ret_base_reversed = 3;
const int inc_ret_base_pascal = 5;

const int inc_ret_derived_normal = 7;
const int inc_ret_derived_reversed = 11;
const int inc_ret_derived_pascal = 13;

const int inc_ret_extern_c = 17;

void checkBase( const Base& chk, int expected ) {
    if( expected != chk.value ) {
        printf( "RET06 -- bad value(%d) expecting(%d) this(%x)\n"
              , chk.value
              , expected
              , &chk
              );
        ++ errors;
    }
}


void checkDerived( const Base& chk, int expected ) {
    checkBase( chk, expected );
}


Base ret_base_normal( const Base& src ) {
    Base tgt = { src.value + inc_ret_base_normal };
    return tgt;
}


Base __reversed ret_base_reversed( const Base& src ) {
    Base tgt = { src.value + inc_ret_base_reversed };
    return tgt;
}


Base __pascal ret_base_pascal( const Base& src ) {
    Base tgt = { src.value + inc_ret_base_pascal };
    return tgt;
}


Base ret_base_normal_ell( const Base src, ... ) {
    va_list args;
    va_start( args, src );
    int added = va_arg( args, __typeof( added ) );
    va_end( args );
    Base tgt = { src.value + inc_ret_base_normal + added };
    return tgt;
}


Base __reversed ret_base_reversed_ell( const Base src, ... ) {
    va_list args;
    va_start( args, src );
    int added = va_arg( args, __typeof( added ) );
    va_end( args );
    Base tgt = { src.value + inc_ret_base_reversed + added };
    return tgt;
}


Base __pascal ret_base_pascal_ell( const Base src, ... ) {
    va_list args;
    va_start( args, src );
    int added = va_arg( args, __typeof( added ) );
    va_end( args );
    Base tgt = { src.value + inc_ret_base_pascal + added };
    return tgt;
}


Derived ret_derived_normal( const Derived& src ) {
    Derived tgt = { src.value + inc_ret_derived_normal };
    return tgt;
}


Derived __reversed ret_derived_reversed( const Derived& src ) {
    Derived tgt = { src.value + inc_ret_derived_reversed };
    return tgt;
}


Derived __pascal ret_derived_pascal( const Derived& src ) {
    Derived tgt = { src.value + inc_ret_derived_pascal };
    return tgt;
}


extern "C" Base ret_c_base_normal( const Base& src ) {
    Base tgt = { src.value + inc_ret_base_normal + inc_ret_extern_c };
    return tgt;
}


extern "C" Base __reversed ret_c_base_reversed( const Base& src ) {
    Base tgt = { src.value + inc_ret_base_reversed + inc_ret_extern_c };
    return tgt;
}


extern "C" Base __pascal ret_c_base_pascal( const Base& src ) {
    Base tgt = { src.value + inc_ret_base_pascal + inc_ret_extern_c };
    return tgt;
}


extern "C" Base ret_c_base_normal_ell( const Base src, ... ) {
    va_list args;
    va_start( args, src );
    int added = va_arg( args, __typeof( added ) );
    va_end( args );
    Base tgt = { src.value + inc_ret_base_normal + inc_ret_extern_c + added };
    return tgt;
}


extern "C" Base __reversed ret_c_base_reversed_ell( const Base src, ... ) {
    va_list args;
    va_start( args, src );
    int added = va_arg( args, __typeof( added ) );
    va_end( args );
    Base tgt = { src.value + inc_ret_base_reversed + inc_ret_extern_c + added };
    return tgt;
}


extern "C" Base __pascal ret_c_base_pascal_ell( const Base src, ... ) {
    va_list args;
    va_start( args, src );
    int added = va_arg( args, __typeof( added ) );
    va_end( args );
    Base tgt = { src.value + inc_ret_base_pascal + inc_ret_extern_c + added };
    return tgt;
}


extern "C" Derived ret_c_derived_normal( const Derived& src ) {
    Derived tgt = { src.value + inc_ret_derived_normal + inc_ret_extern_c };
    return tgt;
}


extern "C" Derived __reversed ret_c_derived_reversed( const Derived& src ) {
    Derived tgt = { src.value + inc_ret_derived_reversed + inc_ret_extern_c };
    return tgt;
}


extern "C" Derived __pascal ret_c_derived_pascal( const Derived& src ) {
    Derived tgt = { src.value + inc_ret_derived_pascal + inc_ret_extern_c };
    return tgt;
}


Base base_struct = { val_base_struct };
Derived derived_struct( val_derived_struct );


void test_regular()
{
    Base b0 = ret_base_normal( base_struct );
    checkBase( b0, val_base_struct + inc_ret_base_normal );

    Base b1 = ret_base_reversed( base_struct );
    checkBase( b1, val_base_struct + inc_ret_base_reversed );

    Base b2 = ret_base_pascal( base_struct );
    checkBase( b2, val_base_struct + inc_ret_base_pascal );

    Base b3 = ret_base_normal_ell( base_struct, val_ellipsis );
    checkBase( b3, val_base_struct + inc_ret_base_normal + val_ellipsis );

    Base b4 = ret_base_reversed_ell( base_struct, val_ellipsis );
    checkBase( b4, val_base_struct + inc_ret_base_reversed + val_ellipsis );

    Base b5 = ret_base_pascal_ell( base_struct, val_ellipsis );
    checkBase( b5, val_base_struct + inc_ret_base_pascal + val_ellipsis );

    Derived d0 = ret_derived_normal( derived_struct );
    checkDerived( d0, val_derived_struct + inc_ret_derived_normal );

    Derived d1 = ret_derived_reversed( derived_struct );
    checkDerived( d1, val_derived_struct + inc_ret_derived_reversed );

    Derived d2 = ret_derived_pascal( derived_struct );
    checkDerived( d2, val_derived_struct + inc_ret_derived_pascal );
}



void test_extern_c()
{
    Base b6 = ret_c_base_normal( base_struct );
    checkBase
        ( b6
        , val_base_struct + inc_ret_base_normal + inc_ret_extern_c );

    Base b7 = ret_c_base_reversed( base_struct );
    checkBase
        ( b7
        , val_base_struct + inc_ret_base_reversed + inc_ret_extern_c );

    Base b8 = ret_c_base_pascal( base_struct );
    checkBase
        ( b8
        , val_base_struct + inc_ret_base_pascal + inc_ret_extern_c );

    Base b9 = ret_c_base_normal_ell( base_struct, val_ellipsis );
    checkBase
        ( b9
        , val_base_struct + inc_ret_base_normal + val_ellipsis
        + inc_ret_extern_c );

    Base bA = ret_c_base_reversed_ell( base_struct, val_ellipsis );
    checkBase
        ( bA
        , val_base_struct + inc_ret_base_reversed + val_ellipsis
        + inc_ret_extern_c );

    Base bB = ret_c_base_pascal_ell( base_struct, val_ellipsis );
    checkBase
        ( bB
        , val_base_struct + inc_ret_base_pascal + val_ellipsis + inc_ret_extern_c );

    Derived d3 = ret_c_derived_normal( derived_struct );
    checkDerived
        ( d3
        , val_derived_struct + inc_ret_derived_normal + inc_ret_extern_c );

    Derived d4 = ret_c_derived_reversed( derived_struct );
    checkDerived
        ( d4
        , val_derived_struct + inc_ret_derived_reversed + inc_ret_extern_c );

    Derived d5 = ret_c_derived_pascal( derived_struct );
    checkDerived
        ( d5
        , val_derived_struct + inc_ret_derived_pascal + inc_ret_extern_c );
}


int main()
{
    int gbl_derived = Derived::count;
    
    test_regular();
    if( Derived::count != gbl_derived ) {
        printf( "RET06 FAILURE: Some Derived not DTORED\n" );
        ++errors;
    }
    test_extern_c();
    if( Derived::count != gbl_derived ) {
        printf( "RET06 FAILURE: Some Derived not DTORED\n" );
        ++errors;
    }

    _PASS;
}



// #pragma on ( dump_exec_ic )
