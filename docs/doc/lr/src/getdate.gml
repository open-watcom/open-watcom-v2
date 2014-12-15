.func _dos_getdate
.synop begin
#include <&doshdr>
void _dos_getdate( struct dosdate_t *date );

struct dosdate_t {
        unsigned char day;      /* 1-31 */
        unsigned char month;    /* 1-12 */
        unsigned short year;    /* 1980-2099 */
        unsigned char dayofweek;/* 0-6 (0=Sunday) */
};
.ixfunc2 '&DosFunc' &funcb
.synop end
.desc begin
The
.id &funcb.
function uses system call 0x2A to get the current system date.
The date information is returned in a
.kw dosdate_t
structure pointed to by
.arg date
.ct .li .
.desc end
.return begin
The
.id &funcb.
function has no return value.
.return end
.see begin
.seelist _dos_getdate _dos_gettime _dos_setdate _dos_settime gmtime localtime mktime time
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
