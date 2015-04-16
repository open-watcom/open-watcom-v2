.func begin ttyname
.func2 ttyname_r
.func end
.synop begin
#include <unistd.h>
char *ttyname( int fd );
int ttyname_r( int fd, char *buf, int buflen );
.synop end
.desc begin
The POSIX-compliant
.id &funcb.
function retrieves the name of a TTY as specified by the
.arg fd
file descriptor, returning a static pointer to the name.
Subsequent calls may change the contents of the string 
pointed to by the original call to 
.id &funcb.
, and the memory to which the return value points should not
be freed.
.np
The 
.kw ttyname_r
function performs the same function as
.id &funcb.
, but the name of the TTY is returned in the
.arg buf
argument.  The argument
.arg buflen
should specify the size of the
.arg buf
argument.  The
.kw ttyname_r
function is an Open Watcom extension commonly implemented in other 
standard libraries.
.desc end
.return begin
Upon success, the 
.id &funcb.
function returns a pointer to statically allocated memory containing
the name of the TTY.   Upon failure, the returned pointer will be
.kw NULL
and
.kw errno
will be set appropriately.
.np
Upon success, the
.kw ttyname_r
function will return zero, and the
.arg buf
argument will contain the name of the TTY.   Upon failure, the
.kw ttyname_r
function will return the error code and set
.kw errno
as well.
.return end
.error begin
.begterm 4
.termhd1 Constant
.termhd2 Meaning
.term ENOTTY
The value of
.arg fd
does not refer to a TTY
.term EACCES
The user does not have permission to query the file descriptor
.term EBADFD
The file descriptor
.arg  fd
is invalid
.term ERANGE
The
.arg buf
is not large enough to hold the result, or the file descriptor name exceeds
.kw POSIX_PATH_MAX
.endterm
.error end
.see begin
.seelist isatty
.see end
.class POSIX 1003.1, WATCOM
.system
