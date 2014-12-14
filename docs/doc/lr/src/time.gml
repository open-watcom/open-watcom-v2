.func time
.synop begin
#include <time.h>
time_t time( time_t *tloc );
.ixfunc2 '&TimeFunc' &func
.synop end
.desc begin
The
.id &func.
function determines the current calendar time and encodes it
into the type
.kw time_t
.ct .li .
.np
The time represents the time since January 1, 1970 Coordinated
Universal Time (UTC) (formerly known as Greenwich Mean Time (GMT)).
.im tzref
.desc end
.return begin
The
.id &func.
function returns the current calendar time.
If
.arg tloc
is not
.mono NULL,
the current calendar time is also stored in the object pointed to by
.arg tloc
.ct .li .
.return end
.see begin
.im seetime
.see end
.exmp begin
#include <stdio.h>
#include <time.h>

void main()
  {
    time_t time_of_day;
.exmp break
    time_of_day = time( NULL );
    printf( "It is now: %s", ctime( &time_of_day ) );
  }
.exmp output
It is now: Fri Dec 25 15:58:42 1987
.exmp end
.class ANSI, POSIX 1003.1
.system
