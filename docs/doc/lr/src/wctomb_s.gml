.func begin
.funct_w  wctomb_s   TR 24731
.funct_fw _fwctomb_s
.func end
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdlib.h>
errno_t wctomb_s( int * restrict status,
                  char * restrict s,
                  rsize_t smax,
                  wchar_t wc);
.ixfunc2 '&Wide' &func
.ixfunc2 '&Multibyte' &func
.if &farfnc eq 1 .do begin
errno_t _fwctomb_s( int __far * restrict status,
                  char __far * restrict s,
                  rsize_t smax,
                  wchar_t wc);
.ixfunc2 '&Wide' &fwfunc
.ixfunc2 '&Multibyte' &fwfunc
.do end
.synop end
.*
.rtconst begin
Let
.arg n
denote the number of bytes needed to represent the multibyte character
corresponding to the wide character given by
.arg wc
(including any shift sequences).
.np
If
.arg s
is not a null pointer, then
.arg smax
shall not be less than
.arg n
.ct , and
.arg smax
shall not be greater than
.kw RSIZE_MAX
.ct .li .
If
.arg s
is a null pointer, then
.arg smax
shall equal zero.
.np
If there is a runtime-constraint violation, &func does not modify
the int pointed to by
.arg status
.ct , and if
.arg s
is not a null pointer, no more than
.arg smax
elements in the array
pointed to by
.arg s
will be accessed.
.rtconst end
.*
.desc begin
The &func. function determines
.arg n
and stores the multibyte character representation
of
.arg wc
in the array whose first element is pointed to by
.arg s
(if
.arg s
is not a null pointer). The
number of characters stored never exceeds
.kw MB_CUR_MAX
or
.arg smax
.ct .li .
If
.arg wc
is a null wide character, a null byte is stored, preceded by any shift
sequence needed to restore the initial shift state, and the function is
left in the initial conversion state.
.np
The implementation shall behave as if no library function calls the &func. function.
.np
If
.arg s
is a null pointer,the &func. function stores into the int pointed to by
.arg status
a nonzero or zero value, if multibyte character encodings, respectively,
do or do not have state-dependent encodings.
.np
If
.arg s
is not a null pointer,the &func. function stores into the int pointed to by
.arg status
either
.arg n
or
.arg -1
if
.arg wc
.ct , respectively, does or does not correspond to a valid multibyte character.
.np
In no case will the int pointed to by
.arg status
be set to a value greater than the
.kw MB_CUR_MAX
macro.
.im safefarw
.desc end
.*
.return begin
.saferet
.return end
.*
.see begin
.seelist wctomb_s wctomb mblen mbstowcs mbstowcs_s mbtowc wcstombs wcstombs_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdlib.h>

wchar_t wchar = { 0x0073 };
char    mbbuffer[3];
.exmp break
int main()
{
    int     len;
    int     status;
    errno_t rc;
.exmp break
    rc = wctomb_s( &status, NULL, 0, wchar );
    printf( "Character encodings are %sstate dependent\n",
            ( status ) ? "" : "not " );

    rc = wctomb_s( &len, mbbuffer, 2, wchar );
    if( rc != 0) {
        printf( "Character encoding error\n");
    } else {
        mbbuffer[len] = '\0';
        printf( "%s(%d)\n", mbbuffer, len );
    }
    return( rc );
}
.exmp output
Character encodings are not state dependent
s(1)
.exmp end
.classt
.system
