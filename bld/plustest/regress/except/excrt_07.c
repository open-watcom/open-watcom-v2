// excrt_06 - no handler, in nested catch
#include <stdio.h>
#include <except.h>

int main()
{
    try {
        throw 23;
    } catch( int ) {
        try {
            throw 24;
        } catch( long ) {
            printf( "FAILURE -- wrong catch!\n" );
        }
        printf( "FAILURE -- no error!\n" );
	fflush( stdout );
    }
    return( 0 );
}
