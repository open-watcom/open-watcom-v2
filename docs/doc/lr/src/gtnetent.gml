.func getnetent
.synop begin
#include <netdb.h>
struct netent *getnetent( void );
.synop end
.desc begin
The
.id &funcb.
function retrieves the next entry in the network database.  If
not proceeded by an appropriate call to
.kw setnetent
, the function will always return the first network in the
database.  The routine will query the local database only.
.np
The structure returned is defined as:
.blkcode begin
struct netent {
    char     *n_name;       /* official network name */
    char     **n_aliases;   /* alias list */
    int      n_addrtype;    /* address type */
    uint32_t n_net;         /* network number */
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
.kw n_aliases
member as a NULL-terminated list, and this structure entry will
never be NULL.
.np
If no additional network is found or an error occurs, the return
value will be NULL.
.return end
.see begin
.seelist setnetent endnetent getnetbyname getnetbyaddr
.see end
.class POSIX
.system
