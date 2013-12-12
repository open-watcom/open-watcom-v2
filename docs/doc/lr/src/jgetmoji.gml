.func jgetmoji _fjgetmoji
.synop begin
#include <jstring.h>
JSTRING jgetmoji( const JCHAR *str, JMOJI *c );
.ixfunc2 '&Jstring' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjgetmoji( const JCHAR __far *str,
                           JMOJI __far *c );
.ixfunc2 '&Jstring' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The &func function places
.do end
.el .do begin
The &func and &ffunc functions place
.do end
the next single- or double-byte character from the start of the Kanji
string specified by
.arg str
in the wide character pointed to by
.arg c
.ct .li .
If the second-half of a double-byte character is NULL, then the
returned wide character is NULL.
.im ffarfunc
.desc end
.return begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
a pointer to the next character to be obtained from the string.
If
.arg str
points at a null character then
.arg str
is returned.
.return end
.see begin
.seelist &function. btom jgetmoji jputmoji
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
