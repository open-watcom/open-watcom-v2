#include <stdio.h>
#include <direct.h>

void main( void )
{
   int drive = 3;
   if( _chdrive( drive  ) == 0 )
       printf( "Changed the current drive to %c\n",
               'A' + drive - 1 );
}
