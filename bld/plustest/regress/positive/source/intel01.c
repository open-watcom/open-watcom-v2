#include "fail.h"


#if ( defined(__386__) || defined(__I86__) ) && !defined( __OS2__ ) && !defined( __LINUX__ )

// verify __interrupt implies default __far

typedef void __interrupt __far (*FAR_INT)( void );

extern void __interrupt int1( void );

void __interrupt __export int2( void )
{
}

void set_vec( void ( __interrupt *p )( void ) )
{
    if( sizeof( p ) != sizeof( FAR_INT ) ) fail(__LINE__);
    if( sizeof( &int1 ) != sizeof( FAR_INT ) ) fail(__LINE__);
    if( sizeof( &int2 ) != sizeof( FAR_INT ) ) fail(__LINE__);
}

#else

void set_vec( int ) {
}

#endif

int main() {
    set_vec( 0 );
    _PASS;
}
