.func _getdcwd _wgetdcwd
#include <direct.h>
char *_getdcwd( int drive, char *buffer, size_t maxlen );
.ixfunc2 '&Direct' &func
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wgetdcwd( int drive, wchar_t *buffer,
                    size_t maxlen );
.ixfunc2 '&Direct' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.funcend
.desc begin
The &func function gets the full path of the current working directory
on the specified drive.
The
.arg drive
argument specifies the drive (0 = default drive, 1 = A, 2 = B, etc.).
The
.arg buffer
address is either
.mono NULL
or is the location at which a string containing the name of the
current working directory is placed.
In the latter case, the value of
.arg maxlen
is the length (including the terminating
.mono '\0'
character) which can be be used to store this name.
An error occurs if the length of the path (including the terminating
.mono '\0'
character) exceeds
.arg maxlen
.ct .li .
.np
The maximum size that might be required for
.arg buffer
is
.kw PATH_MAX
+ 1 bytes.
.np
When
.arg buffer
has a value of
.mono NULL,
a string is allocated using
.kw malloc
to contain the name of the current working directory.
This string may be freed using the
.kw free
function.
.if &'length(&wfunc.) ne 0 .do begin
The &wfunc function is identical to &func except that it returns the
name of the current working directory as a wide-character string
(which is twice as long).
.do end
.desc end
.return begin
The &func function returns the address of the string containing the
name of the current working directory on the specified drive, unless
an error occurs, in which case
.mono NULL
is returned.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term ENODEV
The drive cannot be accessed.
.term ENOMEM
Not enough memory to allocate a buffer.
.term ERANGE
The buffer is too small (specified by
.arg size
.ct ) to contain the name of the current working directory.
.endterm
.error end
.see begin
.seelist &function. chdir chmod getcwd _getdcwd mkdir mknod rmdir
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>

void main()
  {
    char *cwd;
.exmp break
    cwd = _getdcwd( 3, NULL, 0 );
    if( cwd != NULL ) {
      printf( "The current directory on drive C is %s\n",
              cwd );
      free( cwd );
    }
  }
.exmp output
The current directory on drive C is C:\PROJECT\C
.exmp end
.class WATCOM
.system
