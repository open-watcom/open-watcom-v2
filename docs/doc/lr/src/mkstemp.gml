.func mkstemp
.synop begin
#include <stdlib.h>
int mkstemp( char *template );
.ixfunc2 '&FileOp' &func
.synop end
.desc begin
The &func function creates a file with unique name by modifying the
.arg template
argument, and returns its file handle open for reading and writing
in binary mode. The use of &func prevents any possible race condition between
testing whether the file exists and opening it for use.
.np
The string
.arg template
has the form
.mono baseXXXXXX
where
.mono base
is the fixed part of the generated filename and
.mono XXXXXX
is the variable part of the generated filename.
Each of the 6 X's is a placeholder for a character supplied by &func..
Each placeholder character in
.arg template
must be an uppercase "X".
&func preserves
.mono base
and replaces the first of the 6 trailing X's with a unique sequence
of alphanumeric characters. The string
.arg template
therefore must be writable.
.np
&func checks to see if a file with the generated name already exists
and if so selects another name, until it finds a file that doesn't exist.
If it is unsuccessful at finding a name for a file that does not
already exist or is unable to create a file, &func returns -1.
.desc end
.return begin
The &func function returns a file handle. When an error occurs while creating
the file, -1 is returned.
.return end
.see begin
.seelist &function. fopen freopen mkstemp _mktemp _tempnam tmpfile tmpnam
.see end
.exmp begin
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define TEMPLATE    "_tXXXXXX"
#define MAX_TEMPS   5

void main( void )
{
    char    name[sizeof( TEMPLATE )];
    int     i;
    int     handles[MAX_TEMPS];
.exmp break
    for( i = 0; i < MAX_TEMPS; i++ ) {
        strcpy( name, TEMPLATE );
        handles[i] = mkstemp( name );
        if( handles[i] == -1 ) {
            printf( "Failed to create temporary file\n" );
        } else {
            printf( "Created temporary file '%s'\n", name );
        }
    }
    for( i = 0; i < MAX_TEMPS; i++ ) {
        if( handles[i] != -1 ) {
            close( handles[i] );
        }
    }
}
.exmp end
.class POSIX
.system
