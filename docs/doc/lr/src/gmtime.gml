.func begin gmtime Functions
.func2 gmtime
.func2 _gmtime
.func end
.synop begin
#include <time.h>
struct tm * gmtime( const time_t *timer );
struct tm *_gmtime( const time_t *timer,
                    struct tm *tmbuf );
.ixfunc2 '&TimeFunc' gmtime
.ixfunc2 '&TimeFunc' _gmtime
.im structtm
.synop end
.*
.safealt
.*
.desc begin
The
.id &funcb.
functions convert the calendar time pointed to by
.arg timer
into a broken-down time, expressed as Coordinated Universal Time (UTC)
(formerly known as Greenwich Mean Time, or GMT).
.np
The function
.kw _gmtime
places the converted time in the
.kw tm
structure pointed to by
.arg tmbuf
.ct , and the
.id &funcb.
function places the converted time in a static
structure that is re-used each time
.id &funcb.
is called.
.im tzref
.desc end
.return begin
The
.id &funcb.
functions return a pointer to a structure containing the
broken-down time.
.return end
.see begin
.im seetime
.see end
.cp 12
.exmp begin
#include <stdio.h>
#include <time.h>

void main()
{
    time_t time_of_day;
    auto char buf[26];
    auto struct tm tmbuf;
.exmp break
    time_of_day = time( NULL );
    _gmtime( &time_of_day, &tmbuf );
    printf( "It is now: %.24s GMT\n",
            _asctime( &tmbuf, buf ) );
}
.exmp output
It is now: Fri Dec 25 15:58:27 1987 GMT
.exmp end
.class ISO C
.system
