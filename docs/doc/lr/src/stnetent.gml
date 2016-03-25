.func setnetent
.synop begin
#include <netdb.h>
void setnetent( int stayopen );
.synop end
.desc begin
The
.id &funcb.
function opens or rewinds the network database for subsequent
access by the
.kw getnetent
function.  If 
.arg stayopen
is non-zero, the network database will remain open between calls
to the
.kw getnetent
function.  The database can be closed when
.kw endnetent
is called.
.np
This function is not thread-safe.  Other calls to this function
or to other functions accessing the hostname database may affect 
the return value from this function.
.desc end
.see begin
.seelist getnetent endnetent getnetbyname getnetbyaddr
.see end
.class POSIX
.system
