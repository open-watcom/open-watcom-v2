.func memmove_s wmemmove_s
#define __STDC_WANT_LIB_EXT1__  1
#include <string.h>
errno_t memmove_s( void * restrict s1,
                   rsize_t s1max,
                   const void * restrict s2,
                   rsize_t n );
.ixfunc2 '&Copy' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
errno_t wmemmove_s( wchar_t * restrict s1,
                    rsize_t s1max,
                    const wchar_t * restrict s2,
                    size_t n );
.ixfunc2 '&Copy' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
void *_umemmove_s( void * restrict s1,
                   rsize_t s1max,
                   const void * restrict s2,
                   size_t n );
.ixfunc2 '&Copy' &ufunc
.do end
.funcend
.*
.rtconst begin
Neither
.arg s1
nor
.arg s2
shall be a null pointer. Neither
.arg s1max
nor
.arg n
shall be greater than
.arg RSIZE_MAX
.ct .li .
.arg n
shall not be greater than
.arg s1max
.ct .li .
.np
If there is a runtime-constraint violation, the &func function stores zeros in the first
.arg s1max
characters of the object pointed to by
.arg s1
if
.arg s1
is not a null pointer and
.arg s1max
is
not greater than
.arg RSIZE_MAX
.ct .li .
.rtconst end
.*
.desc begin
The &func function copies
.arg n
characters from the buffer pointed to by
.arg s2
into the buffer pointed to by
.arg s1
.ct .li .
This copying takes place as if the
.arg n
characters from the buffer
pointed to by
.arg s2
are first copied into a temporary array of
.arg n
characters that does not overlap the objects pointed to by
.arg s1
or
.arg s2
,and then the
.arg n
characters from the temporary array are copied into the object pointed to by
.arg s1
.ct .li .
.np
See the
.arg memcpy_s
function if you wish to copy objects that do not overlap.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc wide-character function is identical to &func except that it
operates on characters of
.arg wchar_t
type.
The arguments
.arg s1max
and
.arg n
are interpreted to mean the number of wide characters.
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
operates on 16-bit Unicode character strings.
The argument
.arg n
is interpreted to mean the number of Unicode characters.
.do end
.desc end
.*
.return begin
.saferet
.return end
.*
.see begin
.seelist &function. memchr memcmp memcpy memicmp memmove memset memcpy_s
.see end
.exmp begin
#define __STDC_WANT_LIB_EXT1__  1
#include <string.h>
void main( void )
{
    char buffer[80] = "0123456789";
.exmp break
    memmove_s( buffer + 1, sizeof( buffer ), buffer, 79 );
    buffer[0] = '*';
    printf( buffer );
}
.exmp output
*0123456789
.exmp end
.class TR 24731
.system
