.func setservent
.synop begin
#include <netdb.h>
void setservent( int stayopen );
.synop end
.desc begin
The
.id &funcb.
function opens or rewinds the service database.  If
.arg stayopen
is non-zero, the database will be kept open between calls
to
.kw getservent
until
.kw endservent
is called to close the database.
.np
This function is not thread-safe.  Other calls to this function
or to other functions accessing the hostname database may affect 
the return value from this function.
.desc end
.see begin
.seelist getservent endservent getservbyname getservbyport
.see end
.class POSIX
.system
