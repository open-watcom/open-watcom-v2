.func vcprintf
.synop begin
#include <conio.h>
#include <stdarg.h>
int vcprintf( const char *format, va_list arg );
.ixfunc2 '&KbIo' &funcb
.synop end
.desc begin
The
.id &funcb.
function writes output directly to the console under control
of the argument
.arg format
.ct .li .
The
.kw putch
function is used to output characters to the console.
The
.arg format
string is described under the description of the
.kw printf
function.
The
.id &funcb.
function is equivalent to the
.kw cprintf
function, with the variable argument list replaced with
.arg arg
.ct , which has been initialized by the
.kw va_start
macro.
.desc end
.return begin
The
.id &funcb.
function returns the number of characters written, or a
negative value if an output error occurred.
.im errnoref
.return end
.see begin
.im seevprtf
.see end
.exmp begin
#include <conio.h>
#include <stdarg.h>
#include <time.h>

#define ESCAPE 27
.exmp break
void tprintf( int row, int col, char *format, ... )
 {
    auto va_list arglist;
.exmp break
    cprintf( "%c[%2.2d;%2.2dH", ESCAPE, row, col );
    va_start( arglist, format );
    vcprintf( format, arglist );
    va_end( arglist );
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
.class WATCOM
.system
