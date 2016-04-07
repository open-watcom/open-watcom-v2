.func gethostent
.synop begin
#include <netdb.h>
struct hostent *gethostent( void );
.synop end
.desc begin
The
.id &funcb.
function reads, parses, and returns entries from the network host
database at /etc/hosts.  The first call will return the first
entry, and subsequent calls return subsequent entries.  A null
pointer is returned if either an error is encountered or no
further entries exist. 
.np
If a call to
.kw sethostent
was not first made, the
.id &funcb.
function will open the database if necessary initially.  By default,
this will always return the first entry in the database.
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
points to a static location, and the user should free neither 
the pointer itself nor any of its consituent structure members.
.np
This function is not thread-safe.  Other calls to functions
accessing the hostname database may affect the return value from
this function.
.desc end
.return begin
A parsed  host database entry, or NULL if no further entries
exist or an error occurred.
.return end
.see begin
.seelist sethostent endhostent inet_addr
.see end
.exmp begin
.blktext begin
The following program will print out each user and their user ID in
the system's password database
.blktext end
.blkcode begin
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
  {
  struct hostent *e;

    sethostent(1);
    e = gethostent();
    while(e != NULL) {
        char *ia = e->h_addr_list[0];
        printf("%s - %d.%d.%d.%d\n", 
               e->h_name, 
               ia[0],ia[1],ia[2],ia[3]

        );
        e = gethostent();
    }

    endhostent();
    return 0;
  }

.blkcode end
.exmp end
.class POSIX
.system
