.func va_end
.synop begin
#include <stdarg.h>
void va_end( va_list param );
.ixfunc2 'variable arguments' &funcb
.synop end
.desc begin
.id &funcb.
is a macro used to complete the acquisition of arguments
from a list of variable arguments.
It must be used with the associated macros
.kw va_start
and
.kw va_arg
.ct .li .
See the description for
.kw va_arg
for complete documentation on these macros.
.desc end
.return begin
The macro does not return a value.
.return end
.see begin
.im seevarg
.see end
.exmp begin
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define ESCAPE 27
.exmp break
void tprintf( int row, int col, char *fmt, ... )
 {
    auto va_list ap;
    char *p1, *p2;

    va_start( ap, fmt );
    p1 = va_arg( ap, char * );
    p2 = va_arg( ap, char * );
    printf( "%c[%2.2d;%2.2dH", ESCAPE, row, col );
    printf( fmt, p1, p2 );
    va_end( ap );
 }
.exmp break
void main()
  {
    struct tm  time_of_day;
    time_t     ltime;
    auto char  buf[26];
.exmp break
    time( &ltime );
    _localtime( &ltime, &time_of_day );
    tprintf( 12, 1, "Date and time is: %s\n",
            _asctime( &time_of_day, buf ) );
  }
.exmp end
.class ISO C
.system
