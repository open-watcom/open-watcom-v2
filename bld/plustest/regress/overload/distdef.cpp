#include "dump.h"

//Distinguishable Types:
// these are all valid distinguishable types, no errors should be generated.

// scalar integral types
void fit( char ) GOOD;
void fit( signed char ) GOOD;
void fit( unsigned char ) GOOD;
void fit( long char ) GOOD;
void fit( unsigned short ) GOOD;
void fit( signed short ) GOOD;
void fit( signed int ) GOOD;
void fit( unsigned int ) GOOD;
void fit( signed long int ) GOOD;
void fit( unsigned long int ) GOOD;
enum Eitc { ac=0, bc=127 };	// uses unsigned char
enum Eits { as=0, bs=32767 };	// uses unsigned short
enum Eitl { al=0, bl=8388607L };// uses unsigned long
void fit( Eitc ) GOOD;
void fit( Eits ) GOOD;
void fit( Eitl ) GOOD;
void git(
	char		    a,
	signed char	    b,
	unsigned char	    c,
	long char	    d,
	unsigned short	    e,
	signed short	    f,
	signed int	    g,
	unsigned int	    h,
	signed long int	    i,
	unsigned long int   j,
	Eitc		    k,
	Eits		    l,
	Eitl		    m )
{
    fit( a );
    fit( b );
    fit( c );
    fit( d );
    fit( e );
    fit( f );
    fit( g );
    fit( h );
    fit( i );
    fit( j );
    fit( k );
    fit( l );
    fit( m );
}

// scalar float types
void fft( float ) GOOD;
void fft( double ) GOOD;
void fft( long double ) GOOD;
void gft( float a, double b, long double c )
{
    fft( a );
    fft( b );
    fft( c );
}

// all scalar types
void fst( char ) GOOD;
void fst( signed char ) GOOD;
void fst( unsigned char ) GOOD;
void fst( long char ) GOOD;
void fst( unsigned short ) GOOD;
void fst( signed short ) GOOD;
void fst( signed int ) GOOD;
void fst( unsigned int ) GOOD;
void fst( signed long int ) GOOD;
void fst( unsigned long int ) GOOD;
enum Est { c, d };
void fst( Est ) GOOD;
void fst( float ) GOOD;
void fst( double ) GOOD;
void fst( long double ) GOOD;
void gst(
	char		    a,
	signed char	    b,
	unsigned char	    c,
	long char	    d,
	unsigned short	    e,
	signed short	    f,
	signed int	    g,
	unsigned int	    h,
	signed long int	    i,
	unsigned long int   j,
	Est		    k,
	float		    l,
	double		    m,
	long double	    n )
{
    fst( a );
    fst( b );
    fst( c );
    fst( d );
    fst( e );
    fst( f );
    fst( g );
    fst( h );
    fst( i );
    fst( j );
    fst( k );
    fst( l );
    fst( m );
    fst( n );
}

//- enumerations
enum Ee { e, f, g };
enum Fe { h, i, j };
void fe( Ee ) GOOD;
void fe( Fe ) GOOD;
void ge( Ee a, Fe b )
{
    fe( a );
    fe( b );
}

//- T&, const T&, volatile T&
void fr( int & ) GOOD;
void fr( const int & ) GOOD;
void fr( volatile int & ) GOOD;
void gr( int &a, const int &b, volatile int &c )
{
    fr( a );
    fr( b );
    fr( c );
}

//- T*, const T*, volatile T*
void fp( int * ) GOOD;
void fp( const int * ) GOOD;
void fp( volatile int * ) GOOD;
void gp( int *a, const int * b, volatile int * c )
{
    fp( a );
    fp( b );
    fp( c );
}

//- arrays by magnitude of second and subsequent dimensions
void fa( int [][5] ) GOOD;
void fa( int [][6] ) GOOD;
void fa( int [][6][7] ) GOOD;
void fa( int [][6][8] ) GOOD;
void fa( int *[7] ) GOOD;
void ga( int a[][5],
	 int b[][6],
	 int c[][6][7],
	 int d[][6][8],
	 int *e[7] )
{
    fa( a );
    fa( b );
    fa( c );
    fa( d );
    fa( e );
}

// extremes of array depth.....................................!!
void fae( int  [1][2][3][4][5][6][7][8][9][10][11][12][13][14][15] ) GOOD;
void fae( int  [1][2][3][4][5][6][7][8][9][10][11][12][13][14][16] ) GOOD;
void gae( int a[1][2][3][4][5][6][7][8][9][10][11][12][13][14][15],
          int b[1][2][3][4][5][6][7][8][9][10][11][12][13][14][16] )
{
    fae( a );
    fae( b );
}

	char		    git_a;
	signed char	    git_b;
	unsigned char	    git_c;
	long char	    git_d;
	unsigned short	    git_e;
	signed short	    git_f;
	signed int	    git_g;
	unsigned int	    git_h;
	signed long int	    git_i;
	unsigned long int   git_j;
	Eitc		    git_k;
	Eits		    git_l;
	Eitl		    git_m;
	float gft_a;
	double gft_b;
	long double gft_c;
	char		    gst_a;
	signed char	    gst_b;
	unsigned char	    gst_c;
	long char	    gst_d;
	unsigned short	    gst_e;
	signed short	    gst_f;
	signed int	    gst_g;
	unsigned int	    gst_h;
	signed long int	    gst_i;
	unsigned long int   gst_j;
	Est		    gst_k;
	float		    gst_l;
	double		    gst_m;
	long double	    gst_n;
	Ee ge_a;
	Fe ge_b;
	int &gr_a = gst_g;
	const int gr_b_ci = 0;
	const int &gr_b = gr_b_ci;
	volatile int gr_c_vi;
	volatile int &gr_c = gr_c_vi;
	int *gp_a;
	const int * gp_b = &gr_a;
	volatile int * gp_c;
	int ga_a[1][5];
	int ga_b[1][6];
	int ga_c[1][6][7];
	int ga_d[1][6][8];
	int *ga_e[7];
	int (*gae_a)[1][2][3][4][5][6][7][8][9][10][11][12][13][14][15];
	int (*gae_b)[1][2][3][4][5][6][7][8][9][10][11][12][13][14][16];

int main( void ) {
    git( git_a, git_b, git_c, git_d, git_e, git_f, git_g, git_h, git_i,
	git_j, git_k, git_l, git_m );
    gft( gft_a, gft_b, gft_c );
    gst( gst_a, gst_b, gst_c, gst_d, gst_e, gst_f, gst_g, gst_h, gst_i,
	gst_j, gst_k, gst_l, gst_m, gst_n );
    ge( ge_a, ge_b );
    gr( gr_a, gr_b, gr_c );
    gp( gp_a, gp_b, gp_c );
    ga( ga_a, ga_b, ga_c, ga_d, ga_e );
    gae( *gae_a, *gae_b );
    CHECK_GOOD( 3433 );
    return errors != 0;
}
