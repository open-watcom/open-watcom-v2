.func _fullpath _wfullpath
#include <stdlib.h>
char *_fullpath( char *buffer,
                 const char *path,
                 size_t size );
.ixfunc2 '&Parsing' &func
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wfullpath( wchar_t *buffer ,
                     const wchar_t *path,
                     size_t size );
.ixfunc2 '&Parsing' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function returns the full pathname of the file specification
in
.arg path
in the specified buffer
.arg buffer
of length
.arg size
.ct .li .
.np
The maximum size that might be required for
.arg buffer
is
.kw _MAX_PATH
.ct .li .
If the buffer provided is too small,
.mono NULL
is returned and
.kw errno
is set.
.np
If
.arg buffer
is
.mono NULL
then a buffer of size
.kw _MAX_PATH
is allocated using
.kw malloc
.ct .li .
This buffer may be freed using the
.kw free
function.
.np
If
.arg path
is
.mono NULL
or points to a null string ("") then the current working directory is
returned in
.arg buffer
.ct .li .
.im widefunc
.desc end
.return begin
The &func function returns a pointer to the full path specification if
no error occurred.
Otherwise,
.mono NULL
is returned.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term ENOENT
The current working directory could not be obtained.
.term ENOMEM
The buffer could not be allocated.
.term ERANGE
The buffer passed was too small.
.endterm
.error end
.see begin
.seelist _fullpath _makepath _splitpath
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
.exmp break
void main( int argc, char *argv[] )
  {
    int i;
    char buff[ PATH_MAX ];
.exmp break
    for( i = 1; i < argc; ++i ) {
      puts( argv[i] );
      if( _fullpath( buff, argv[i], PATH_MAX ) ) {
        puts( buff );
      } else {
        puts( "FAIL!" );
      }
    }
  }
.exmp end
.class WATCOM
.system
