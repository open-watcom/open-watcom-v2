.func getservent
.synop begin
#include <netdb.h>
struct servent *getservent( void );
.synop end
.desc begin
The
.id &funcb.
function retrieves the next entry in the service database.  If
not proceeded by an appropriate call to
.kw setservent
, the function will always return the first service in the
database.  The routine will query the local database only.
.np
The structure returned is defined as:
.blkcode begin
struct servent {
    char    *s_name;        /* official service name */
    char    **s_aliases;    /* alias list */
    int     s_port;         /* port number */
    char    *s_proto;       /* protocol to use */
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
or to other functions accessing the hostname database may affect 
the return value from this function.
.desc end
.return begin
If the database contains more entries, the return value will be
non-NULL. The returned pointer should not be freed by the calling
routine.
.np
The alias names of said network are contained in the 
.kw s_aliases
member as a NULL-terminated list, and this structure entry will
never be NULL.
.np
If no additional network is found or an error occurs, the return
value will be NULL.
.return end
.see begin
.seelist setservent endservent getservbyname getservbyport
.see end
.class POSIX
.system
