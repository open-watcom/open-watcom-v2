.func jasctime
.synop begin
#include <jtime.h>
unsigned char *jasctime( const struct tm *timeptr );
.im structtm
.ixfunc2 '&TimeFunc' jasctime
.synop end
.desc begin
The
.id &func.
function converts the time information in the structure
pointed to by
.arg timeptr
into a string containing exactly 37 characters.
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
.pp
The
.id &func.
function places the result string in a static buffer that is
re-used each time
.id &func.
or
.kw jctime
is called.
.desc end
.return begin
The
.id &func.
function returns a pointer to the character string result.
.return end
.see begin
.im seetime
.see end
.exmp begin
#include <stdio.h>
#include <jtime.h>

void main()
  {
    struct tm  time_of_day;
    time_t     ltime;
.exmp break
    time( &ltime );
    _localtime( &ltime, &time_of_day );
    printf( "Date and time is: %s\n",
            jasctime( &time_of_day ) );
  }
.exmp output
:cmt. Date and time is: 1992 年  9 月 28 日 （月） 16:01:40
Date and time is: 1992 ..  9 .. 28 .. ( .. ) 16:01:40
.exmp end
.class WATCOM
.system
