.func cprintf
.synop begin
#include <conio.h>
int cprintf( const char *format, ... );
.ixfunc2 '&KbIo' &func
.synop end
.desc begin
The &func function writes output directly to the console under control
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
.desc end
.return begin
The &func function returns the number of characters written.
.return end
.see begin
.im seeprtf cprintf
.see end
.exmp begin
#include <conio.h>

void main()
  {
    char *weekday, *month;
    int day, year;
.exmp break
    weekday = "Saturday";
    month = "April";
    day = 18;
    year = 1987;
    cprintf( "%s, %s %d, %d\n",
          weekday, month, day, year );
  }
.exmp output
Saturday, April 18, 1987
.exmp end
.class WATCOM
.system
