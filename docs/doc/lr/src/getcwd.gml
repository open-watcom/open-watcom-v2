.func getcwd _wgetcwd
.if '&machsys' eq 'QNX' .do begin
#include <unistd.h>
char *getcwd( char *buffer, size_t size );
.do end
.el .do begin
#include <direct.h>
char *getcwd( char *buffer, size_t size );
.do end
.ixfunc2 '&Direct' &func
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wgetcwd( wchar_t *buffer, size_t size );
.ixfunc2 '&Direct' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function returns the name of the current working directory.
The
.arg buffer
address is either
.mono NULL
or is the location at which a string containing the name of the
current working directory is placed.
In the latter case, the value of
.arg size
is the length (including the delimiting
.mono '\0'
character) which can be be used to store this name.
.np
The maximum size that might be required for
.arg buffer
is
.kw PATH_MAX
+ 1 bytes.
.np
.us Extension:
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
name of the current working directory, unless an error occurs, in
which case
.mono NULL
is returned.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EINVAL
The argument
.arg size
is negative.
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
.if '&machsys' eq 'QNX' .do begin
#include <unistd.h>
.do end
.el .do begin
#include <direct.h>
.do end

void main()
  {
    char *cwd;
.exmp break
    cwd = getcwd( NULL, 0 );
    if( cwd != NULL ) {
      printf( "My working directory is %s\n", cwd );
      free( cwd );
    }
  }
.exmp output
.if '&machsys' eq 'QNX' .do begin
My working directory is /home/bill
.do end
.el .do begin
My working directory is C:\PROJECT\C
.do end
.exmp end
.class POSIX 1003.1 with extensions
.system
