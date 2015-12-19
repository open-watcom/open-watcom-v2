.func gethostbyname
.synop begin
#include <netdb.h>
struct hostent *gethostbyname( const char *name );
.synop end
.desc begin
The
.id &funcb.
function determines the address or addresses of a host specified
by the
.arg name
argument.  The routine will query the local database initially.
If not found, the routine will perform a DNS query, returning 
all "A" records for the given host name.
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
In the current Open Watcom implementation, this routine will only
ever return IPv4 addresses, and all addresses will be of 
.kw AF_INET
address type.
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
The structure entry
.kw h_aliases
will be returned as NULL.  If the host is found, the
.kw h_name
member will be a copy of the
.arg name
argument.  The addresses are contained in the 
.kw h_addr_list
member as a NULL-terminated list, and this structure entry will
never be NULL.
.np
If no matching host is found or an error occurs, the return value
will be NULL.
.return end
.see begin
.seelist gethostent
.see end
.exmp begin
.blktext begin
The following program will attempt to determine the address of
a hostname passed as an argument.
.blktext end
.blkcode begin
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
  {
    int i;
    struct hostent *he;
    struct in_addr **addr_list;
    
    if (argc != 2) {
        fprintf(stderr,"usage: %s <hostname>\n", argv[0]);
        return 1;
    }
    
    if ((he = gethostbyname(argv[1])) == NULL) {  // get the host info
        printf("Failed to find %s\n", argv[1]);
        return 2;
    }

    printf("IP addresses for %s:\n", argv[1]);
    addr_list = (struct in_addr **)he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) {
        printf("    %s\n", inet_ntoa(*addr_list[i]));
    }

    return 0;
  }

.blkcode end
.exmp end
.class POSIX
.system
