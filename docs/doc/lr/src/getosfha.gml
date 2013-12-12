.func _get_osfhandle
#include <&iohdr>
long _get_osfhandle( int posixhandle );
.synop end
.desc begin
The &func function returns the operating system's internal file handle
that corresponds to the POSIX-level file handle specified by
.arg posixhandle
.ct .li .
.np
The value returned by &func can be used as an argument to the
.kw _open_osfhandle
function which can be used to connect a second POSIX-level handle to
an open file.
.np
The example below demonstrates the use of these two functions.
Note that the example shows how the
.kw dup2
function can be used to obtain almost identical functionality.
.np
When the POSIX-level file handles associated with one OS file handle
are closed, the first one closes successfully but the others return
an error (since the first call close the file and released the OS file
handle).
So it is important to call
.kw close
at the right time, i.e., after all I/O operations are completed to the
file.
.desc end
.return begin
If successful, &func returns an operating system file handle
corresponding to
.arg posixhandle
.ct .li .
Otherwise, it returns -1 and sets
.kw errno
to
.kw EBADF
.ct , indicating an invalid file handle.
.return end
.see begin
.seelist &function. close dup2 fdopen _get_osfhandle _hdopen
.seelist &function. open _open_osfhandle _os_handle
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <&iohdr>
#include <fcntl.h>

void main()
{
    long os_handle;
    int fh1, fh2, rc;
.exmp break
    fh1 = open( "file",
                O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    if( fh1 == -1 ) {
        printf( "Could not open output file\n" );
        exit( EXIT_FAILURE );
    }
    printf( "First POSIX handle %d\n", fh1 );
.exmp break
#if defined(USE_DUP2)
    fh2 = 6;
    if( dup2( fh1, fh2 ) == -1 ) fh2 = -1;
#else
    os_handle = _get_osfhandle( fh1 );
    printf( "OS Handle %ld\n", os_handle );

    fh2 = _open_osfhandle( os_handle, O_WRONLY |
                                      O_BINARY );
#endif
    if( fh2 == -1 ) {
        printf( "Could not open with second handle\n" );
        exit( EXIT_FAILURE );
    }
    printf( "Second POSIX handle %d\n", fh2 );
.exmp break
    rc = write( fh2, "trash\x0d\x0a", 7 );
    printf( "Write file using second handle %d\n", rc );

    rc = close( fh2 );
    printf( "Closing second handle %d\n", rc );
    rc = close( fh1 );
    printf( "Closing first handle %d\n", rc );
}
.exmp end
.class WATCOM
.system
