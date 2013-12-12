.func jctime
#include <jtime.h>
unsigned char *jctime( const time_t *timer );
.ixfunc2 '&TimeFunc' jctime
.synop end
.desc begin
The &func function converts the calendar time pointed to by
.arg timer
to local time in the form of a string.
The &func function is equivalent to
.millust begin
jasctime( localtime( timer ) )
.millust end
.np
The &func functions convert the time into a string containing exactly
37 characters.
This string has the form shown in the following example:
.millust begin
:cmt. YYYY 年 MM 月 DD 日 （WW） HH:MM:SS
YYYY .. MM .. DD .. ( WW ) HH:MM:SS
.millust end
.pc
All fields have a constant width.
.begterm 10
.termhd1 Field
.termhd2 Meaning
.term YYYY
represents the year (e.g., 1992)
.term MM
represents the month (e.g., 11)
.term DD
represents the day (e.g., 29)
.term WW
represents the day of the week as a double-byte character
.term HH
represents the hours
.term MM
represents the minutes
.term SS
represents the seconds
.endterm
.np
The new-line character
.id '\n'
and the null character
.id '\0'
occupy the last two positions of the string.
.np
The &func function places the result string in a static buffer that is
re-used each time &func or
.kw jasctime
is called.
.np
Whenever the &func function is called, the
.kw tzset
function is also called.
.np
The calendar time is usually obtained by using the
.kw time
function.
That time is Coordinated Universal Time (UTC) (formerly known as
Greenwich Mean Time (GMT)).
.im tzref
.desc end
.return begin
The &func function returns the pointer to the string containing the
local time.
.return end
.see begin
.im seetime jctime
.see end
.exmp begin
#include <stdio.h>
#include <jtime.h>

void main()
  {
    time_t time_of_day;
.exmp break
    time_of_day = time( NULL );
    printf( "It is now: %s", jctime( &time_of_day ) );
  }
.exmp output
:cmt. It is now: 1992 年  9 月 28 日 （月） 16:01:40
It is now: 1992 ..  9 .. 28 .. ( .. ) 15:01:58
.exmp end
.class WATCOM
.system
