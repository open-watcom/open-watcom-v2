.func endnetent
.synop begin
#include <netdb.h>
void endnetent( void );
.synop end
.desc begin
The
.id &funcb.
function closes the network database.
.np
This function is not thread-safe.  Other calls to this function
or to other functions accessing the hostname database may affect 
the return value from this function.
.desc end
.see begin
.seelist getnetent setnetent getnetbyname getnetbyaddr
.see end
.class POSIX
.system
