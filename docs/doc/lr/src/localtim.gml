.func begin localtime Functions
.func2 localtime
.func2 _localtime
.func end
.synop begin
#include <time.h>
struct tm * localtime( const time_t *timer );
struct tm *_localtime( const time_t *timer,
                       struct tm *tmbuf );
.ixfunc2 '&TimeFunc' localtime
.ixfunc2 '&TimeFunc' _localtime
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
into a structure of type
.kw tm
.ct, of time information, expressed as local time.
Whenever
.id &funcb.
is called, the
.kw tzset
function is also called.
.np
The calendar time is usually obtained by using the
.kw time
function.
That time is Coordinated Universal Time (UTC) (formerly known as
Greenwich Mean Time (GMT)).
.np
The
.kw _localtime
function places the converted time in the
.kw tm
structure pointed to by
.arg tmbuf
.ct , and the
.id &funcb.
function places the converted time in a static structure
that is re-used each time
.id &funcb.
is called.
.im tzref
.desc end
.return begin
The
.id &funcb.
functions return a pointer to a
.kw tm
structure containing the time information.
.return end
.see begin
.im seetime
.see end
.exmp begin
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
    _localtime( &time_of_day, &tmbuf );
    printf( "It is now: %s", _asctime( &tmbuf, buf ) );
}
.exmp output
It is now: Sat Mar 21 15:58:27 1987
.exmp end
.class ANSI
.system
