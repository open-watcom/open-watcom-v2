/*
    spwndd4g.c - Will be spawned by the spwnrd4g program.

    Compile & Link: wcl386 -l=dos4g spwndd4g.c
 */
#include <stdio.h>
#include <stdlib.h>

void main()
{
    puts( "\nApplication #2 spawned\n" );
    /* Send back exit code 59 */
    exit( 59 );
}
