.func _strdate _wstrdate
#include <time.h>
char *_strdate( char *datestr )
.ixfunc2 '&Conversion' &func
.if &'length(&wfunc.) ne 0 .do begin
wchar_t _wstrdate( wchar_t *datestr );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.funcend
.desc begin
The &func function copies the current date to the buffer pointed to by
.arg datestr
.ct .li .
The date is formatted as "MM/DD/YY"
where "MM" is two digits representing the month,
where "DD" is two digits representing the day, and
where "YY" is two digits representing the year.
The buffer must be at least 9 bytes long.
.im widefunc
.desc end
.return begin
The &func function returns a pointer to the resulting text string
.arg datestr
.ct .li .
.return end
.see begin
.seelist &function. asctime Functions ctime Functions gmtime localtime mktime
.seelist &function. _strdate _strtime time tzset
.see end
.exmp begin
#include <stdio.h>
#include <time.h>

void main()
  {
    char datebuff[9];
.exmp break
    printf( "%s\n", _strdate( datebuff ) );
  }
.exmp end
.class WATCOM
.system
