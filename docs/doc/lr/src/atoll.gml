.func atoll _wtoll
#include <stdlib.h>
long long int atoll( const char *ptr );
.ixfunc2 '&Conversion' &func
.if &'length(&wfunc.) ne 0 .do begin
long long int _wtoll( const wchar_t *ptr );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.funcend
.desc begin
The &func function converts the string pointed to by
.arg ptr
to
.id long long int
representation.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it
accepts a wide-character string argument.
.do end
.desc end
.return begin
The &func function returns the converted value.
.return end
.see begin
.im seestoi &function.
.see end
.exmp begin
#include <stdlib.h>

void main()
{
    long int x;
.exmp break
    x = atoll( "-289356768201" );
}
.exmp end
.class ANSI
.system
