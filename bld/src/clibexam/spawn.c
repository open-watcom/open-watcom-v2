#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <errno.h>
#include <string.h>

void main()
  {
    int   process_id, status, rc;

    process_id = spawnl( P_NOWAIT, "child.exe",
               "child", "5", NULL );
    if( process_id == -1 ) {
      printf( "spawn failed - %s\n", strerror( errno ) );
      exit( EXIT_FAILURE );
    }
    printf( "Process id = %d\n", process_id );

#if defined(__OS2__) || defined(__NT__)
    rc = cwait( &status, process_id, WAIT_CHILD );
    if( rc == -1 ) {
      printf( "wait failed - %s\n", strerror( errno ) );
    } else {
      printf( "wait succeeded - %x\n", status );
      switch( status & 0xff ) {
      case 0:
        printf( "Normal termination exit code = %d\n",
                status >> 8 );
        break;
      case 1:
        printf( "Hard-error abort\n" );
        break;
      case 2:
        printf( "Trap operation\n" );
        break;
      case 3:
        printf( "SIGTERM signal not intercepted\n" );
        break;
      default:
        printf( "Bogus return status\n" );
      }
    }
#endif
    printf( "spawn completed\n" );
  }

/*
[child.c]
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

void main( int argc, char *argv[] )
  {
    int delay;

    if( argc <= 1 ) exit( EXIT_FAILURE );
    delay = atoi( argv[1] );
    printf( "I am a child going to sleep "
            "for %d seconds\n", delay );
    sleep( delay );
    printf( "I am a child awakening\n" );
    exit( 123 );

  }
*/
