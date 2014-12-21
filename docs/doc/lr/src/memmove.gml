.func begin memmove _fmemmove
.func2 wmemmove ISO C95
.func end
.synop begin
#include <string.h>
void *memmove( void *dst,
               const void *src,
               size_t length );
.ixfunc2 '&Copy' &funcb
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
.synop end
.*
.safealt
.*
.desc begin
The
.id &funcb.
function copies
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
.im widefun1
.if &'length(&wfunc.) ne 0 .do begin
The argument
.arg length
is interpreted to mean the number of wide characters.
.do end
.desc end
.*
.return begin
The
.id &funcb.
function returns
.arg dst
.ct .li .
.return end
.*
.see begin
.seelist memchr memcmp memcpy memicmp memmove memset memmove_s memcpy_s
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
.class ISO C
.system
