.func strpbrk _fstrpbrk wcspbrk _mbspbrk _fmbspbrk _ustrpbrk
#include <string.h>
char *strpbrk( const char *str, const char *charset );
.ixfunc2 '&String' &func
.ixfunc2 '&Search' &func
.if &farfnc eq 1 .do begin
char __far *_fstrpbrk( const char __far *str,
                       const char __far *charset );
.ixfunc2 '&String' &ffunc
.ixfunc2 '&Search' &ffunc
.do end
.if &'length(&wfunc.) ne 0 .do begin
#include <wchar.h>
wchar_t *wcspbrk( const wchar_t *str,
                  const wchar_t *charset );
.ixfunc2 '&String' &wfunc
.ixfunc2 '&Search' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&mfunc.) ne 0 .do begin
#include <mbstring.h>
unsigned char *_mbspbrk( const unsigned char *str,
                         const unsigned char *charset );
.ixfunc2 '&String' &mfunc
.ixfunc2 '&Search' &mfunc
.ixfunc2 '&Wide' &mfunc
.do end
.if &'length(&fmfunc.) ne 0 .do begin
unsigned char __far *_fmbspbrk(
                    const unsigned char __far *str,
                    const unsigned char __far *charset );
.ixfunc2 '&String' &fmfunc
.ixfunc2 '&Search' &fmfunc
.ixfunc2 '&Wide' &fmfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
wchar_t *_ustrpbrk( const wchar_t *str,
                     const wchar_t *charset );
.ixfunc2 '&String' &ufunc
.ixfunc2 '&Search' &ufunc
.do end
.funcend
.desc begin
The &func function locates the first occurrence in the string pointed
to by
.arg str
of any character from the string pointed to by
.arg charset
.ct .li .
.im farfunc
.im widefunc
.im mbsffunc
.im unifunc
.desc end
.return begin
The &func function returns a pointer to the located character, or
.mono NULL
if no character from
.arg charset
occurs in
.arg str
.ct .li .
.return end
.see begin
.seelist &function. strchr strpbrk strrchr strtok
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    char *p = "Find all vowels";
.exmp break
    while( p != NULL ) {
      printf( "%s\n", p );
      p = strpbrk( p+1, "aeiouAEIOU" );
    }
  }
.exmp output
Find all vowels
ind all vowels
all vowels
owels
els
.exmp end
.class ANSI
.system
