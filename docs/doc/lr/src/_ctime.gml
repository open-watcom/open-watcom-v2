.func _ctime
.synop begin
#include <time.h>
char *_ctime( const time_t *timer, char *buf );
.ixfunc2 '&TimeFunc' &func
.synop end
.desc begin
The
.id &func.
function converts the calendar time pointed to by
.arg timer
to local time in the form of a string.
.pp
The
.id &func.
function converts the time into a string containing exactly
26 characters.
This string has the form shown in the following example:
.millust begin
Sat Nov 30 15:58:27 1991\n\0
.millust end
.pc
All fields have a constant width.
The new-line character
.id '\n'
and the null character
.id '\0'
occupy the last two positions of the string.
.pp
The string is placed in the buffer pointed to by
.arg buf
.ct .li .
.pp
Whenever
.id &func.
is called, the
.kw tzset
function is also called.
.pp
The calendar time is usually obtained by using the
.kw time
function.
That time is Coordinated Universal Time (UTC) (formerly known as
Greenwich Mean Time (GMT)).
.im tzref
.desc end
.return begin
The
.id &func.
function returns the pointer
to the string containing the local time.
.return end
.see begin
.im seetime _ctime
.see end
.exmp begin
#include <stdio.h>
#include <time.h>

void main()
  {
    time_t time_of_day;
    auto char buf[26];
.exmp break
    time_of_day = time( NULL );
    _ctime( &time_of_day, buf );
    printf( "It is now: %s", buf );
  }
.exmp output
It is now: Fri Nov 29 15:58:42 1991
.exmp end
.class WATCOM
.system
