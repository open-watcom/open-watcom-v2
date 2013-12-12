.func _pipe
.synop begin
#include <&iohdr>
int _pipe( int *phandles, unsigned psize, int textmode );
.synop end
.desc begin
The
.id &func.
function creates a pipe (an unnamed FIFO) and places a file
descriptor for the read end of the pipe in
.arg phandles[0]
and a file descriptor for the write end of the pipe in
.arg phandles[1]
.ct .li .
Their integer values are the two lowest available at the time of the
.id &func.
function call.
The
.kw O_NONBLOCK
flag is cleared for both file descriptors.
(The
.kw fcntl
call can be used to set the
.kw O_NONBLOCK
flag.)
.np
Data can be written to file descriptor
.arg phandles[1]
and read from file descriptor
.arg phandles[0]
.ct .li .
A read on file descriptor
.arg phandles[0]
returns the data written to
.arg phandles[1]
on a first-in-first-out (FIFO) basis.
.np
This function is typically used to connect together standard utilities
to act as filters, passing the write end of the pipe to the data
producing process as its
.kw STDOUT_FILENO
and the read end of the pipe to the data consuming process as its
.kw STDIN_FILENO
.ct .li .
(either via the traditional fork/dup2/exec or the more efficient spawn
calls).
.np
If successful,
.id &func.
marks for update the
.us st_ftime,
.us st_ctime,
.us st_atime
and
.us st_mtime
fields of the pipe for updating.
.desc end
.return begin
The
.id &func.
function returns zero on success.
Otherwise, (-1) is
returned and
.kw errno
is set to indicate the error.
.return end
.error begin
If any of the following conditions occur, the
.id &func.
function shall
return (-1) and set
.kw errno
to the corresponding value:
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EMFILE
The calling process does not have at least 2 unused file descriptors
available.
.term ENFILE
The number of simultaneously open files in the system would exceed the
configured limit.
.term ENOSPC
There is insufficient space available to allocate the pipe buffer.
.term EROFS
The pipe pathname space is a read-only filesystem.
.endterm
.error end
.see begin
.seelist &function. fcntl open _pclose perror _pipe _popen read write
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <io.h>
#include <process.h>

static int handles[2] = { 0, 0 };
static int pid;
.exmp break
create_pipe()
{
    if( _pipe( (int *)&handles, 2048, _O_BINARY ) == -1 ) {
        perror( "create_pipe" );
        exit( EXIT_FAILURE );
    }
}
.exmp break
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
.exmp break
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
    i = -1;
    write( handles[1], &i, sizeof( int ) );
    close( handles[1] );
}
.exmp break
empty_pipe( int in_pipe )
{
    int i;
    int amt;

    for(;;) {
        amt = read( in_pipe, &i, sizeof( int ) );
        if( amt != sizeof( int ) || i == -1 )
            break;
        printf( "Parent, 5 times %d is %d\n", i, 5*i );
    }
    if( amt == -1 ) {
        perror( "empty_pipe" );
        exit( EXIT_FAILURE );
    }
    close( in_pipe );
}
.exmp break
void main( int argc, char *argv[] )
{
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
.exmp output
Child, what is 5 times 1
Child, what is 5 times 2
Parent, 5 times 1 is 5
Parent, 5 times 2 is 10
Child, what is 5 times 3
Child, what is 5 times 4
Parent, 5 times 3 is 15
Parent, 5 times 4 is 20
Child, what is 5 times 5
Child, what is 5 times 6
Parent, 5 times 5 is 25
Parent, 5 times 6 is 30
Child, what is 5 times 7
Child, what is 5 times 8
Parent, 5 times 7 is 35
Parent, 5 times 8 is 40
Child, what is 5 times 9
Child, what is 5 times 10
Parent, 5 times 9 is 45
Parent, 5 times 10 is 50
.exmp end
.class WATCOM
.system
