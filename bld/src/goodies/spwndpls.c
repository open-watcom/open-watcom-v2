/*
    spwndpls.c - Will be spawned by the spwnrpls program.

    Compile & Link: wcl386 -l=pharlap spwndpls.c
 */
#include <stdio.h>
#include <stdlib.h>

void main()
{
    puts( "\nApplication #2 spawned\n" );
    /* Send back exit code 59 */
    exit( 59 );
}
