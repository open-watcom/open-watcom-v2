/*
    spwnrcw.c - The following program demonstrates how to
    spawn another CauseWay application.

    Compile and link: wcl386 -l=causeway spwnrcw.c
 */
#include <process.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
    int app2_exit_code;

    puts( "Spawning a protected-mode application..."
          "using spawnlp() with P_WAIT" );
    app2_exit_code = spawnlp( P_WAIT, "spwndcw", NULL );
    printf( "Application #2 returned with exit code %d\n",
                            app2_exit_code );
}
