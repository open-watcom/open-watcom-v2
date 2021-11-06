.func getnetbyname
.synop begin
#include <netdb.h>
struct netent *getnetbyname( const char *name );
.synop end
.desc begin
The
.id &funcb.
function searches the network database for a network
.arg name
and returns information about said network if a match
is found.  The routine will query the local database only.
.np
The structure returned is defined as:
.blkcode begin
.im strucnet
.blkcode end
.np
The pointer returned by
.id &funcb.
points to a private location, and the user should free neither
the pointer itself nor any of its constituent structure members.
Subsequent calls to this function may result in the values
changing.
.np
This function is not thread-safe.  Other calls to this function
or to other functions accessing the hostname database may affect
the return value from this function.
.desc end
.return begin
If a matching host is found, the return value will be non-NULL.
The returned pointer should not be freed by the calling routine.
.np
The addresses are contained in the
.kw n_aliases
member as a NULL-terminated list, and this structure entry will
never be NULL.
.np
If no matching host is found or an error occurs, the return value
will be NULL.
.return end
.see begin
.seelist getnetent getnetbyaddr
.see end
.class POSIX 1003.1
.system
