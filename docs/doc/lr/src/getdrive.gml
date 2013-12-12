.func _getdrive
.synop begin
#include <direct.h>
int _getdrive( void );
.ixfunc2 '&DosFunc' &func
.synop end
.desc begin
The
.id &func.
function returns the current (default) drive number.
.desc end
.return begin
A value of 1 is drive A, 2 is drive B, 3 is drive C, etc.
.return end
.see begin
.seelist &function. _dos_getdiskfree _dos_getdrive _dos_setdrive _getdiskfree _getdrive
.see end
.exmp begin
#include <stdio.h>
#include <direct.h>
.exmp break
void main( void )
{
    printf( "The current drive is %c\n",
                'A' + _getdrive() - 1 );
}
.exmp output
The current drive is C
.exmp end
.class DOS
.system
