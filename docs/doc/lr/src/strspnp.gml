.func strspnp _fstrspnp
.synop begin
#include <string.h>
char *strspnp( const char *s,
               const char *charset );
.ixfunc2 '&String' strspnp
.ixfunc2 '&Search' strspnp
.if &farfnc ne 0 .do begin
char __far *_fstrspnp( const char __far *s,
                       const char __far *charset );
.ixfunc2 '&String' _fstrspnp
.ixfunc2 '&Search' _fstrspnp
.do end
.synop end
.desc begin
The
.id &funcb.
function returns a pointer to the first character in
.arg s
that does not belong to the set of characters in
.arg charset
.period
The terminating null character is not considered to be part of
.arg charset
.period
.farfunc &ffunc. &funcb.
.desc end
.return begin
These functions return
.mono NULL
if
.arg s
consists entirely of characters
from
.arg charset
.period
.return end
.see begin
.seelist strspnp strcspn strspn
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%s\n", strspnp( "out to lunch", "aeiou" ) );
    printf( "%s\n", strspnp( "out to lunch", "xyz" ) );
  }
.exmp output
t to lunch
out to lunch
.exmp end
.class WATCOM
.system
