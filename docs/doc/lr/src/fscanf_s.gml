.func begin fscanf_s fwscanf_s
.funcw fwscanf_s
.func end
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
int fscanf_s( FILE * restrict stream,
        const char * restrict format, ... );
.ixfunc2 '&StrIo' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <stdio.h>
#include <wchar.h>
int fwscanf_s( FILE * restrict stream, 
      const wchar_t * restrict format, ... );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst begin
Neither
.arg stream
nor
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
.kw fscanf
except that the
.mono c,
.mono s,
and
.mono [
conversion specifiers apply to a pair of arguments (unless assignment
suppression is indicated by a
.mono *
.ct ).
The first of these arguments is the same as for
.kw fscanf
.ct .li .
That argument is immediately followed in the argument list by the second
argument, which has type
.kw size_t
and gives the number of elements in the array pointed to by the first
argument of the pair. If the first argument points to a scalar object, it is
considered to be an array of one element.
.np
A matching failure occurs if the number of elements in a receiving object is
insufficient to hold the converted input (including any trailing null
character).
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it accepts a
wide-character string argument for
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
.np
When a file input error occurs, the
.kw errno
global variable may be set.
.return end
.*
.see begin
.im seevscnf
.see end
.*
.exmp begin
.blktext begin
To scan a date in the form "Friday August 13 2004":
.blktext end
.blkcode begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

void main( void )
{
    int day, year;
    char weekday[10], month[10];
    FILE *in_data;
.exmp break
    in_data = fopen( "file", "r" );
    if( in_data != NULL ) {
        fscanf_s( in_data, "%s %s %d %d",
                weekday, sizeof( weekday ),
		month, sizeof( month ),
		&day, &year );
        printf_s( "Weekday=%s Month=%s Day=%d Year=%d\n",
                weekday, month, day, year );
        fclose( in_data );
    }
}
.blkcode end
.exmp end
.*
.class TR 24731
.system
