.func _dos_setdate
#include <&doshdr>
unsigned _dos_setdate( struct dosdate_t *date );

struct dosdate_t {
        unsigned char day;      /* 1-31 */
        unsigned char month;    /* 1-12 */
        unsigned short year;    /* 1980-2099 */
        unsigned char dayofweek;/* 0-6 (0=Sunday) */
};
.ixfunc2 '&DosFunc' &func
.funcend
.desc begin
The &func function uses system call 0x2B to set the current system date.
The date information is passed in a
.kw dosdate_t
structure pointed to by
.arg date
.ct .li .
.desc end
.return begin
The &func function returns zero if successful.
Otherwise, it returns an OS error code and sets
.kw errno
accordingly.
.return end
.see begin
.seelist _dos_setdate _dos_getdate _dos_gettime _dos_settime gmtime localtime mktime time
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
