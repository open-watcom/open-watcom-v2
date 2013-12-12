.func _dos_gettime
.synop begin
#include <&doshdr>
void _dos_gettime( struct dostime_t *time );

struct dostime_t {
        unsigned char hour;     /* 0-23 */
        unsigned char minute;   /* 0-59 */
        unsigned char second;   /* 0-59 */
        unsigned char hsecond;  /* 1/100 second; 0-99 */
};
.ixfunc2 '&DosFunc' &func
.synop end
.desc begin
The
.id &func.
function uses system call 0x2C to get the current system time.
The time information is returned in a
.kw dostime_t
structure pointed to by
.arg time
.ct .li .
.desc end
.return begin
The
.id &func.
function has no return value.
.return end
.see begin
.seelist _dos_gettime _dos_getdate _dos_setdate _dos_settime gmtime localtime mktime time
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
  }
.exmp output
The date (MM-DD-YYYY) is: 12-25-1989
The time (HH:MM:SS) is: 14:23:57
.exmp end
.class DOS
.system
