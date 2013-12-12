.func cgets
#include <conio.h>
char *cgets( char *buf );
.ixfunc2 '&KbIo' &func
.synop end
.desc begin
The &func
function gets a string of characters directly from the console
and stores the string and its length in the array pointed to by
.arg buf
.ct .li .
The first element of the array
.arg buf[0]
must contain the maximum length in characters of the string to be read.
The array must be big enough to hold the string, a terminating null
character, and two additional bytes.
.pp
The &func function reads characters until a
.if '&machsys' eq 'QNX' .do begin
newline character
.do end
.el .do begin
carriage-return line-feed combination
.do end
is read, or until the specified number of characters is read.
The string is stored in the array starting at
.arg buf[2]
.ct .li .
The
.if '&machsys' eq 'QNX' .do begin
newline character,
.do end
.el .do begin
carriage-return line-feed combination,
.do end
if read, is replaced by a null character.
The actual length of the string read is placed in
.arg buf[1]
.ct .li .
.desc end
.return begin
The &func function returns a pointer to the start of the string
which is at
.arg buf[2]
.ct .li .
.return end
.see begin
.seelist cgets fgets getch getche gets
.see end
.exmp begin
#include <conio.h>

void main()
  {
    char buffer[82];
.exmp break
    buffer[0] = 80;
    cgets( buffer );
    cprintf( "%s\r\n", &buffer[2] );
  }
.exmp end
.class WATCOM
.system
