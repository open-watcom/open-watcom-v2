#include "dump.h"

//Integral Promotions 
// This file should compile without error.
//#pragma on ( dump_rank );

// from char		to int
void fci( ... ) BAD;
void fci( int ) GOOD;
void gci( char a )
{
    fci( a );
}

// from signed char	to int
void fsci( ... ) BAD;
void fsci( int ) GOOD;
void gsci( signed char a )
{
    fsci( a );
}

// from wide char	to unsigned int
void flci( ... ) BAD;
void flci( unsigned int ) GOOD;
void glci( long char a )
{
    flci( a );
}

// from unsigned char	to int
void fuci( ... ) BAD;
void fuci( int ) GOOD;
void guci( unsigned char a )
{
    fuci( a );
}

// from signed short to int
void fssi( ... ) BAD;
void fssi( int ) GOOD;
void gssi( signed short a )
{
    fssi( a );
}

// from unsigned short to unsigned int
void fusui( ... ) BAD;
void fusui( unsigned int ) GOOD;
void gusui( unsigned short a )
{
    fusui( a );
}

// from int bit-field	to int
void fbii( ... ) BAD;
void fbii( int ) GOOD;
struct BITF {
    int	one	: 1;
    int	two	: 2;
    int	three	: 3;
};
void gbii( BITF a )
{
    fbii( a.one );
    fbii( a.two );
    fbii( a.three );
}

// from enum 		to result that depends on base-type used
void fei( ... ) BAD;
void fei( int ) GOOD;
void fei( unsigned int ) GOOD;
enum Euc    { auc =0, buc =255	    };	// unsigned char
enum Eus    { aus =0, bus =65535    };	// unsigned short
//enum Euli   { auli=0, buli=8388607L };	// unsigned long int
// this one will require a standard conversion
// there is not integral promotion to other than int
void gei( Euc a, Eus b )
{
    fei( a );
    fei( b );
}

// from float		to double
void ffd( ... ) BAD;
void ffd( double ) GOOD;
void gfd( float a )
{
    ffd( a );
}

// from float		to long double
void ffld( ... ) BAD;
void ffld( long double ) GOOD;
void gfld( float a )
{
    ffld( a );
}

// from double		to long double
void fdld( ... ) BAD;
void fdld( long double ) GOOD;
void gdld( double a )
{
    fdld( a );
}

int main( void ) {
    char gci_a;
    signed char gsci_a;
    long char glci_a;
    unsigned char guci_a;
    signed short gssi_a;
    unsigned short gusui_a;
    BITF gbii_a;
    Euc gei_a;
    Eus gei_b;
    float gfd_a;
    float gfld_a;
    double gdld_a;

    gci( gci_a );
    gsci( gsci_a );
    glci( glci_a );
    guci( guci_a );
    gssi( gssi_a );
    gusui( gusui_a );
    gbii( gbii_a );
    gei( gei_a, gei_b );
    gfd( gfd_a );
    gfld( gfld_a );
    gdld( gdld_a );
#ifdef _M_I86
    // 65535 is unsigned int, not int; hence slightly different result
    CHECK_GOOD( 775 );
#else
    CHECK_GOOD( 774 );
#endif
    return errors != 0;
}
