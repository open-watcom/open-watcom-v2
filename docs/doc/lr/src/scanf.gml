.func begin scanf
.func2 wscanf ISO C95
.func end
.synop begin
#include <stdio.h>
int scanf( const char *format, ... );
.ixfunc2 '&StrIo' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int wscanf( const wchar_t *format, ... );
.ixfunc2 '&StrIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.safealt
.*
.desc begin
The
.id &funcb.
function scans input from the file designated by
.kw stdin
under control of the argument
.arg format
.ct .li .
The
.arg format
string is described below.
Following the format string is the list of addresses of items to
receive values.
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
if an input failure occured before any conversion.
Otherwise, the number of input arguments for which values were
successfully scanned and stored is returned.
.return end
.*
.see begin
.im seevscnf
.see end
.*
.exmp begin
.blktext begin
To scan a date in the form "Saturday April 18 1987":
.blktext end
.blkcode begin
#include <stdio.h>

void main( void )
{
    int day, year;
    char weekday[10], month[10];
.exmp break
    scanf( "%s %s %d %d", weekday, month, &day, &year );
}
.blkcode end
.*
.exmp end
.im scanfs
