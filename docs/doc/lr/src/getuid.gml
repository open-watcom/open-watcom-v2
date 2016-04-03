.func getuid
.synop begin
#include <sys/types.h>
#include <unistd.h>
uid_t getuid( void );
.synop end
.desc begin
The
.id &funcb.
function returns the user ID for the calling process.
.desc end
.return begin
The user ID for the calling process
.return end
.see begin
.seelist geteuid getgid getegid
.see end
.exmp begin
/*
 * Print the user ID of this process.
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main( void )
  {
    printf( "My userid is %d\n", getuid() );
    return( 0 );
  }
.exmp end
.class POSIX 1003.1
.system
