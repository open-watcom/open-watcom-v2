.func memchr _fmemchr wmemchr _umemchr
#include <string.h>
void *memchr( const void *buf, int ch, size_t length );
.ixfunc2 '&Search' &func
.if &farfnc eq 1 .do begin
void __far *_fmemchr( const void __far *buf,
                      int ch,
                      size_t length );
.ixfunc2 '&Search' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *wmemchr( const wchar_t *buf, wchar_t ch, size_t length );
.ixfunc2 '&Search' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
void *_umemchr( const void *buf, int ch, size_t length );
.ixfunc2 '&Search' &ufunc
.do end
.funcend
.desc begin
The &func function locates the first occurrence of
.arg ch
(converted to an unsigned char) in the first
.arg length
characters of the object pointed to by
.arg buf
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
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
operates on 16-bit Unicode characters.
The argument
.arg length
is interpreted to mean the number of Unicode characters.
.do end
.desc end
.return begin
The &func function returns a pointer to the located character, or
.mono NULL
if the character does not occur in the object.
.return end
.see begin
.seelist &function. memchr memcmp memcpy memicmp memset
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main( void )
{
    char buffer[80];
    char *where;
.exmp break
    strcpy( buffer, "video x-rays" );
    where = (char *)memchr( buffer, 'x', 6 );
    if( where == NULL )
        printf( "'x' not found\n" );
    else
        printf( "%s\n", where );
    where = (char *)memchr( buffer, 'r', 9 );
    if( where == NULL )
        printf( "'r' not found\n" );
    else
        printf( "%s\n", where );
}
.exmp end
.class ANSI
.system
