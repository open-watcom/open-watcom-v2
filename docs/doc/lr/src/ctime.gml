.func begin ctime Functions
.func2 ctime
.func2 _ctime
.func2 _wctime
.func2 __wctime
.func2 _uctime
.func2 _u_ctime
.func gen
#include <time.h>
char * ctime( const time_t *timer );
char *_ctime( const time_t *timer, char *buf );
.ixfunc2 '&TimeFunc' ctime
.ixfunc2 '&TimeFunc' _ctime
.if &'length(&wfunc.) ne 0 .do begin
wchar_t * _wctime( const time_t *timer );
wchar_t *__wctime( const time_t *timer, wchar_t *buf );
.ixfunc2 '&TimeFunc' _wctime
.ixfunc2 '&Wide' _wctime
.ixfunc2 '&TimeFunc' __wctime
.ixfunc2 '&Wide' __wctime
.do end
.if &'length(&ufunc.) ne 0 .do begin
wchar_t * _uctime( const time_t *timer );
wchar_t *_u_ctime( const time_t *timer, wchar_t *buf );
.ixfunc2 '&TimeFunc' _uctime
.ixfunc2 '&TimeFunc' _u_ctime
.do end
.func end
.*
.safealt
.*
.funcbold ctime
.desc begin
The &func functions convert the calendar time pointed to by
.arg timer
to local time in the form of a string.
The &func function is equivalent to
.millust begin
asctime( localtime( timer ) )
.millust end
.np
The &func functions convert the time into a string containing exactly
26 characters.
This string has the form shown in the following example:
.millust begin
Sat Mar 21 15:58:27 1987\n\0
.millust end
.pc
All fields have a constant width.
The new-line character
.id '\n'
and the null character
.id '\0'
occupy the last two positions of the string.
.np
The ANSI function &func places the result string in a static buffer
that is re-used each time &func or
.kw asctime
is called.
The non-ANSI function
.kw _ctime
places the result string in the buffer pointed to by
.arg buf
.ct .li .
.if &'length(&wfunc.) ne 0 .do begin
.np
The wide-character function
.kw _wctime
is identical to &func except that it produces a wide-character string
(which is twice as long).
The wide-character function
.kw __wctime
is identical to
.kw _ctime
except that it produces a wide-character string (which is twice as
long).
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The Unicode function
.kw _uctime
is identical to &func except that it produces a Unicode string (which
is twice as long).
The Unicode function
.kw _u_ctime
is identical to
.kw _ctime
except that it produces a Unicode string (which is twice as long).
.do end
.np
Whenever the &func functions are called, the
.kw tzset
function is also called.
.np
The calendar time is usually obtained by using the
.kw time
function.
That time is Coordinated Universal Time (UTC) (formerly known as
Greenwich Mean Time (GMT)).
.im tzref
.desc end
.return begin
The &func functions return the pointer to the string containing the
local time.
.return end
.see begin
.im seetime ctime
.seelist ctime_s
.see end
.exmp begin
#include <stdio.h>
#include <time.h>
.exmp break
void main()
{
    time_t time_of_day;
    auto char buf[26];
.exmp break
    time_of_day = time( NULL );
    printf( "It is now: %s", _ctime( &time_of_day, buf ) );
}
.exmp output
It is now: Fri Dec 25 15:58:42 1987
.exmp end
.* The following line forces an entry out for the function
.sr mfunc='x __wctime'
.class ANSI
.system
