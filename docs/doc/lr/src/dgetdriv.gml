.func _dos_getdrive
#include <&doshdr>
void _dos_getdrive( unsigned *drive );
.ixfunc2 '&DosFunc' &func
.funcend
.desc begin
The &func function uses system call 0x19 to get the current disk drive
number.
The current disk drive number is returned in the word pointed to by
.arg drive
.ct .li .
A value of 1 is drive A, 2 is drive B, 3 is drive C, etc.
.desc end
.return begin
The &func function has no return value.
.return end
.see begin
.seelist &function. _dos_getdiskfree _dos_getdrive _dos_setdrive _getdiskfree _getdrive
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>
.exmp break
void main()
  {
    unsigned drive;
.exmp break
    _dos_getdrive( &drive );
    printf( "The current drive is %c\n",
                'A' + drive - 1 );
  }
.exmp output
The current drive is C
.exmp end
.class DOS
.system
