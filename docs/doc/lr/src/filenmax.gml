.func FILENAME_MAX
#include <stdio.h>
#define FILENAME_MAX 123
.funcend
.desc begin
The &func macro is the size of an array of char big enough to hold a string 
naming any file that the implementation expects to open;
If there is no practical file name length limit, &func is the recommended
size of such an array.  As file name string contents must meet other
system-specific constraints, some strings of length &func may not work.
.np
&func typically sizes an array to hold a file name.
.desc end
.return begin
The &func macro returns a positive integer value.
.return end
.exmp begin
#include <stdio.h>
#include <string.h>

.exmp break
int main( int argc, char *argv[] )
{
    if( argc ) {
        char fname[FILENAME_MAX];

        strcpy( fname, argv[0] );
        puts( fname );
    }
    return( 0 );
}
.exmp end
.class ANSI
.system
