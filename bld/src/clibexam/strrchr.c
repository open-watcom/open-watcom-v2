#include <stdio.h>
#include <string.h>

void main()
{
    printf( "%s\n", strrchr( "abcdeaklmn", 'a' ) );
    if( strrchr( "abcdeaklmn", 'x' ) == NULL )
        printf( "NULL\n" );
}
