#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <io.h>
#include <dos.h>
#include <process.h>

static int handles[2] = { 0, 0 };
static int pid;

create_pipe()
  {
    if( _pipe( (int *)&handles, 2048, _O_BINARY ) == -1 ) {
      perror( "create_pipe" );
      exit( EXIT_FAILURE );
    }
  }

create_child( char *name )
  {
    char buff[10];

    itoa( handles[0], buff, 10 );
    pid = spawnl( P_NOWAIT, name,
               "_pipe", buff, NULL );
    close( handles[0] );
    if( pid == -1 ) {
      perror( "create_child" );
      close( handles[1] );
      exit( EXIT_FAILURE );
    }
  }

fill_pipe()
  {
    int i;
    int rc;

    for( i = 1; i <= 10; i++ ) {
      printf( "Child, what is 5 times %d\n", i );
      rc = write( handles[1], &i, sizeof( int ) );
      if( rc < sizeof( int ) ) {
        perror( "fill_pipe" );
        close( handles[1] );
        exit( EXIT_FAILURE );
      }
    }
    /* indicate that we are done */
    write( handles[1], &i, 1 );
    close( handles[1] );
  }

empty_pipe( int in_pipe )
  {
    int i;
    int amt;

    for(;;) {
      amt = read( in_pipe, &i, sizeof( int ) );
      if( amt != sizeof( int ) ) break;
      printf( "Parent, 5 times %d is %d\n", i, 5*i );
    }
    if( amt == -1 ) {
      perror( "empty_pipe" );
      exit( EXIT_FAILURE );
    }
    close( in_pipe );
  }

void main( int argc, char *argv[] )
  {
    _fileinfo = 1;

    if( argc <= 1 ) {
      /* we are the spawning process */
      create_pipe();
      create_child( argv[0] );
      fill_pipe();
    } else {
      /* we are the spawned process */
      empty_pipe( atoi( argv[1] ) );
    }
    exit( EXIT_SUCCESS );
  }
