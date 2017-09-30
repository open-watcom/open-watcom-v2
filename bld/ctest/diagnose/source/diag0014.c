/* Test function return and dead code diagnostics */

// fake a longjmp()
typedef int jmp_buf[13];
void longjmp( jmp_buf env, int val );
#pragma aux longjmp __aborts;

jmp_buf     env;

int fn1( void )
{
    return;             
}                       // bad - not returning value

int fn2( void )
{
}                       // bad - not returning anything

int fn3( void )
{
    return( 1 );        // OK
}

int fn4( int i )
{
    longjmp( env, 1 );
    ++i;                // bad - unreachable code
}                       // OK - return not required because of longjmp()

int fn5( int i )
{
    if( i ) {
        longjmp( env, 1 );
    }
}                       // bad - not returning anything

int fn6( int i )
{
    if( i ) {
        longjmp( env, 1 );
    } else {
        return( 0 );
    }
}                       // OK - won't reach this point due to longjmp()

int fn7( int i )
{
    if( i ) {
        return( 1 );
    } else {
        return( 0 );
    }
    ++i;                // bad - unreachable code
}

int fn8( int i )
{
    if( i ) {
        return( 1 );
    }
    ++i;                
}                       // bad - not returning anything
