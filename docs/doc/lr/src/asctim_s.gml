.func asctime_s _wasctime_s
#define __STDC_WANT_LIB_EXT1__ 1
#include <time.h>
errno_t asctime_s( char * s,
                   rsize_t maxsize,
                   const struct tm * timeptr);
.ixfunc2 '&TimeFunc' &func
.if &'length(&wfunc.) ne 0 .do begin
errno_t _wasctime_s( wchar_t * s,
                     rsize_t maxsize,
                     const struct tm * timeptr);
.ixfunc2 '&TimeFunc' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.im structtm
.synop end
.*
.rtconst begin
Neither
.arg s
nor
.arg timeptr
shall be a null pointer.
maxsize shall not be less than 26 and shall not be greater than
.arg RSIZE_MAX
.ct .li .
The broken-down time pointed to by
.arg timeptr
shall be normalized. The calendar year represented by the broken-down time
pointed to by
.arg timeptr
shall not be less than calendar year 0 and shall not be greater than calendar
year 9999.
If there is a runtime-constraint violation,
there is no attempt to convert the time, and
.arg s[0]
is set to a null character if
.arg s
is not a null pointer and
.arg maxsize
is not zero and is not greater than
.arg RSIZE_MAX
.ct .li .
.rtconst end
.*
.desc begin
The &func function converts the normalized broken-down time in the structure
pointed to by
.arg timeptr
into a 26 character (including the null character) string in the form
.millust begin
Sun Sep 16 01:03:52 1973\n\0
.millust end
The fields making up this string are (in order):
:ol.
:li. The name of the day of the week represented by
.arg timeptr->tm_wday
using the following three character weekday names:
.np
Sun, Mon, Tue, Wed, Thu, Fri, and Sat.
:li. The character space.
:li. The name of the month represented by
.arg timeptr->tm_mon
using the following three character month names:
.np
Jan, Feb, Mar,Apr,May,Jun, Jul, Aug, Sep, Oct, Nov, and Dec.
:li. The character space.
:li. The value of
.arg timeptr->tm_mday
as if printed using the fprintf format "%2d".
:li. The character space.
:li. The value of timeptr->tm_hour as if printed using the fprintf format "%.2d".
:li. The character colon.
:li. The value of
.arg timeptr->tm_min
as if printed using the fprintf format "%.2d".
:li. The character colon.
:li. The value of
.arg timeptr->tm_sec
as if printed using the fprintf format "%.2d".
:li. The character space.
:li. The value of
.arg timeptr->tm_year + 1900
as if printed using the fprintf format "%4d".
:li. The character new line.
:li. The null character.
:eol.
.im widefunc
.desc end
.*
.return begin
.saferet
.return end
.*
.see begin
.im seetime asctime_s
.see end
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <time.h>
.exmp break
void main()
{
    struct tm  time_of_day;
    time_t     ltime;
    auto char  buf[26];
.exmp break
    time( &ltime );
    _localtime( &ltime, &time_of_day );
    asctime_s( buf, sizeof( buf ), &time_of_day );
    printf( "Date and time is: %s\n", buf );
}
.exmp output
Date and time is: Mon Jan 30 11:32:45 2006
.exmp end
.class TR 24731
.system
