.func jputmoji _fjputmoji
.synop begin
#include <jstring.h>
JSTRING jputmoji( JCHAR *s, JMOJI c );
.ixfunc2 '&Jstring' &funcb
.if &farfnc ne 0 .do begin
FJSTRING _fjputmoji( JCHAR __far *s, JMOJI c );
.ixfunc2 '&Jstring' _f&funcb
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
the next single-byte or double-byte character specified by
.arg c
at the start of the buffer specified by
.arg s
.period
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
a pointer to the next location in which to store a character.
.return end
.see begin
.seelist btom jgetmoji jputmoji
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

void main()
  {
    JMOJI c;
    JCHAR *str1;
    JCHAR *str2;
    JCHAR buf[30];
.exmp break
    str1 = "abCDef";
    str2 = buf;

    for( ; *str1 != '\0'; ) {
        str1 = jgetmoji( str1, &c );
        str2 = jputmoji( str2, '<' );
        str2 = jputmoji( str2, c );
        str2 = jputmoji( str2, '>' );
    }
    *str2 = '\0';
    printf( "%s\n", buf );
  }
.exmp output
<a><b><C><D><e><f>
.exmp end
.class WATCOM
.system
