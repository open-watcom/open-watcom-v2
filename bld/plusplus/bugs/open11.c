// need to add IC codes so that data will not be output if a function
// is not output
#include <stdio.h>

inline void foo( void )
{
    char *usage[] = { "str1", "str2", "str3", "str4", NULL };
    char **p;

    for( p = usage; *p; ++p ) {
        puts( *p );
    }
}

void main( void )
{
}
