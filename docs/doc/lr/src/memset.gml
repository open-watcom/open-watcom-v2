.func memset _fmemset wmemset
#include <string.h>
void *memset( void *dst, int c, size_t length );
.ixfunc2 '&String' &func
.if &farfnc eq 1 .do begin
void __far *_fmemset( void __far *dst, int c,
                      size_t length );
.ixfunc2 '&String' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
wchar_t *wmemset( wchar_t *dst,
                  wchar_t c,
                  size_t length );
.ixfunc2 '&String' &wfunc
.do end
.synop end
.desc begin
The &func function fills the first
.arg length
characters of the object pointed to by
.arg dst
with the value
.arg c
.ct .li .
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
.return begin
The &func function returns the pointer
.arg dst
.ct .li .
.return end
.see begin
.seelist &function. memchr memcmp memcpy memicmp memmove memset
.see end
.exmp begin
#include <string.h>

void main( void )
{
    char buffer[80];
.exmp break
    memset( buffer, '=', 80 );
}
.exmp end
.class ANSI
.system
