/*
    spwndcw.c - Will be spawned by the spwnrcw program.

    Compile & Link: wcl386 -l=causeway spwndcw.c
 */
#include <stdio.h>
#include <stdlib.h>

void main()
{
    puts( "\nApplication #2 spawned\n" );
    /* Send back exit code 59 */
    exit( 59 );
}
