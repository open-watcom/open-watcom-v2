.func atoi _wtoi
#include <stdlib.h>
int atoi( const char *ptr );
.ixfunc2 '&Conversion' &func
.if &'length(&wfunc.) ne 0 .do begin
int _wtoi( const wchar_t *ptr );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function converts the string pointed to by
.arg ptr
to
.id int
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
    int x;
.exmp break
    x = atoi( "-289" );
}
.exmp end
.class ANSI
.system
