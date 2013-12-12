.func ftime
.synop begin
#include <sys/timeb.h>
int ftime( struct timeb *timeptr );

struct timeb {
 time_t time;  /* time in seconds since Jan 1, 1970 UTC */
 unsigned short millitm; /* milliseconds */
 short timezone; /* difference in minutes from UTC */
 short dstflag;  /* nonzero if in daylight savings time */
};
.ixfunc2 '&TimeFunc' &func
.synop end
.desc begin
The
.id &func.
function gets the current time and stores it in the
structure pointed to by
.arg timeptr
.ct .li .
.desc end
.return begin
The
.id &func.
function fills in the fields of the structure pointed to by
.arg timeptr
.ct .li .
The
.id &func.
function returns &minus.1 if not successful, and no useful
value otherwise.
.return end
.see begin
.im seetime ftime
.see end
.exmp begin
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
.exmp break
void main()
  {
    struct timeb timebuf;
    char   *tod;
.exmp break
    ftime( &timebuf );
    tod = ctime( &timebuf.time );
    printf( "The time is %.19s.%hu %s",
        tod, timebuf.millitm, &tod[20] );
  }
.exmp output
The time is Tue Dec 25 15:58:42.870 1990
.exmp end
.class WATCOM
.system
