.func atol _wtol
.synop begin
#include <stdlib.h>
long int atol( const char *ptr );
.ixfunc2 '&Conversion' &funcb
.if &'length(&wfunc.) ne 0 .do begin
long int _wtol( const wchar_t *ptr );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function converts the string pointed to by
.arg ptr
to
.id long int
representation.
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it
accepts a wide-character string argument.
.do end
.desc end
.return begin
The
.id &funcb.
function returns the converted value.
.return end
.see begin
.im seestoi
.see end
.exmp begin
#include <stdlib.h>

void main()
{
    long int x;
.exmp break
    x = atol( "-289" );
}
.exmp end
.class ANSI
.system
