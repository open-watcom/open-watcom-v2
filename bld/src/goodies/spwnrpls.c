/*
    spwnrpls.c - The following program demonstrates how to
    spawn another Phar Lap application.

    Compile & Link:
        wcl386 -l=pharlap -"runt minr=300K,maxr=400K" spwnrpls.c
 */
#include <process.h>
#include <stdio.h>

void main()
{
    int app2_exit_code;

    puts( "Spawning a protect-mode application..."
          "using spawnlp() with P_WAIT" );
    puts( "Spawning application #2..." );
    app2_exit_code = spawnlp( P_WAIT, "run386",
                            "run386", "spwndpls", NULL );

    printf( "Application #2 returned with exit code %d",
                app2_exit_code );
}
