.func jstrstr _fjstrstr
#include <jstring.h>
JSTRING jstrstr( const JCHAR *str, const JCHAR *substr );
.ixfunc2 '&Jstring' &func
.ixfunc2 '&Jsearch' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrstr( const JCHAR __far *str,
                          const JCHAR __far *substr );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jsearch' &ffunc
.do end
.funcend
.desc begin
.if &farfnc eq 0 .do begin
The &func function locates
.do end
.el .do begin
The &func and &ffunc functions locate
.do end
the first occurrence in the Kanji string pointed to by
.arg str
of the sequence of single- and double-byte characters (excluding the
terminating null character) in the Kanji string pointed to by
.arg substr
.ct .li .
.im ffarfunc
.desc end
.return begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
a pointer to the located string, or
.mono NULL
if the string is not found.
.return end
.see begin
.seelist &function. jstrcspn jstrstr strcspn strstr
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%s\n",
            jstrstr( "This is an example", "is" )
          );
  }
.exmp output
is is an example
.exmp end
.class WATCOM
.system
