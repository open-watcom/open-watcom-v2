/*
    SPWNDD4G.C - Will be spawned by the SPWNRD4G program.

    Compile & Link: wcl386 /l=dos4g spwndd4g
 */
#include <stdio.h>
#include <stdlib.h>

void main()
{
    puts( "\nApplication #2 spawned\n" );
    /* Send back exit code 59 */
    exit( 59 );
}
