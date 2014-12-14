.func fwide
.synop begin
#include <stdio.h>
#include <wchar.h>
int fwide( FILE *fp, int mode );
.synop end
.*
.desc begin
The
.id &func.
function determines the orientation of the stream pointed
to by
.arg fp
.ct .li .
If
.arg mode
is greater than zero, the function first attempts to make the stream
wide oriented. If
.arg mode
is less than zero, the function first attempts to make the stream
byte oriented. Otherwise,
.arg mode
is zero and the
.id &func.
function does not alter the orientation of the stream.
.desc end
.*
.return begin
The
.id &func.
function returns a value greater than zero if, after the call, 
the stream has wide orientation, a value less than zero if the stream
has byte orientation, or zero if the stream has no orientation.
.return end
.*
.see begin
.seelist fopen freopen
.see end
.*
.exmp begin
#include <stdio.h>
#include <wchar.h>

void main( void )
{
    FILE    *fp;
    int     mode;
.exmp break
    fp = fopen( "file", "r" );
    if( fp != NULL ) {
        mode = fwide( fp, -33 );
        printf( "orientation: %s\n",
            mode > 0 ? "wide" :
            mode < 0 ? "byte" : "none" );
    }
}
.exmp output
orientation: byte
.exmp end
.class ANSI
.system
