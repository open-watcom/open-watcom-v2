.func readlink
.synop begin
#include <unistd.h>
int readlink( const char *path, 
              char *buf, 
              size_t bufsiz );
.ixfunc2 '&OsIo' readlink
.synop end
.desc begin
The
.id &funcb.
function places the contents of the symbolic link named 
by path into the buffer pointed to by
.arg buf
.ct , which has a size of 
.arg bufsiz
.period
The contents of the returned symbolic link don't include 
a 
.id NULL
terminator. 
Its length must be determined from the 
.id stat
structure returned by the 
.reffunc lstat
function, or by the return value of the
.reffunc readlink
call.
.np
If 
.reffunc readlink
is successful, up to 
.arg bufsiz
bytes from the contents of the symbolic link are placed in
.arg buf
.period
.desc end
.return begin
On success, the number of bytes placed in the buffer
.arg buf
.period
Otherwise, -1 is returned, and
.id errno
is set to indicate the error.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EACCES
Search permission is denied for a component of the path prefix.
.term EINVAL
The named file isn't a symbolic link.
.term ELOOP
A loop exists in the symbolic links encountered during 
resolution of the path argument, and more than SYMLOOP_MAX 
symbolic links were encountered.
.term ENAMETOOLONG
A component of the path exceeded NAME_MAX characters, or 
the entire pathname exceeded PATH_MAX characters.
.term ENOENT
The named file doesn't exist.
.term ENOSYS
Links aren't supported by the resource manager associated with path.
.term ENOTDIR
A component of the path prefix named by path isn't a directory.
.endterm
.error end
.see begin
.seelist read close creat fread open write
.see end
.exmp begin
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
.exmp break
char buf[PATH_MAX + 1];

int main( int argc, char **argv )
{
  int n;
  int len;
  int ecode = 0;

  for( n = 1; n < argc; ++n ) {
    if( (len = readlink( argv[n], buf, PATH_MAX )) == -1 ) {
      perror( argv[n] );
      ecode++;
    } else {
      buf[len] = '\0';
      printf( "%s -> %s\n", argv[n], buf );
    }
  }
  return( ecode );
}
.exmp end
.class POSIX 1003.1
.system
