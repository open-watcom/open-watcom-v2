.func atof _wtof
.synop begin
#include <stdlib.h>
double atof( const char *ptr );
.ixfunc2 '&Conversion' &func
.if &'length(&wfunc.) ne 0 .do begin
double _wtof( const wchar_t *ptr );
.ixfunc2 '&Conversion' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function converts the string pointed to by
.arg ptr
to
.id double
representation.
It is equivalent to
.millust begin
strtod( ptr, (char **)NULL )
.millust end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it
accepts a wide-character string argument.
It is equivalent to
.millust begin
wcstod( ptr, (wchar_t **)NULL )
.millust end
.do end
.desc end
.return begin
The &func function returns the converted value.
.im errnocnv
.return end
.see begin
.seelist atof sscanf strtod
.see end
.exmp begin
#include <stdlib.h>

void main()
  {
    double x;
.exmp break
    x = atof( "3.1415926" );
  }
.exmp end
.class ANSI
.system
