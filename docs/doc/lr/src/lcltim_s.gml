.func localtime_s
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <time.h>
struct tm * localtime_s( const time_t * restrict timer,
                         struct tm * restrict result);
.ixfunc2 '&TimeFunc' localtime_s
.im structtm
.synop end
.*
.rtconst begin
Neither
.arg timer
nor
.arg result
shall be a null pointer.
If there is a runtime-constraint violation,
there is no attempt to convert the time.
.rtconst end
.*
.desc begin
The &func function converts the calendar time pointed to by
.arg timer
into a broken-down time, expressed as local time.
The broken-down time is stored in the structure pointed to by
.arg result
.ct .li .
.desc end
.*
.return begin
The &func function returns
.arg result
.ct , or a null pointer if the specified time cannot be converted to local time
or there is a runtime-constraint violation.
.return end
.*
.see begin
.im seetime localtime_s
.see end
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <time.h>
.exmp break
void main()
{
    time_t time_of_day;
    auto char buf[26];
    auto struct tm tmbuf;
.exmp break
    time_of_day = time( NULL );
    localtime_s( &time_of_day, &tmbuf );
    asctime_s( buf, sizeof( buf ), &tmbuf );
    printf( "It is now: %s", buf );
}
.exmp output
It is now: Mon Jan 30 15:28:33 2006
.exmp end
.class TR 24731
.system
