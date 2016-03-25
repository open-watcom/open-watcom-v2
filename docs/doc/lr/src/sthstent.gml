.func sethostent
.synop begin
#include <netdb.h>
void sethostent(int stayopen);
.synop end
.desc begin
The
.id &funcb.
function opens the network host database at /etc/hosts and sets
the position for reading to the first entry.  If the network host
database is already open, the position is reset to the first entry.
The
.arg stayopen
argument, if non-zero, will cause the database to remain open after
subsequent calls to the
.kw gethostent
function.
.desc end
.see begin
.seelist gethostent endhostent
.see end
.class POSIX
.system
