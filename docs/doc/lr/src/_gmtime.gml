.func _gmtime
#include <time.h>
struct tm *_gmtime( const time_t *timer,
                    struct tm *tmbuf );
.ixfunc2 '&TimeFunc' &func
.im structtm
.synop end
.desc begin
The &func function converts the calendar time pointed to by
.arg timer
into a broken-down time, expressed as Coordinated Universal Time (UTC)
(formerly known as Greenwich Mean Time (GMT)).
The time information is stored in the structure pointed to by
.arg tmbuf
.ct .li .
.im tzref
.desc end
.return begin
The &func
function returns a pointer to the structure containing the broken-down
time.
.return end
.see begin
.im seetime _gmtime
.see end
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
It is now: Fri Nov 29 15:58:27 1991 GMT
.exmp end
.class WATCOM
.system
