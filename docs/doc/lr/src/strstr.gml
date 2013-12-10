.func strstr _fstrstr wcsstr _mbsstr _fmbsstr _ustrstr
#include <string.h>
char *strstr( const char *str,
              const char *substr );
.ixfunc2 '&String' &func
.ixfunc2 '&Search' &func
.if &farfnc eq 1 .do begin
char __far *_fstrstr( const char __far *str,
                      const char __far *substr );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Search' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *wcsstr( const wchar_t *str,
                 const wchar_t *substr );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Search' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbsstr( const unsigned char *str,
                        const unsigned char *substr );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Search' &mfunc
.ixfunc2 '&Multibyte' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbsstr(
                    const unsigned char __far *str,
                    const unsigned char __far *substr );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Search' &fmfunc
.ixfunc2 '&Multibyte' &fmfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
wchar_t *_ustrstr( const wchar_t *str,
                    const wchar_t *substr );
.ixfunc2 '&String' &ufunc
.ixfunc2 '&Search' &ufunc
.do end
.funcend
.desc begin
The &func function locates the first occurrence in the string pointed
to by
.arg str
of the sequence of characters (excluding the terminating null
character) in the string pointed to by
.arg substr
.ct .li .
.im farfunc
.im widefunc
.im mbsffunc
.desc end
.return begin
The &func function returns a pointer to the located string, or
.mono NULL
if the string is not found.
.return end
.see begin
.seelist strstr strcspn
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    printf( "%s\n", strstr("This is an example", "is") );
  }
.exmp output
is is an example
.exmp end
.class ANSI
.system
