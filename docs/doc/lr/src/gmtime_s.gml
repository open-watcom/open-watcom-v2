.func gmtime_s
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <time.h>
struct tm * gmtime_s( const time_t * restrict timer,
                      struct tm * restrict result );
.ixfunc2 '&TimeFunc' gmtime_s
.im structtm
.synop end
.*
.rtconst begin
Neither
.arg timer
nor
.arg result
shall be a null pointer.
If there is a runtime-constraint violation, there is no attempt to convert the time.
.rtconst end
.*
.desc begin
The
.id &func.
function converts the calendar time pointed to by
.arg timer
into a broken-down time, expressed as UTC. The broken-down time is stored in
the structure pointed to by
.arg result
.ct .li .
.desc end
.*
.return begin
The
.id &func.
function returns
.arg result
.ct , or a null pointer if the specified time cannot
be converted to UTC or there is a runtime-constraint violation.
.return end
.*
.see begin
.im seetime gmtime_s
.see end
.cp 12
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <time.h>

void main()
{
    time_t time_of_day;
    auto char buf[26];
    auto struct tm tmbuf;
.exmp break
    time_of_day = time( NULL );
    gmtime_s( &time_of_day, &tmbuf );
    asctime_s( buf, sizeof( buf ), &tmbuf );
    printf( "It is now: %.24s GMT\n", buf );
}
.exmp output
It is now: Thu Jan 31 15:12:27 2006 GMT
.exmp end
.class TR 24731
.system
