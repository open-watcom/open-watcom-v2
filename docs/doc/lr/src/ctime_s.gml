.func ctime_s _wctime_s
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <time.h>
errno_t ctime_s( char * s,
                 rsize_t maxsize,
                 const time_t * timer);
.ixfunc2 '&TimeFunc' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
errno_t _wctime_s( wchar_t * s,
                   rsize_t maxsize,
                   const time_t * timer);
.ixfunc2 '&TimeFunc' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst begin
Neither
.arg s
nor
.arg timer
shall be a null pointer.
.arg maxsize
shall not be less than 26 and shall not be greater than
.arg RSIZE_MAX
.ct .li .
If there is a runtime-constraint violation,
.arg s[0]
is set to a null character if
.arg s
is not a null pointer and
.arg maxsize
is not equal zero and is not greater than
.arg RSIZE_MAX
.ct .li .
.rtconst end
.*
.desc begin
The
.id &func.
function converts the calendar time pointed to by
.arg timer
to local time in the form of a string. It is equivalent to
.millust begin
asctime_s( s, maxsize, localtime_s( timer ) )
.millust end
.desc end
.*
.newtext Recommended practice:
.np
The
.arg strftime
function allows more flexible formatting and supports locale-specific
behavior. If you do not require the exact form of the result string produced by the
.id &func.
function, consider using the
.arg strftime
function instead.
.oldtext
.*
.return begin
.saferet
.return end
.*
.see begin
.im seetime
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
.exmp break
    time_of_day = time( NULL );
    ctime_s( buf, sizeof( buf ), &time_of_day );
    printf( "It is now: %s", buf );
}
.exmp output
It is now: Mon Jan 30 14:29:55 2006
.exmp end
.class TR 24731
.system
