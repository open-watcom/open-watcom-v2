.func endprotoent
.synop begin
#include <netdb.h>
void endprotoent( void );
.synop end
.desc begin
The
.id &funcb.
function explicitly closes the protocol database.
.np
This function is not thread-safe.  Other calls to this function
or to other functions accessing the protocol database may affect 
the return value from this function.
.desc end
.see begin
.seelist getprotoent setprotoent getprotobyname getprotobynumber
.see end
.class POSIX
.system
