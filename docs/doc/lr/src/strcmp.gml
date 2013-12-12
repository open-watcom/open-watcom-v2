.func strcmp _fstrcmp wcscmp _mbscmp _fmbscmp
.synop begin
#include <string.h>
int strcmp( const char *s1, const char *s2 );
.ixfunc2 '&String' &func
.ixfunc2 '&Compare' &func
.if &farfnc eq 1 .do begin
int _fstrcmp( const char __far *s1,
              const char __far *s2 );
.do end
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Compare' &ffunc
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
int wcscmp( const wchar_t *s1, const wchar_t *s2 );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Compare' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
int _mbscmp( const unsigned char *s1,
             const unsigned char *s2 );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Compare' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
int _fmbscmp( const unsigned char __far *s1,
              const unsigned char __far *s2 );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Compare' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.synop end
.desc begin
The
.id &func.
function compares the string pointed to by
.arg s1
to the string pointed to by
.arg s2
.ct .li .
.im farparm
.im widefunc
.im mbsfunc
.desc end
.return begin
The
.id &func.
function returns an integer less than, equal to, or greater
than zero, indicating that the string pointed to by
.arg s1
is less than, equal to, or greater than the string pointed to by
.arg s2
.ct .li .
.return end
.see begin
.seelist &function. strcmp strcmpi stricmp strncmp strnicmp
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%d\n", strcmp( "abcdef", "abcdef" ) );
    printf( "%d\n", strcmp( "abcdef", "abc" ) );
    printf( "%d\n", strcmp( "abc", "abcdef" ) );
    printf( "%d\n", strcmp( "abcdef", "mnopqr" ) );
    printf( "%d\n", strcmp( "mnopqr", "abcdef" ) );
  }
.exmp output
0
1
-1
-1
1
.exmp end
.class ANSI
.system
