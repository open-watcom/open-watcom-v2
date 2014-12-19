.func begin strftime
.func2 wcsftime ISO C95
.func2 _wstrftime_ms
.func end
.synop begin
#include <time.h>
size_t strftime( char *s,
                 size_t maxsize,
                 const char *format,
                 const struct tm *timeptr );
.ixfunc2 '&TimeFunc' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
size_t wcsftime( wchar_t *s,
                 size_t maxsize,
                 const wchar_t *format,
                 const struct tm *timeptr );
.ixfunc2 '&TimeFunc' &wfunc
.ixfunc2 '&Wide' &wfunc
#include <time.h>
size_t _wstrftime_ms( wchar_t *s,
                      size_t maxsize,
                      const char *format,
                      const struct tm *timeptr );
.ixfunc2 '&TimeFunc' _wstrftime_ms
.ixfunc2 '&Wide' _wstrftime_ms
.do end
.im structtm
.synop end
.desc begin
The
.id &funcb.
function formats the time in the argument
.arg timeptr
into the array pointed to by the argument
.arg s
according to the
.arg format
argument.
.im widefunc
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.kw _wstrftime_ms
function is identical to
.id &wfunc.
except that the
.arg format
is not a wide-character string.
.do end
.np
The
.arg format
string consists of zero or more directives and ordinary characters.
A directive consists of a '%' character followed by a character
that determines the substitution that is to take place.
All ordinary characters are copied unchanged into the array.
No more than
.arg maxsize
characters are placed in the array.
The format directives %D, %h, %n, %r, %t, and %T are from POSIX.
.begnote
.termhd1 Directive
.termhd2 Meaning
.note %a
locale's abbreviated weekday name
.note %A
locale's full weekday name
.note %b
locale's abbreviated month name
.note %B
locale's full month name
.note %c
locale's appropriate date and time representation
.note %C
is replaced by the year devided by 100 and truncated to an integer (00-99)
.note %d
day of the month as a decimal number (01-31)
.note %D
date in the format mm/dd/yy (POSIX)
.note %e
day of the month as a decimal number ( 1-31), a single digit is preceded by a blank
.note %F
is equivalent to '%Y-%m-%d' (the ISO 8601 date format)
.note %g
is replaced by the last 2 digits of the week-based year as a decimal number (00-99)
.note %G
is replaced by the week-based year as a decimal number (e.g. 2006)
.note %h
locale's abbreviated month name (POSIX)
.note %H
hour (24-hour clock) as a decimal number (00-23)
.note %I
hour (12-hour clock) as a decimal number (01-12)
.note %j
day of the year as a decimal number (001-366)
.note %m
month as a decimal number (01-12)
.note %M
minute as a decimal number (00-59)
.note %n
newline character (POSIX)
.note %p
locale's equivalent of either AM or PM
.note %r
12-hour clock time (01-12) using the AM/PM notation in the format
HH:MM:SS (AM|PM) (POSIX)
.note %S
second as a decimal number (00-59)
.note %t
tab character (POSIX)
.note %T
24-hour clock time in the format HH:MM:SS (POSIX)
.note %u
is replaced by the ISO 8601 weekday as a decimal number (1-7), where Monday is 1
.note %U
week number of the year as a decimal number (00-52) where Sunday
is the first day of the week
.note %V
is replaced by the ISO 8601 week number as a decimal number (01-53)
.note %w
weekday as a decimal number (0-6) where 0 is Sunday
.note %W
week number of the year as a decimal number (00-52) where Monday
is the first day of the week
.note %x
locale's appropriate date representation
.note %X
locale's appropriate time representation
.note %y
year without century as a decimal number (00-99)
.note %Y
year with century as a decimal number
.note %z
offset from UTC in the ISO 8601 format '-0430' (meaning 4 hours 30 minutes behind
UTC, west of Greenwich), or by no characters, if no timezone is determinable
.note %Z
timezone name, or by no characters if no timezone exists
.note %%
character %
.endnote
.np
When the
.mono %Z
or
.mono %z
directive is specified, the
.kw tzset
function is called.
.np
% g, %G, %V give values according to the ISO 8601 week-based year. In this
system, weeks begin on a monday and week 1 of the year is the week that
includes January 4th, which is also the week that includes the first Thursday
of the year, and is also the first week that contains at least four days in
the year. If the first Monday of January is the 2nd, 3rd, or 4th, the preceding
days are part of the last week of the preceding year; thus, for Saturday
2nd January 1999, %G is replaced by 1998 and %V is replaced by 53.
If december 29th, 30th, or 31st is a Monday, it and any following days are
part of week 1 of the following year. Thus, for Tuesday 30th December 1997,
%G is replaced by 1998 and %V is replaced by 01.
.np
The format modifiers E and O are ignored. (eg. %EY is the same as %Y)
.desc end
.return begin
If the number of characters to be placed into the array is less than
.arg maxsize
.ct , the
.id &funcb.
function returns the number of characters placed into the
array pointed to by
.arg s
not including the terminating null character.
Otherwise, zero is returned.
.im errnoref
.return end
.see begin
.seelist strftime setlocale
.im seetime
.see end
.exmp begin
#include <stdio.h>
#include <time.h>

void main()
  {
    time_t time_of_day;
    char buffer[ 80 ];
.exmp break
    time_of_day = time( NULL );
    strftime( buffer, 80, "Today is %A %B %d, %Y",
               localtime( &time_of_day ) );
    printf( "%s\n", buffer );
  }
.exmp output
Today is Friday December 25, 1987
.exmp end
.class ISO C, POSIX
.system
