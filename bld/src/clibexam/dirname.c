#include <stdio.h>
#include <libgen.h>

int main( void )
{
    puts( dirname( "/usr/lib" ) );
    puts( dirname( "/usr/" ) );
    puts( dirname( "usr" ) );
    puts( dirname( "/" ) );
    puts( dirname( ".." ) );
    return( 0 );
}
