.func begin unsetenv
.func2 _wunsetenv WATCOM
.func end
.synop begin
#include <stdlib.h>
int unsetenv( const char *name );
.ixfunc2 '&Process' unsetenv
int _wunsetenv( const wchar_t *name );
.ixfunc2 '&Process' _wunsetenv
.ixfunc2 '&Wide' _wunsetenv
.synop end
.desc begin
.ix 'environment'
The environment list consists of a number of environment names,
each of which has a value associated with it.
.np
The
.id &funcb.
delete all items with name
.arg name
from the environment list.
.widefunc &wfunc. &funcb.
.desc end
.return begin
The
.id &funcb.
function returns zero upon successful completion.
Otherwise, it will return -1 value and set
.kw errno
to indicate the error.
.return end
.error begin
.begterm 12
.term EINVAL
The name argument points to an empty string, or points to
a string containing an '=' character.
.endterm
.error end
.see begin
.im seeenv
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
.exmp break
int main( void )
  {
    if( unsetenv( "INCLUDE" ) == 0 )
      printf( "INCLUDE environment variable deleted\n" );
  }
.exmp end
.class POSIX 1003.1
.system
