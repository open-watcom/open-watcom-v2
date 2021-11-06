.func btom _fbtom
.synop begin
.deprec
#include <jstring.h>
size_t btom( const unsigned char *s, size_t nb );
.ixfunc2 '&Jstring' btom
.if &farfnc ne 0 .do begin
size_t _fbtom( const unsigned char far *s, size_t nb );
.ixfunc2 '&Jstring' _fbtom
.do end
.synop end
.desc begin
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
the number of characters between the start of the Japanese double-byte
character set (DBCS) string specified by
.arg s
and
.arg nb
bytes from
.arg s
.period
A double-byte character is treated as one character.
Counting stops when either
.arg nb
bytes have been processed or a null character is encountered.
.farfuncp &ffunc. &funcb.
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
the number of single-byte and double-byte characters in the string.
.return end
.see begin
.seelist jgetmoji mtob
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
