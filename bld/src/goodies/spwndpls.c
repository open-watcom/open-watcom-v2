/*
    SPWNDPLS.C - Will be spawned by the SPWNRPLS program.

    Compile & Link: wcl386 /l=pharlap spwndpls
 */
#include <stdio.h>
#include <stdlib.h>

void main()
{
    puts( "\nApplication #2 spawned\n" );
    /* Exit with error code 59 */
    exit( 59 );
}
