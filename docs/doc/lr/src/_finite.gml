.func _finite
.synop begin
#include <float.h>
int _finite( double x );
.ixfunc2 '&Math' &func
.synop end
.desc begin
.ix 'infinity'
.ix 'NAN'
The
.id &func.
function determines whether the double precision
floating-point argument is a valid number (i.e., not infinite
and not a NAN).
.desc end
.return begin
The
.id &func.
function returns 0 if the number is not valid and
non-zero otherwise.
.return end
.see begin
.seelist _clear87 _control87 _controlfp _finite _fpreset
.seelist printf _status87 isfinite fpclassify
.see end
.exmp begin
#include <stdio.h>
#include <float.h>

void main()
  {
    printf( "%s\n", (_finite( 1.797693134862315e+308 ) )
        ? "Valid" : "Invalid" );
    printf( "%s\n", (_finite( 1.797693134862320e+308 ) )
        ? "Valid" : "Invalid" );
  }
.exmp output
Valid
Invalid
.exmp end
.class WATCOM
.system
