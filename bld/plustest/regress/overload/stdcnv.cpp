#include "dump.h"

//Standard Conversions
// This file should compile without error.
//#pragma on ( dump_rank );

// from base		to signed base
//- base is one of { char, short, int, long int }
//  (long char does not have signed or unsigned)
void fcsb( ... ) BAD;
void fcsb( signed char ) GOOD;
void fsisb( ... ) BAD;
void fsisb( signed short int ) GOOD;
void fisb( ... ) BAD;
void fisb( signed int ) GOOD;
void flisb( ... ) BAD;
void flisb( signed long int ) GOOD;
void gbsb( void )
{
    char a; short int b; int c; long int d;
    fcsb( a );
    fsisb( b );
    fisb( c );
    flisb( d );
}

// from base		to unsigned base
//- base is one of { char, short, int, long int }
//  (long char does not have signed or unsigned)
void fcub( ... ) BAD;
void fcub( unsigned char ) GOOD;
void fsiub2( ... ) BAD;
void fsiub2( unsigned short int ) GOOD;
void fiub( ... ) BAD;
void fiub( unsigned int ) GOOD;
void fliub( ... ) BAD;
void fliub( unsigned long int ) GOOD;
void gbub( void )
{
    char a; short int b; int c; long int d;
    fcub( a );
    fsiub2( b );
    fiub( c );
    fliub( d );
}

// from signed base	to base
//- base is one of { char, short, int, long int }
//  (long char does not have signed or unsigned)
void fscb( ... ) BAD;
void fscb( char ) GOOD;
void fssib( ... ) BAD;
void fssib( short int ) GOOD;
void fsib( ... ) BAD;
void fsib( int ) GOOD;
void fslib( ... ) BAD;
void fslib( long int ) GOOD;
void gsbb( void )
{
    signed char a; signed short int b; signed int c; signed long int d;
    fscb( a );
    fssib( b );
    fsib( c );
    fslib( d );
}

// from signed base	to unsigned base
//- base is one of { char, short, int, long int }
//  (long char does not have signed or unsigned)
void fscub( ... ) BAD;
void fscub( unsigned char ) GOOD;
void fssiub( ... ) BAD;
void fssiub( unsigned short int ) GOOD;
void fsiub1( ... ) BAD;
void fsiub1( unsigned int ) GOOD;
void fsliub( ... ) BAD;
void fsliub( unsigned long int ) GOOD;
void gsbub( void )
{
    signed char a; signed short int b; signed int c; signed long int d;
    fscub( a );
    fssiub( b );
    fsiub1( c );
    fsliub( d );
}

// from unsigned base	to base
//- base is one of { char, short, int, long int }
//  (long char does not have signed or unsigned)
void fucb( ... ) BAD;
void fucb( char ) GOOD;
void fusib( ... ) BAD;
void fusib( short int ) GOOD;
void fuib( ... ) BAD;
void fuib( int ) GOOD;
void fulib( ... ) BAD;
void fulib( long int ) GOOD;
void gucb( void )
{
unsigned char a; unsigned short int b; unsigned int c; unsigned long int d;
    fucb( a );
    fusib( b );
    fuib( c );
    fulib( d );
}

// from unsigned base	to signed base
//- base is one of { char, short, int, long int }
//  (long char does not have signed or unsigned)
void fucsb( ... ) BAD;
void fucsb( signed char ) GOOD;
void fusisb( ... ) BAD;
void fusisb( signed short int ) GOOD;
void fuisb( ... ) BAD;
void fuisb( signed int ) GOOD;
void fulisb( ... ) BAD;
void fulisb( signed long int ) GOOD;
void gubsb( void )
{
unsigned char a; unsigned short int b; unsigned int c; unsigned long int d;
    fucsb( a );
    fusisb( b );
    fuisb( c );
    fulisb( d );
}

// from floating	to integral
void fc( ... ) BAD;
void fc( char ) GOOD;
void fsc( ... ) BAD;
void fsc( signed char ) GOOD;
void fuc( ... ) BAD;
void fuc( unsigned char ) GOOD;
void flc( ... ) BAD;
void flc( long char ) GOOD;
void fusi( ... ) BAD;
void fusi( unsigned short int ) GOOD;
void fssi( ... ) BAD;
void fssi( signed short int ) GOOD;
void fsi( ... ) BAD;
void fsi( signed int ) GOOD;
void fui( ... ) BAD;
void fui( unsigned int ) GOOD;
void fsli( ... ) BAD;
void fsli( signed long int ) GOOD;
void fuli( ... ) BAD;
void fuli( unsigned long int ) GOOD;

void gfi( void )
{
    float a; double b; long double c;
    fc( a );
    fc( b );
    fc( c );
    fsc( a );
    fsc( b );
    fsc( c );
    fuc( a );
    fuc( b );
    fuc( c );
    flc( a );
    flc( b );
    flc( c );
    fusi( a );
    fusi( b );
    fusi( c );
    fssi( a );
    fssi( b );
    fssi( c );
    fsi( a );
    fsi( b );
    fsi( c );
    fui( a );
    fui( b );
    fui( c );
    fsli( a );
    fsli( b );
    fsli( c );
    fuli( a );
    fuli( b );
    fuli( c );
};

// from integral	to floating
void ff( ... ) BAD;
void ff( float ) GOOD;
void fd( ... ) BAD;
void fd( float ) GOOD;
void fld( ... ) BAD;
void fld( float ) GOOD;
enum Eif { aEif, bEif };
void gif( void )
{
	char			a;
	signed char		b;
	unsigned char		c;
	long char		d;
	unsigned short int	e;
	signed short int	f;
	signed int		g;
	unsigned int		h;
	signed long int		i;
	unsigned long int	j;
	Eif			k;

    ff( a );
    ff( b );
    ff( c );
    ff( d );
    ff( e );
    ff( f );
    ff( g );
    ff( h );
    ff( i );
    ff( j );
    ff( k );
    fd( a );
    fd( b );
    fd( c );
    fd( d );
    fd( e );
    fd( f );
    fd( g );
    fd( h );
    fd( i );
    fd( j );
    fd( k );
    fld( a );
    fld( b );
    fld( c );
    fld( d );
    fld( e );
    fld( f );
    fld( g );
    fld( h );
    fld( i );
    fld( j );
    fld( k );
}

// from non-const, non-volatile T*	to void*
// from (T*)()				to void*
void fncvpv( ... ) BAD;
void fncvpv( void * ) GOOD;
void gncvpv( void )
{
    int *a;
    int (*b)( int );
    fncvpv( a );
    fncvpv( b );
}

// from constant 0	to T* (null pointer)
void fi( ... ) BAD;
void fi( int * ) GOOD;
void fX( ... ) BAD;
class X {};
void fX( X * ) GOOD;
void gcznp( void )
{
    fi( 0 );
    fX( 0 );
}

// from B*		to A* (if conversion is unambiguous and
//			    A is accessible to B) (null goes to null) (p36)
struct A { };
struct B : public A { };
void fab( ... ) BAD;
void fab( A * ) GOOD;
void gab( void )
{
    B *x;
    fab( x );
    fab( 0 );
}

// from A*		to B* (if conversion is unambiguous and
//			    A is accessible to B) (p38)
void fba( ... ) GOOD;
void fba( B * ) GOOD;
void gba( void )
{
    A *x;
    fba( x );
    fba( 0 );
}

// from constant 0	to M* (null pointer to member)
class C {
    public:
	int ci;
};
//void fmpc( ... ) BAD;
//void fmpc( int C::* ) GOOD;
//void gmpc( void )
//{
//    fmpc( 0 );
//}

// from MA*		to MB* (if inverse pointer to object conversion can
//			      be performed) (p39)
class MA {
    public:
	int mai;
};
class MB : public MA {
    public:
	int mbi;
};
//void fmamb( ... ) BAD;
//void fmamb( int B:: * ) GOOD;
//void gmamb( void )
//{
//    int A::* pma;
//    fmamb( pma );
//}
int main( void ) {
    gbsb( );
    gbub( );
    gsbb( );
    gsbub( );
    gucb( );
    gubsb( );
    gfi( );
    gif( );
    gncvpv( );
    gcznp( );
    gab( );
    gba( );
//    gmpc( );
//    gmamb( );
    CHECK_GOOD( 13982 );
    return errors != 0;
}
