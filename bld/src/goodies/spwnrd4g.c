/*
    spwnrd4g.c - The following program demonstrates how to
    spawn another Rational Systems DOS/4GW application.

    Compile and link: wcl386 -l=dos4g spwnrd4g.c
 */
#include <process.h>
#include <stdio.h>
#include <stdlib.h>

/* DOS/4GW var for WLINK MINREAL option */
unsigned __near __minreal = 100*1024;

void main()
{
    int app2_exit_code;

    puts( "Spawning a protected-mode application..."
          "using spawnlp() with P_WAIT" );
    app2_exit_code = spawnlp( P_WAIT, "dos4gw",
                            "dos4gw", "spwndd4g", NULL );
    printf( "Application #2 returned with exit code %d\n",
                            app2_exit_code );
}
