.func getppid
.synop begin
.if '&machsys' eq 'QNX' .do begin
#include <sys/types.h>
.do end
#include <unistd.h>
pid_t getppid(void);
.synop end
.desc begin
The
.id &funcb.
function allows the calling process to find out its parent ID.
.desc end
.return begin
The process parent's process ID.
.return end
.see begin
.seelist getpid
.see end
.exmp begin
/*
 * Print the parent's process ID.
 */
#include <stdio.h>
.if '&machsys' eq 'QNX' .do begin
#include <sys/types.h>
.do end
#include <unistd.h>

int main( void )
  {
    printf( "My parent is %d\n", getppid() );
    return( 0 );
  }
.exmp end
.class POSIX 1003.1
.system
