.func btom _fbtom
.synop begin
#include <jstring.h>
size_t btom( const unsigned char *str, size_t nb );
.ixfunc2 '&Jstring' &func
.if &farfnc eq 1 .do begin
size_t far _fbtom( const unsigned char far *str, size_t nb );
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
the number of characters between the start of the Japanese double-byte
character set (DBCS) string specified by
.arg str
and
.arg nb
bytes from
.arg str
.ct .li .
A double-byte character is treated as one character.
Counting stops when either
.arg nb
bytes have been processed or a null character is encountered.
.im ffarparm
.desc end
.return begin
.if &farfnc eq 0 .do begin
The &func function returns
.do end
.el .do begin
The &func and &ffunc functions return
.do end
the number of single- and double-byte characters in the string.
.return end
.see begin
.seelist btom jgetmoji mtob
.see end
.exmp begin
#include <stdio.h>
#include <jstring.h>

const JCHAR date_str[] = {
    "YYYY \x94\x4e MM \x8c\x8e DD \x93\xfa"
    " \x81\x69WW\x81\x6a HH:MM:SS"
};
.exmp break
void main()
  {
    printf( "%d characters found\n",
            btom( date_str, 35 ) );
  }
.exmp output
30
.exmp end
.class WATCOM
.system
