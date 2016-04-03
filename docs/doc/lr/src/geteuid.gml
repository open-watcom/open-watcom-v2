.func geteuid
.synop begin
#include <sys/types.h>
#include <unistd.h>
uid_t geteuid( void );
.synop end
.desc begin
The
.id &funcb.
function returns the efective user ID for the calling process.
.desc end
.return begin
The efective user ID for the calling process
.return end
.see begin
.seelist getuid getgid getegid
.see end
.exmp begin
/*
 * Print the effective user ID of the process.
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main( void )
  {
    printf( "My effective user ID is %d\n", geteuid() );
    return( 0 );
  }
.exmp end
.class POSIX 1003.1
.system
