.func jstrrchr _fjstrrchr
#include <jstring.h>
JSTRING jstrrchr( const JCHAR *s, JMOJI c );
.ixfunc2 '&Jstring' &func
.ixfunc2 '&Jsearch' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrrchr( const JCHAR __far *s,
                           JMOJI c );
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
the last occurrence of the single- or double-byte character
.arg c
in the Kanji string pointed to by
.arg s
.ct .li .
The terminating null character is considered to be part of the string.
.im ffarfunc
.desc end
.return begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
a pointer to the located character,
or a NULL pointer if the character does not occur in the Kanji string.
.return end
.see begin
.seelist &function. jstrchr jstrpbrk jstrrchr strchr strpbrk strrchr
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    printf( "%s\n", jstrrchr( "abcdeabcde", 'a' ) );
    if( jstrrchr( "abcdeabcde", 'x' ) == NULL )
        printf( "NULL\n" );
  }
.exmp output
abcde
NULL
.exmp end
.class WATCOM
.system
