.func jgetmoji _fjgetmoji
.synop begin
#include <jstring.h>
JSTRING jgetmoji( const JCHAR *s, JMOJI *c );
.ixfunc2 '&Jstring' &funcb
.if &farfnc ne 0 .do begin
FJSTRING _fjgetmoji( const JCHAR __far *s, JMOJI __far *c );
.ixfunc2 '&Jstring' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The
.id &funcb.
function places
.do end
.el .do begin
The
.id &funcb.
and
.id &ffunc.
functions place
.do end
the next single-byte or double-byte character from the start of the Kanji
string specified by
.arg s
in the wide character pointed to by
.arg c
.period
If the second-half of a double-byte character is NULL, then the
returned wide character is NULL.
.farfunc &ffunc. &funcb.
.desc end
.return begin
.if &farfnc eq 0 .do begin
The
.id &funcb.
function returns
.do end
.el .do begin
The
.id &funcb.
and
.id &ffunc.
functions return
.do end
a pointer to the next character to be obtained from the string.
If
.arg s
points at a null character then
.arg s
is returned.
.return end
.see begin
.seelist btom jgetmoji jputmoji
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

const JCHAR set[] = {
    "ab\x81\x41\x81\x42\cd\x81"
};
.exmp break
void main()
  {
    JMOJI c;
    const JCHAR *str;
.exmp break
    str = set;
    for( ; *str != '\0'; ) {
        str = jgetmoji( str, &c );
        printf( "Character code 0x%2.2x\n", c );
    }
  }
.exmp output
Character code 0x61
Character code 0x62
Character code 0x8141
Character code 0x8142
Character code 0x63
Character code 0x64
Character code 0x00
.exmp end
.class WATCOM
.system
