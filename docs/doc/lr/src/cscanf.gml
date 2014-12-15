.func cscanf
.synop begin
#include <conio.h>
int cscanf( const char *format, ... );
.ixfunc2 '&KbIo' &funcb
.synop end
.desc begin
The
.id &funcb.
function scans input from the console
under control of the argument
.arg format
.ct .li .
Following the format string is a list of addresses to receive values.
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
.blktext begin
To scan a date in the form "Saturday April 18 1987":
.blktext end
.blkcode begin
#include <conio.h>

void main()
  {
    int day, year;
    char weekday[10], month[10];
.exmp break
    cscanf( "%s %s %d %d",
            weekday, month, &day, &year );
    cprintf( "\n%s, %s %d, %d\n",
            weekday, month, day, year );
  }
.blkcode end
.exmp end
.class WATCOM
.system
