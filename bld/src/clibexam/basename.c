#include <stdio.h>
#include <libgen.h>

int main( void )
{
    puts( basename( "/usr/lib" ) );
    puts( basename( "//usr//lib//" ) );
    puts( basename( "///" ) );
    puts( basename( "foo" ) );
    puts( basename( NULL ) );
    return( 0 );
}
