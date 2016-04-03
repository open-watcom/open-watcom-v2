.func getegid
.synop begin
#include <sys/types.h>
#include <unistd.h>
gid_t getegid( void );
.synop end
.desc begin
The
.id &funcb.
function returns the efective group ID for the current process.
.desc end
.return begin
The efective group ID for the current process.
.return end
.see begin
.seelist getgid getuid geteuid
.see end
.exmp begin
/*
 * Print the effective group ID of the process.
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main( void )
  {
     printf( "My effective group ID is %d\n", getegid() );
     return( 0 );
  }
.exmp end
.class POSIX 1003.1
.system
