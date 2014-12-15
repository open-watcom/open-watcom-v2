.func _dos_settime
.synop begin
#include <&doshdr>
unsigned _dos_settime( struct dostime_t *time );
struct dostime_t {
        unsigned char hour;     /* 0-23 */
        unsigned char minute;   /* 0-59 */
        unsigned char second;   /* 0-59 */
        unsigned char hsecond;  /* 1/100 second; 0-99 */
};
.ixfunc2 '&DosFunc' &funcb
.synop end
.desc begin
The
.id &funcb.
function uses system call 0x2D to set the current system time.
The time information is passed in a
.kw dostime_t
structure pointed to by
.arg time
.ct .li .
.desc end
.return begin
The
.id &funcb.
function returns zero if successful.
Otherwise, it returns a non-zero value and sets
.kw errno
to
.kw EINVAL
indicating that an invalid time was given.
.return end
.see begin
.seelist _dos_settime _dos_getdate _dos_setdate _dos_gettime gmtime localtime mktime time
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>
.exmp break
void main()
  {
    struct dosdate_t date;
    struct dostime_t time;
.exmp break
    /* Get and display the current date and time */
    _dos_getdate( &amp.date );
    _dos_gettime( &amp.time );
    printf( "The date (MM-DD-YYYY) is: %d-%d-%d\n",
        date.month, date.day, date.year );
    printf( "The time (HH:MM:SS) is: %.2d:%.2d:%.2d\n",
        time.hour, time.minute, time.second );
.exmp break
    /* Change it to the turn of the century */
    date.year = 1999;
    date.month = 12;
    date.day = 31;
    time.hour = 23;
    time.minute = 59;
    _dos_setdate( &amp.date );
    _dos_settime( &amp.time );
    printf( "New date (MM-DD-YYYY) is: %d-%d-%d\n",
                  date.month, date.day, date.year );
    printf( "New time (HH:MM:SS) is: %.2d:%.2d:%.2d\n",
                  time.hour, time.minute, time.second );
  }
.exmp output
The date (MM-DD-YYYY) is: 12-25-1989
The time (HH:MM:SS) is: 14:23:15
New date (MM-DD-YYYY) is: 12-31-1999
New time (HH:MM:SS) is: 23:59:16
.exmp end
.class DOS
.system
