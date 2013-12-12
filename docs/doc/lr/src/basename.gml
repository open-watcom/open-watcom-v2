.func basename 
.synop begin
#include <libgen.h>
char *basename( char *path );
.synop end
.*
.desc begin
The &func function returns a pointer to the final component of a pathname
pointed to by the
.arg path
argument, deleting trailing path separators.
.np
If the string pointed to by
.arg path
consists entirely of path separators, a string consisting of single path
separator is returned.
.np
If
.arg path
is a null pointer or points to an empty string, a pointer to the string "."
is returned.
.np
The &func function may modify the string pointed to by
.arg path
and may return a pointer to static storage that may be overwritten by
a subsequent call to &func..
.np
The &func function is not re-entrant or thread-safe.
.desc end
.*
.return begin
The &func function returns a pointer to the final component of
.arg path
.ct .li .
.return end
.*
.see begin
.seelist &function. dirname
.see end
.*
.exmp begin
#include <stdio.h>
#include <libgen.h>

int main( void )
{
.exmp break
    puts( basename( "/usr/lib" ) );
    puts( basename( "//usr//lib//" ) );
    puts( basename( "///" ) );
    puts( basename( "foo" ) );
    puts( basename( NULL ) );
    return( 0 );
}
.exmp output
lib
lib
/
foo
~.
.blkcode end
.exmp end
.*
.class POSIX
.system
