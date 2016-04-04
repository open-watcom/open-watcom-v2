.func getpgrp
.synop begin
.if '&machsys' eq 'QNX' .do begin
#include <sys/types.h>
.do end
#include <unistd.h>
gid_t getpgrp( void );
.synop end
.desc begin
The
.id &funcb.
function returns the process group ID for the current process.
.desc end
.return begin
The process group ID for the current process.
.return end
.see begin
.seelist getuid geteuid getegid
.see end
.exmp begin
/*
 * Print the process group ID of the process.
 */
#include <stdio.h>
.if '&machsys' eq 'QNX' .do begin
#include <sys/types.h>
.do end
#include <unistd.h>

int main( void )
  {
     printf( "I belong to group ID %d\n", getpgrp() );
     return( 0 );
  }
.exmp end
.class POSIX 1003.1
.system
