.func memmove _fmemmove wmemmove
#include <string.h>
void *memmove( void *dst,
               const void *src,
               size_t length );
.ixfunc2 '&Copy' &func
.if &farfnc eq 1 .do begin
void __far *_fmemmove( void __far *dst,
                       const void __far *src,
                       size_t length );
.ixfunc2 '&Copy' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *wmemmove( wchar_t *dst,
                   const wchar_t *src,
                   size_t length );
.ixfunc2 '&Copy' &wfunc
.do end
.funcend
.*
.safealt
.*
.desc begin
The &func function copies
.arg length
characters from the buffer pointed to by
.arg src
to the buffer pointed to by
.arg dst
.ct .li .
Copying of overlapping objects will take place properly.
See the
.kw memcpy
function to copy objects that do not overlap.
.im farfunc
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc wide-character function is identical to &func except that it
operates on characters of
.kw wchar_t
type.
The argument
.arg length
is interpreted to mean the number of wide characters.
.do end
.desc end
.*
.return begin
The &func function returns
.arg dst
.ct .li .
.return end
.*
.see begin
.seelist &function. memchr memcmp memcpy memicmp memmove memset memmove_s memcpy_s
.see end
.*
.exmp begin
#include <string.h>

void main( void )
{
    char buffer[80];
.exmp break
    memmove( buffer + 1, buffer, 79 );
    buffer[0] = '*';
}
.exmp end
.class ANSI
.system
