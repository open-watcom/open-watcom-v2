.func setlocale _wsetlocale
.synop begin
#include <locale.h>
char *setlocale( int category, const char *locale );
.ixfunc2 '&Locale' &func
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wsetlocale( int category, const wchar_t *locale);
.ixfunc2 '&Locale' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &func.
function selects a portion of a program's
.us locale
according to the category given by
.arg category
and the locale specified by
.arg locale
.ct .li .
A
.us locale
affects the collating sequence (the order in which characters compare
with one another), the way in which certain character-handling functions
operate, the decimal-point character that is used in formatted
input/output and string conversion, and the format and names used in
the time string produced by the
.kw strftime
function.
.np
Potentially, there may be many such environments.
&company C/C++ supports only the
.mono "C"
locale and so invoking this function will have no effect upon the
behavior of a program at present.
.np
The possible values for the argument
.arg category
are as follows:
.begterm 15
.termhd1 Category
.termhd2 Meaning
.term .mono LC_ALL
select entire environment
.term .mono LC_COLLATE
select collating sequence
.term .mono LC_CTYPE
select the character-handling
.if '&machsys' eq 'QNX' .do begin
.term .mono LC_MESSAGES
.do end
.term .mono LC_MONETARY
select monetary formatting information
.term .mono LC_NUMERIC
select the numeric-format environment
.term .mono LC_TIME
select the time-related environment
.endterm
.np
At the start of a program, the equivalent of the following statement
is executed.
.blkcode begin
    setlocale( LC_ALL, "C" );
.blkcode end
.im widefunc
.desc end
.return begin
If the selection is successful, a string is returned to indicate
the locale that was in effect before the function was invoked;
otherwise, a
.mono NULL
pointer is returned.
.return end
.see begin
.seelist setlocale strcoll strftime strxfrm
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <locale.h>

char src[] = { "A sample STRING" };
char dst[20];
.exmp break
void main()
  {
    char *prev_locale;
    size_t len;
.exmp break
    /* set native locale */
    prev_locale = setlocale( LC_ALL, "" );
    printf( "%s\n", prev_locale );
    len = strxfrm( dst, src, 20 );
    printf( "%s (%u)\n", dst, len );
  }
.exmp output
C
A sample STRING (15)
.exmp end
.class ANSI, POSIX 1003.1
.system
