.func getgid
.synop begin
.if '&machsys' eq 'QNX' .do begin
#include <sys/types.h>
.do end
#include <unistd.h>
gid_t getgid( void );
.synop end
.desc begin
The
.id &funcb.
function returns the group ID for the current process.
.desc end
.return begin
The group ID for the current process.
.return end
.see begin
.seelist getuid geteuid getegid
.see end
.exmp begin
/*
 * Print the group ID of the process.
 */
#include <stdio.h>
.if '&machsys' eq 'QNX' .do begin
#include <sys/types.h>
.do end
#include <unistd.h>

int main( void )
  {
     printf( "I belong to group ID %d\n", getgid() );
     return( 0 );
  }
.exmp end
.class POSIX 1003.1
.system
