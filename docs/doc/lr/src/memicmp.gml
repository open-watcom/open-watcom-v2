.func memicmp _memicmp _fmemicmp
.synop begin
#include <string.h>
int memicmp( const void *s1,
             const void *s2,
             size_t length );
.ixfunc2 '&Compare' &funcb
.if &'length(&_func.) ne 0 .do begin
int _memicmp( const void *s1,
              const void *s2,
              size_t length );
.ixfunc2 '&Compare' &_func
.do end
.if &farfnc ne 0 .do begin
int _fmemicmp( const void __far *s1,
               const void __far *s2,
               size_t length );
.ixfunc2 '&Compare' &ffunc
.do end
.synop end
.desc begin
The function compares, without case sensitivity (upper- and
lowercase characters are equivalent), the first
.arg length
characters of the object pointed to by
.arg s1
to the object pointed to by
.arg s2
.period
All uppercase characters from
.arg s1
and
.arg s2
are mapped to lowercase for the purposes of doing the comparison.
.farfuncp &ffunc. &funcb.
.im ansiconf
.desc end
.return begin
The function returns an integer less than, equal to, or greater
than zero, indicating that the object pointed to by
.arg s1
is less than, equal to, or greater than the object pointed to by
.arg s2
.period
.return end
.see begin
.seelist memchr memcmp memcpy memset
.see end
.exmp begin
#include <stdio.h>
#include <string.h>

void main()
  {
    char buffer[80];
.exmp break
    if( memicmp( buffer, "Hello", 5 ) < 0 ) {
      printf( "Less than\n" );
    }
  }
.exmp end
.class WATCOM
.system
