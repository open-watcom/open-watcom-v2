.func cputs
#include <conio.h>
int cputs( const char *buf );
.ixfunc2 '&KbIo' &func
.funcend
.desc begin
The &func function writes the character string pointed to by
.arg buf
directly to the console using the
.kw putch
function.
Unlike the
.kw puts
function, the carriage-return and line-feed characters are not appended
to the string.
The terminating null character is not written.
.desc end
.return begin
The &func
function returns a non-zero value if an error occurs; otherwise, it
returns zero.
.im errnoref
.return end
.see begin
.seelist cputs fputs putch puts
.see end
.exmp begin
#include <conio.h>

void main()
  {
    char buffer[82];
.exmp break
    buffer[0] = 80;
    cgets( buffer );
    cputs( &buffer[2] );
    putch( '\r' );
    putch( '\n' );
  }
.exmp end
.class WATCOM
.system
