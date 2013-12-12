.func jputmoji _fjputmoji
.synop begin
#include <jstring.h>
JSTRING jputmoji( JCHAR *str, JMOJI c );
.ixfunc2 '&Jstring' &func
.if &farfnc eq 1 .do begin
FJSTRING __far _fjputmoji( JCHAR __far *str, JMOJI c );
.ixfunc2 '&Jstring' _f&func
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The &func function places
.do end
.el .do begin
The &func and &ffunc functions place
.do end
the next single- or double-byte character specified by
.arg c
at the start of the buffer specified by
.arg str
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
a pointer to the next location in which to store a character.
.return end
.see begin
.seelist &function. btom jgetmoji jputmoji
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
