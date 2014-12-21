.func strerrorlen_s wcserrorlen_s
.synop begin
#define __STDC_WANT_LIB_EXT1__  1
#include <string.h>
size_t strerrorlen_s( errno_t errnum );
.ixfunc2 '&String' &funcb
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
size_t wcserrorlen_s( errno errnum );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.*
.rtconst none
.*
.desc begin
The
.id &funcb.
function calculates the length of the (untruncated) locale-specific
message string that the
.kw strerror_s
function maps to
.arg errnum
.ct .li .

.im widefun1
.desc end
.*
.return begin
The
.id &funcb.
function returns the number of characters (not including the
null character) in the full message string.
.return end
.*
.see begin
.seelist strerror strerror_s
.see end
.*
.exmp begin
#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <string.h>
#include <errno.h>

void main( void )
{
    FILE    *fp;
    char    emsg[ 100 ];
    size_t  emsglen;
.exmp break
    fp = fopen( "file.nam", "r" );
    if( fp == NULL ) {
        emsglen = strerrorlen_s( errno );
        printf( "Length of errormessage: %d\n", emsglen );
        strerror_s( emsg, sizeof( emsg ), errno );
        printf( "Unable to open file: %s\n", emsg );
    }
}
.exmp end
.*
.class TR 24731
.system
