.func btowc
.synop begin
#include <wchar.h>
wint_t btowc( int c );
.ixfunc2 '&Wide' &funcb
.synop end
.*
.desc begin
The
.id &funcb.
function determines whether
.arg c
is a valid single-byte character in the initial shift state.
.desc end
.*
.return begin
The
.id &funcb.
function returns
.mono WEOF
if
.arg c
has the value
.mono EOF
or if
.arg (unsigned char)c
does not constitute a valid single-byte character in the initial
shift state. Otherwise,
.id &funcb.
returns the wide character representation of
that character. 
.return end
.*
.see begin
.im seembc
.see end
.*
.exmp begin
#include <stdio.h>
#include <wchar.h>
.exmp break
void main( void )
{
    printf( "EOF is %sa valid single-byte character\n",
        btowc( EOF ) == WEOF ? "not " : "" );
}
.exmp output
EOF is not a valid single-byte character
.exmp end
.*
.class ISO C
.system
