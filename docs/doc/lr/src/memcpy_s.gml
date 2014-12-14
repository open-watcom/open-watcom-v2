.func memcpy_s wmemcpy_s
.synop begin
#define __STDC_WANT_LIB_EXT1__  1
#include <string.h>
errno_t memcpy_s( void * restrict s1,
                  rsize_t s1max,
                  const void * restrict s2,
                  rsize_t n );
.ixfunc2 '&Copy' &func
.if &'length(&ffunc.) ne 0 .do begin
errno_t _fmemcpy_s( void __far * restrict s1,
                    rsize_t s1max,
                    const void __far * restrict s2,
                    size_t n );
.ixfunc2 '&Copy' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
errno_t wmemcpy_s( wchar_t * restrict s1,
                   rsize_t s1max,
                   const wchar_t * restrict s2,
                   size_t n );
.ixfunc2 '&Copy' &wfunc
.do end
.synop end
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
.kw RSIZE_MAX
.ct .li .
.arg n
shall not be greater than
.arg s1max
.ct .li .
. Copying shall not take place between
objects that overlap.
.np
If there is a runtime-constraint violation, the
.id &func.
function stores zeros in the first
.arg s1max
characters of the object pointed to by
.arg s1
if
.arg s1
is not a null pointer and
.arg s1max
is
not greater than
.kw RSIZE_MAX
.ct .li .
.rtconst end
.*
.desc begin
The
.id &func.
function copies
.arg n
characters from the buffer pointed to by
.arg s2
into the buffer pointed to by
.arg s1
.ct .li .
Copying between overlapping objects is not allowed.
See the
.kw memmove_s
function if you wish to copy objects that overlap.
.if &'length(&_func.) ne 0 .do begin
.im farfunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
wide-character function is identical to
.id &func.
except that it
operates on characters of
.kw wchar_t
type.
The arguments
.arg s1max
and
.arg n
are interpreted to mean the number of wide characters.
.do end
.desc end
.*
.return begin
.saferet
.return end
.*
.see begin
.seelist memcpy memchr memcmp memcpy memicmp memmove memset memmove_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <string.h>

void main( void )
{
    char buffer[80];
.exmp break
    memcpy_s( buffer, sizeof( buffer ), "Hello", 5 );
    buffer[5] = '\0';
    printf( "%s\n", buffer );
}
.exmp end
.*
.class TR 24731
.system
