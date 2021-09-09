.func _strnset _fstrnset _wcsnset _mbsnset _fmbsnset strnset wcsnset
.ansiname _strnset
.synop begin
#include <string.h>
char *_strnset( char *s, int fill, size_t count );
.ixfunc2 '&String' _strnset
.if &farfnc ne 0 .do begin
char __far *_fstrnset( char __far *s,
                       int fill,
                       size_t count );
.ixfunc2 '&String' _fstrnset
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *_wcsnset( wchar_t *s, int fill, size_t count );
.ixfunc2 '&String' _wcsnset
.ixfunc2 '&Wide' _wcsnset
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsnset( unsigned char *s,
                         unsigned int fill,
                         size_t count );
.ixfunc2 '&String' _mbsnset
.ixfunc2 '&Multibyte' _mbsnset
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsnset( unsigned char __far *s,
                                unsigned int fill,
                                size_t count );
.ixfunc2 '&String' _fmbsnset
.ixfunc2 '&Multibyte' _fmbsnset
.do end

.deprec
char *strnset( char *s, int fill, size_t count );
.ixfunc2 '&String' strnset
wchar_t *wcsnset( wchar_t *s, int fill, size_t count );
.ixfunc2 '&String' wcsnset
.ixfunc2 '&Wide' wcsnset
.synop end
.desc begin
The
.id &funcb.
function fills the string
.arg s
with the value of the argument
.arg fill
.ct , converted to be a character value.
When the value of
.arg count
is greater than the length of the string, the entire string is filled.
Otherwise, that number of characters at the start of the string are set
to the fill character.
.farfunc &ffunc. &funcb.
.widefunc &wfunc. &funcb.
For &wfunc, the value of
.arg count
is the number of wide characters to fill.
This is half the number of bytes.
.mbcsfunc &mfunc. &funcb.
.farfunc &fmfunc. &mfunc.
.np
For &mfunc, the value of
.arg count
is the number of multibyte characters to fill.
If the number of bytes to be filled is odd and
.arg fill
is a double-byte character, the partial byte at the end is filled with
an ASCII space character.
.np
.deprfunc strnset _strnset
.desc end
.return begin
The address of the original string
.arg s
is returned.
.return end
.see begin
.seelist _strset
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

char source[] = { "A sample STRING" };

void main()
  {
    printf( "%s\n", source );
    printf( "%s\n", _strnset( source, '=', 100 ) );
    printf( "%s\n", _strnset( source, '*', 7 ) );
  }
.exmp output
A sample STRING
===============
*******========
.exmp end
.class WATCOM
.system
