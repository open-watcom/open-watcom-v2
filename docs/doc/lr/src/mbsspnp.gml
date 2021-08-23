.func _mbsspnp _fmbsspnp _strspnp _wcsspnp
.synop begin
#include <mbstring.h>
unsigned char *_mbsspnp( const unsigned char *str,
                         const unsigned char *charset );
.ixfunc2 '&String' _mbsspnp
.ixfunc2 '&Search' _mbsspnp
.ixfunc2 '&Multibyte' _mbsspnp
.if &farfnc ne 0 .do begin
unsigned char __far *_fmbsspnp(
                    const unsigned char __far *str,
                    const unsigned char __far *charset );
.ixfunc2 '&String' _fmbsspnp
.ixfunc2 '&Search' _fmbsspnp
.ixfunc2 '&Multibyte' _fmbsspnp
.do end
#include <tchar.h>
char *_strspnp( const char *str,
                const char *charset );
.ixfunc2 '&String' _strspnp
.ixfunc2 '&Search' _strspnp
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *_wcsspnp( const wchar_t *str,
                   const wchar_t *charset );
.ixfunc2 '&String' _wcsspnp
.ixfunc2 '&Search' _wcsspnp
.ixfunc2 '&Wide' _wcsspnp
.do end
.synop end
.desc begin
The
.id &funcb.
function returns a pointer to the first character in
.arg str
that does not belong to the set of characters in
.arg charset
.period
The terminating null character is not considered to be part of
.arg charset
.period
.farfunc &ffunc. &funcb.
.tcsfunc _tcsspnp &funcb. _strspnp &wfunc.
.desc end
.return begin
These functions return
.mono NULL
if
.arg str
consists entirely of characters (single-byte, wide, or multi-byte)
from
.arg charset
(single-byte, wide, or multi-byte)
.period
.return end
.see begin
.seelist strspnp strcspn strspn
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%s\n", strspnp( "out to lunch", "aeiou" ) );
    printf( "%s\n", strspnp( "out to lunch", "xyz" ) );
  }
.exmp output
t to lunch
out to lunch
.exmp end
.class WATCOM
.system
