.func jstrtok _fjstrtok
#include <jstring.h>
JSTRING jstrtok( JCHAR *s1, const JCHAR *s2 );
.ixfunc2 '&Jstring' &func
.ixfunc2 '&Jsearch' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjstrtok( JCHAR __far *s1,
                          const JCHAR __far *s2 );
.ixfunc2 '&Jstring' &ffunc
.ixfunc2 '&Jsearch' &ffunc
.do end
.funcend
.desc begin
.if &farfnc eq 0 .do begin
The &func function is
.do end
.el .do begin
The &func and &ffunc functions are
.do end
used to break the Kanji string pointed to by
.arg s1
into a sequence of tokens, each of which is delimited by a single- or
double-byte character from the string pointed to by
.arg s2
.ct .li .
The first call to &func will return a pointer to the first token in
the Kanji string pointed to by
.arg s1
.ct .li .
Subsequent calls to &func must pass a NULL pointer as the first
argument, in order to get the next token in the Kanji string.
The set of delimiters used in each of these calls to &func can be
different from one call to the next.
.np
The first call in the sequence searches
.arg s1
for the first single- or double-byte character that is not contained
in the current delimiter string
.arg s2
.ct .li .
If no such character is found, then there are no tokens in
.arg s1
and the &func function returns a NULL pointer.
If such a character is found, it is the start of the first token.
.np
The &func function then searches from there for a single- or
double-byte character that is contained in the current delimiter
If no such character is found, the current token extends to the end of
the string pointed to by
.arg s1
.ct .li .
If such a character is found, it is overwritten by a null character,
which terminates the current token.
The &func function saves a pointer to the following character, from
which the next search for a token will start when the first argument
is a NULL pointer.
.np
Because &func may modify the original string, that string should be
duplicated if the string is to be re-used.
.im ffarfunc
.desc end
.return begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
a pointer to the first character of a token or
.mono NULL
if there is no token found.
.return end
.see begin
.seelist &function. jstrtok jstrcspn jstrpbrk strtok strcspn strpbrk
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <jstring.h>

void main()
  {
    JSTRING p;
    JSTRING buffer;
    JSTRING delims = { " .," };

    buffer = strdup( "Find words, all of them." );
    printf( "%s\n", buffer );
    p = jstrtok( buffer, delims );
    while( p != NULL ) {
      printf( "word: %s\n", p );
      p = jstrtok( NULL, delims );
    }
    printf( "%s\n", buffer );
  }
.exmp output
Find words, all of them.
word: Find
word: words
word: all
word: of
word: them
Find
.exmp end
.class WATCOM
.system
