.func va_start
#include <stdarg.h>
void va_start( va_list param, previous );
.ixfunc2 'variable arguments' &func
.synop end
.desc begin
&func is a macro used to start the acquisition of arguments
from a list of variable arguments.
The
.arg param
argument is used by the
.kw va_arg
macro to locate the current acquired argument.
The
.arg previous
argument is the argument that immediately precedes the
.id "..."
notation in the original function definition.
It must be used with the associated macros
.kw va_arg
and
.kw va_end
.ct .li .
See the description of
.kw va_arg
for complete documentation on these macros.
.desc end
.return begin
The macro does not return a value.
.return end
.see begin
.im seevarg va_start
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
.exmp break
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
.class ANSI
.system
