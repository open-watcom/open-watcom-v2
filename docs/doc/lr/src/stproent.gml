.func setprotoent
.synop begin
#include <netdb.h>
void setprotoent( int stayopen );
.synop end
.desc begin
The
.id &funcb.
function opens or rewinds the protocol database to allow
reading starting at the first entry.  If
.arg stayopen
is non-zero, the database will remain open between subsequent
calls to 
.kw getprotoent
until the
.kw endprotoent
function is called.
.np
This function is not thread-safe.  Other calls to this function
or to other functions accessing the protocol database may affect 
the return value from this function.
.desc end
.see begin
.seelist getprotoent endprotoent getprotobyname getprotobynumber
.see end
.class POSIX
.system
