.func getprotobyname
.synop begin
#include <netdb.h>
struct netent *getprotobyname( const char *name );
.synop end
.desc begin
The
.id &funcb.
function searches the protocol database for a protocol
matching the specified
.arg name
, considering both the official name and aliases. The 
routine will query the local database only.
.np
The structure returned is defined as:
.blkcode begin
struct protoent {
    char    *p_name;        /* official protocol name */
    char    **p_aliases;    /* alias list */
    int     p_proto;        /* protocol number */
};
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
or to other functions accessing the protocol database may affect 
the return value from this function.
.desc end
.return begin
If a matching protocol database entry is found, the return value
will point to a
.kw struct protoent
as defined above.
.np
The alias names of said protocol are contained in the 
.kw p_aliases
member as a NULL-terminated list, and this structure entry will
never be NULL.
.np
If no protocol is found or an error occurs, the return value
will be NULL.
.return end
.see begin
.seelist getprotoent setprotoent endprotoent getprotobynumber
.see end
.class POSIX
.system
