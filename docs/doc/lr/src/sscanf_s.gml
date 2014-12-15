.func begin sscanf_s swscanf_s
.funcw swscanf_s
.func end
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
int sscanf_s( const char * restrict s,
              const char * restrict format, ... );
.ixfunc2 '&StrIo' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int swscanf_s( const wchar_t * restrict s,
               const wchar_t * restrict format, ... );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst begin
Neither
.arg s
not
.arg format
shall be a null pointer.
Any argument indirected through in order to store converted input shall
not be a null pointer.
.np
If there is a runtime-constraint violation, the
.id &funcb.
function does not
attempt to perform further input, and it is unspecified to what extent
.id &funcb.
performed input before discovering the runtime-constraint violation.
.rtconst end
.*
.desc begin
The
.id &funcb.
function is equivalent to
.kw fscanf_s
.ct , except that input is obtained from a string (specified by the argument
.arg s
.ct ) rather than from a stream. Reaching the end of the string is equivalent
to encountering end-of-file for the
.kw fscanf_s
function. If copying takes place between objects that overlap, the objects
take on unspecified values.
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it accepts
wide-character string arguments for
.arg s
and
.arg format
.ct .li .
.do end
.desc end
.*
.return begin
The
.id &funcb.
function returns
.kw EOF
if an input failure occurred before any conversion or if there was
a runtime-constraint violation.
Otherwise, the
.id &funcb.
function returns the number of input items
successfully assigned, which can be fewer than provided for, or even zero.
.return end
.*
.see begin
.im seevscnf
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
.exmp break
void main( void )
{
    int day, year;
    char weekday[10], month[10];
.exmp break
    sscanf_s( "Friday August 0013 2004",
            "%s %s %d  %d",
             weekday, sizeof( weekday ),
             month, sizeof( month ),
             &day, &year );
    printf_s( "%s %s %d %d\n",
             weekday, month, day, year );
}
.exmp output
Friday August 13 2004
.exmp end
.*
.class TR 24731
.system
