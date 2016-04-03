.func endservent
.synop begin
#include <netdb.h>
void endservent( void );
.synop end
.desc begin
The
.id &funcb.
function closes the service database. 
.np
This function is not thread-safe.  Other calls to this function
or to other functions accessing the hostname database may affect 
the return value from this function.
.desc end
.see begin
.seelist getservent setservent getservbyname getservbyport
.see end
.class POSIX
.system
