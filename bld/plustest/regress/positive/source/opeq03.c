#include "fail.h"
// TEST op=

// #pragma on (cdopt)


struct A {
    int a;
//  A operator=( const A& ra ) { a = ra.a; return *this; }
    A( int va ) : a( va ) {}
};

struct B {
    int b;
//  B operator=( const B& rb ) { b = rb.b; return *this; }
    B( int vb ) : b( vb ) {}
};

struct C : A, B {
    int id;
    int c;
    C( int cv ) : A(cv), B(cv), id( cv ), c(cv ) {}
    void check( int ) const;
};


void C::check( int val ) const
{
    if( c != val ) {
        printf( "FAIL[%d] C::c(%d) != %d\n", id, c, val );
	fail(__LINE__);
    } else if( a != val ) {
        printf( "FAIL[%d] C::a(%d) != %d\n", id, a, val );
	fail(__LINE__);
    } else if( b != val ) {
        printf( "FAIL[%d] C::b(%d) != %d\n", id, b, val );
	fail(__LINE__);
    }
}

struct V {
    int assigned;
    int v;
    virtual V operator=( const V& rv ) { v = rv.v; ++assigned; return *this; }
    V( int vv ) : v( vv ), assigned( 0 ) {}
};

struct B1 : virtual V {
    int b1;
    B1( int bv ) : V( bv ), b1( bv ) {}
};

struct B2 : virtual V {
    int b2;
    B2( int bv ) : V( bv ), b2( bv ) {}
};

struct B12 : B1, B2 {
    int b12;
    B12( int bv ) : V( bv ), B1( bv ), B2( bv ), b12( bv ) {}
};

struct D : B12 {
    int id;
    int d;
    D( int dv ) : V( dv ), B12( dv ), id( dv ), d( dv ) {};
    void check( int, int ) const;
};

struct E : virtual A, D {
    int e;
    E( int ev ) : A( ev ), V( ev ), D( ev ), e( ev ) {}
};

void D::check( int val, int ass ) const
{
    if( v != val ) {
        printf( "FAIL[%d] D::v(%d) != %d\n", id, v, val );
	fail(__LINE__);
    } else if( d != val ) {
        printf( "FAIL[%d] D::d(%d) != %d\n", id, d, val );
	fail(__LINE__);
    } else if( b12 != val ) {
        printf( "FAIL[%d] D::b12(%d) != %d\n", id, b12, val );
	fail(__LINE__);
    } else if( b1 != val ) {
        printf( "FAIL[%d] D::b1(%d) != %d\n", id, b1, val );
	fail(__LINE__);
    } else if( b2 != val ) {
        printf( "FAIL[%d] D::b2(%d) != %d\n", id, b2, val );
	fail(__LINE__);
    } else if( ass != assigned ) {
        printf( "FAIL[%d] D::assigned(%d) != %d\n", id, assigned, ass );
	fail(__LINE__);
    }
}


int main()
{
    C c1( 1 );
    C c2( 2 );

    c1.check( 1 );
    c2.check( 2 );

    c1 = c2;
    c1.check( 2 );

    D d1( 3 );
    D d2( 4 );
    
    d1.check( 3, 0 );
    d2.check( 4, 0 );

    d1 = d2;
    d1.check( 4, 1 );

    E e1( 5 );
    e1.check( 5, 0 );

    d2 = e1;
    d2.check( 5, 1 );
    

    _PASS;
}
// #pragma on ( dump_exec_ic )
