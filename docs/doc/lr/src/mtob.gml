.func mtob _fmtob
#include <jstring.h>
size_t mtob( const unsigned char *str, size_t nm );
.ixfunc2 '&Jstring' &func
.if &farfnc eq 1 .do begin
size_t far _fmtob( const unsigned char far *str, size_t nm );
.ixfunc2 '&Jstring' &ffunc
.do end
.synop end
.desc begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
the number of bytes between the start of the Kanji string specified by
.arg str
and
.arg nm
characters from
.arg str
.ct .li .
A double-byte Kanji character is treated as one character.
Processing stops when either
.arg nm
characters have been processed or a null character is encountered.
.im ffarparm
.desc end
.return begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
the number of bytes in the string up to the specified number of
characters.
.return end
.see begin
.seelist mtob btom jgetmoji
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

const JCHAR date_str[] = {
    "YYYY \x94\x4e MM \x8c\x8e DD \x93\xfa"
    " \x81\x69WW\x81\x6a HH:MM:SS"
};

void main()
  {
    printf( "%d bytes found\n",
            mtob( date_str, 30 ) );
  }
.exmp output
35
.exmp end
.class WATCOM
.system
