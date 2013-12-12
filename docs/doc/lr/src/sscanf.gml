.func sscanf swscanf
.synop begin
.funcw swscanf
#include <stdio.h>
int sscanf( const char *in_string,
            const char *format, ... );
.ixfunc2 '&String' &func
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int swscanf( const wchar_t *in_string,
             const wchar_t *format, ... );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The &func function scans input from the character string
.arg in_string
under control of the argument
.arg format
.ct .li .
Following the format string is the list of addresses of items to
receive values.
.pp
The
.arg format
string is described under the description of the
.kw scanf
function.
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it accepts a
wide-character string argument for
.arg format
and the input string
.arg in_string
consists of wide characters.
.do end
.desc end
.*
.return begin
The &func function returns
.kw EOF
if the end of the input string was reached before any input conversion.
Otherwise, the number of input arguments for which values were
successfully scanned and stored is returned.
.return end
.*
.see begin
.im seevscnf sscanf
.see end
.*
.exmp begin
#include <stdio.h>

/* Scan a date in the form "Saturday April 18 1987" */
.exmp break
void main( void )
{
    int day, year;
    char weekday[10], month[10];
.exmp break
    sscanf( "Friday August 0014 1987",
            "%s %s %d  %d",
             weekday, month, &day, &year );
    printf( "%s %s %d %d\n",
             weekday, month, day, year );
}
.exmp output
Friday August 14 1987
.exmp end
.*
.class ISO C90
.system
