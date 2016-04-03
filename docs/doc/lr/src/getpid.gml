.func getpid _getpid
.synop begin
#include <sys/types.h>
#include <unistd.h>
pid_t getpid( void );
.if &'length(&_func.) ne 0 .do begin
pid_t _getpid( void );
.do end
.synop end
.desc begin
The
.id &funcb.
function returns the process ID for the current process.
.desc end
.return begin
The process ID for the current process.
.return end
.see begin
.seelist getpid getppid gettid
.see end
.exmp begin
/*
 * print out the ID of this process
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main( void )
  {
    printf( "I'm process %d\n", getpid() );
    return( 0 );
  }
.exmp end
.class POSIX 1003.1
.system
