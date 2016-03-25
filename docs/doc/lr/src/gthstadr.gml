.func gethostbyaddr
.synop begin
#include <netdb.h>
struct hostent *gethostbyaddr( const void *addr, 
                               socklen_t len,
                               int type );
.synop end
.desc begin
The
.id &funcb.
function searches the system's network host database for a host
matching the
.arg addr
argument.  The address should be specified in network byte order.
The
.arg len
argument specifies the length in bytes of the
.arg addr
argument, and 
.arg type
specifies the address type.  Accepted types, such as
.kw AF_INET
or
.kw AF_INET6
, are expected.  The routine will query only the local network
hosts database.
.np
The structure returned is defined as:
.blkcode begin
struct hostent {
    char        *h_name;       /* host official name */
    char        **h_aliases;   /* host alternate names, up to 16, 
                                * terminated by a NULL pointer
                                */
    int         h_addrtype;    /* address type */
    int         h_length;      /* address length in bytes */
    char **     **h_addr_list; /* array of pointers to network
                                * addresses in network byte
                                * order, terminated by a NULL
                                * pointer
                                */
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
If a matching host is found, the return value will be non-NULL.
The returned pointer should not be freed by the calling routine.
.np
If the host is found, the
.kw h_name
member will be the defintive name of the host, and
.kw h_aliases
will contain a NULL-terminated list of aliases.  The addresses 
are contained in the 
.kw h_addr_list
member as a NULL-terminated list, and this structure entry will
never be NULL.
.np
If no matching host is found or an error occurs, the return value
will be NULL.
.return end
.see begin
.seelist gethostent gethostbyname
.see end
.class POSIX
.system
