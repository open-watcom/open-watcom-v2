#include "fail.h"

#if defined( __OS2__ ) && defined( __386__ )

typedef int (__far16 *p16f_t)( void );
typedef int (*pf_t)( void );

/* Dummy function - just make sure function pointer conversions work */
int foo( void )
{
    return( 0 );
}

/* Make sure converts work in return statement, assignment, initialization */

void __far16 *cnv32to16_r( void *ptr )
{
    return( ptr );
}

void __far16 *cnv32to16_a( void *ptr )
{
    void __far16    *fptr;

    fptr = ptr;
    return( fptr );
}

void __far16 *cnv32to16_i( void *ptr )
{
    void __far16    *fptr = ptr;

    return( fptr );
}

void *cnv16to32_r( void __far16 *ptr )
{
    return( ptr );
}

void *cnv16to32_a( void __far16 *ptr )
{
    void    *nptr;

    nptr = ptr;
    return( nptr );
}

void *cnv16to32_i( void __far16 *ptr )
{
    void    *nptr = ptr;

    return( nptr );
}

int main( void )
{
    int         i = 123;
    int         *pi = &i;
    int         *pi2;
    int __far16 *p16i;
    p16f_t      p16f;
    pf_t        pf;

    p16i = (int __far16 *)cnv32to16_r( pi );

    if( cnv32to16_i( pi ) != cnv32to16_a( pi ) ) _fail;
    if( cnv32to16_i( pi ) != cnv32to16_r( pi ) ) _fail;
    if( cnv16to32_i( p16i ) != cnv16to32_a( p16i ) ) _fail;
    if( cnv16to32_i( p16i ) != cnv16to32_r( p16i ) ) _fail;

    /* Test conversion of function arguments */
    pi2 = (int __far16 *)cnv16to32_r( p16i );
    if( *pi2 != 123 ) _fail;
    if( pi2 != cnv16to32_r( pi ) ) _fail;

    /* Test conversion of pointers to functions */
    pf = &foo;
    p16f = (p16f_t)cnv32to16_r( pf );

    if( cnv32to16_i( pf ) != cnv32to16_a( pf ) ) _fail;
    if( cnv32to16_i( pf ) != cnv32to16_r( pf ) ) _fail;
    if( cnv16to32_i( p16f ) != cnv16to32_a( p16f ) ) _fail;
    if( cnv16to32_i( p16f ) != cnv16to32_r( p16f ) ) _fail;

    _PASS;
}

#else
ALWAYS_PASS
#endif
