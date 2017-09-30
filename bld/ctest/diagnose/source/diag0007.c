/* Test C99 style declaration diagnostics */

/* correct usage */
int foo( int arg )
{
    int a = 0;
    int i = 4;

    for( int i = 2, j = 1; i < 10; ++i )
        a += i + j;     // i in scope opened by 'for'

    return( a - i );    // back to the i in function scope
}

/* also correct usage */
int bar( int arg )
{
    ++arg;
    int i = 5;
    {
       --arg;
       int j = 4;
       i += j;
    }
    unsigned k = 3;
    return( i + k );
}

/* incorrect usage */
int main( int argc, char **argv )
{
    int     z = 0;

    for( register int i = 1; i < 2; ++i ) ;     // OK
    for( auto int i = 1; i < 2; ++i ) ;         // OK
    for( double i = 1; i < 2; ++i ) ;           // OK
    for( extern int i; i < 2; ++i ) ;           // bad storage class
    for( static int i = 1; i < 2; ++i ) ;       // bad storage class
    for( typedef int i; z < 2; ++z ) ;          // bad storage class
    for( register i = 1; i < 2; ++i ) ;         // missing type
    for( int i = 1, j(); i < 2; ++i ) ;         // j is not object

    // TODO: more tests for structs, enums, and other such nonsense in 'for'

    for( int i = 1; i < 2; ++i ) int j;         // no declaration here
    if( argc > 0 )
        float f;                                // or here

    if( argc < 0 )
        goto la_bel;
    else
        double g;                               // or here

    do
        unsigned u;                             // no declaration here

    while( --argc );
    unsigned uu;                                // OK here

    switch( argc )
        int i;                                  // no declaration here

    switch( argc ) {
    default:
        ++argc;
	int yy = 4;                             // OK here
	--yy;
    case 3:
        int zz = 3;                             // no declaration here
        ++zz;
    }
    return( foo( bar( argc ) ) );
la_bel:
    extern int zy;                              // no declaration here
}
