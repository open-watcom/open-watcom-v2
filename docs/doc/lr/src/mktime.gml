.func mktime
.synop begin
#include <time.h>
time_t mktime( struct tm *timeptr );
.im structtm
.ixfunc2 '&TimeFunc' &func
.synop end
.desc begin
The &func function converts the local time information in the
structure pointed to by
.arg timeptr
into a calendar time (Coordinated Universal Time) with the same
encoding used by the
.kw time
function.
The original values of the fields
.kw tm_sec
.ct,
.kw tm_min
.ct,
.kw tm_hour
.ct,
.kw tm_mday
.ct,
and
.kw tm_mon
are not restricted to ranges described for
.kw struct tm
.ct .li .
If these fields are not in their proper ranges, they are adjusted
so that they are in the proper ranges.
Values for the fields
.kw tm_wday
and
.kw tm_yday
are computed after all the other fields have been adjusted.
.np
If the original value of
.kw tm_isdst
is negative, this field is computed also.
Otherwise, a value of 0 is treated as "daylight savings time is not in
effect" and a positive value is treated as "daylight savings time is
in effect".
.np
Whenever &func is called, the
.kw tzset
function is also called.
.desc end
.return begin
The &func function returns the converted calendar time.
.return end
.see begin
.im seetime mktime
.see end
.exmp begin
#include <stdio.h>
#include <time.h>
.exmp break
static const char *week_day[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
.exmp break
void main()
  {
    struct tm new_year;
.exmp break
    new_year.tm_year  = 2001 - 1900;
    new_year.tm_mon   = 0;
    new_year.tm_mday  = 1;
    new_year.tm_hour  = 0;
    new_year.tm_min   = 0;
    new_year.tm_sec   = 0;
    new_year.tm_isdst = 0;
    mktime( &new_year );
    printf( "The 21st century began on a %s\n",
             week_day[ new_year.tm_wday ] );
  }
.exmp output
The 21st century began on a Monday
.exmp end
.class ANSI
.system
