.func vcscanf
.synop begin
#include <conio.h>
#include <stdarg.h>
int vcscanf( const char *format, va_list args )
.ixfunc2 '&KbIo' &funcb
.synop end
.desc begin
The
.id &funcb.
function scans input from the console
under control of the argument
.arg format
.ct .li .
The
.id &funcb.
function uses the function
.kw getche
to read characters from the console.
The
.arg format
string is described under the description of the
.kw scanf
function.
.pp
The
.id &funcb.
function is equivalent to the
.kw cscanf
function, with a variable argument list replaced with
.arg arg
.ct , which has been initialized using the
.kw va_start
macro.
.desc end
.return begin
The
.id &funcb.
function returns
.kw EOF
when the scanning is terminated by reaching the end of the input stream.
Otherwise, the number of input arguments for which
values were successfully scanned and stored is returned.
When a file input error occurs, the
.kw errno
global variable may be set.
.return end
.see begin
.im seevscnf
.see end
.exmp begin
#include <conio.h>
#include <stdarg.h>
.exmp break
void cfind( char *format, ... )
  {
    va_list arglist;

    va_start( arglist, format );
    vcscanf( format, arglist );
    va_end( arglist );
  }
.exmp break
void main()
  {
    int day, year;
    char weekday[10], month[10];
.exmp break
    cfind( "%s %s %d %d",
            weekday, month, &day, &year );
    cprintf( "\n%s, %s %d, %d\n",
            weekday, month, day, year );
  }
.exmp end
.class WATCOM
.system
