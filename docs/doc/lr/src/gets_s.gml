.func gets_s
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
char *gets_s( char *s, rsize_t n );
.synop end

.rtconst freetext
If any of the following runtime-constraints is violated, the currently
active runtime-constraint handler will be invoked and
.id &funcb.
will set
.arg s[0]
to be the null character, and characters are read and discarded from
.kw stdin
until a new-line character is read, or end-of-file or a read error occurs.
.np
.arg s
shall not be a null pointer.
.arg n
shall neither be equal to zero nor be greater than
.kw RSIZE_MAX
 .
A new-line character, end-of-file, or read error shall occur within reading
.arg n-1
characters from
.kw stdin
 .
.rtconst end

.desc begin
The
.id &funcb.
function gets a string of characters from the file
designated by
.kw stdin
and stores them in the array pointed to by
.arg s
until end-of-file is encountered or a new-line character is read.
Size of the array
.arg s
is specified by the argument
.arg n
, this information is used to protect buffer from overflow. If buffer
.arg s
is about to be overflown, runtime-constraint is activated. Any new-line character
is discarded, and a null character is placed immediately after the last character
read into the array.
.desc end

.return begin
The
.id &funcb.
function returns
.arg s
if successful.
.mono NULL
is returned if there was a runtime-constraint violation, or if end-of-file
is encountered and no caracters have been read into the array, or if a
read error occurs.
.return end

.see begin
.seelist fgetc fgetchar fgets fopen getc getchar gets ungetc
.see end

.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>

int main()
  {
    char buffer[80];
.exmp break
    while( gets_s( buffer, sizeof( buffer ) ) != NULL )
      puts( buffer );
  }
.exmp end
.class TR 24731
.system
