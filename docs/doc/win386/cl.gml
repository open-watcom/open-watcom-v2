.chap Compiling and Linking
.*
.np
To create a 32-bit application:
.autonote
.note
Compile application using the 32-bit compiler.
.note
Link application with the 32-bit libraries.
.note
Use
.kw wbind
to bind the 32-bit application with the 32-bit supervisor.
.note
Run and/or debug the application.
.endnote
.np
386 Windows modules are compiled as a regular 386 module.
There are no special switches required.
.np
Executables must be linked as Pharlap relocatable format (.rex).
Note that it is not necessary to export any call back routines.
.np
The linker options
.kw maxdata
and
.kw mindata
are used to control your 32-bit application's heap size.
Specifying
.kw maxdata
sets the amount of heap desired; and
.kw mindata
sets the minimum amount of heap needed.
If
.kw maxdata
cannot be allocated, then smaller and smaller pieces will be tried
until
.kw mindata
is reached.
If
.kw mindata
cannot be allocated, then the application will not start.
Growth of the heap can cause a shuffling of linear memory as Windows
attempts to find a contiguous piece of memory big enough.
By using
.kw mindata
and
.kw maxdata
to control your starting heap, and by using the
.kw amblksize
variable to control the number of bytes added when the heap is grown,
you can minimize this movement of linear memory.
.np
The library
.kw WIN386.LIB
contains the functions for connecting to the Windows API,
and
.kw WLIB3R.LIB
and
.kw WLIB3S.LIB
contains the regular C library functions (register and stack calling
conventions).
.np
The following is a sample directive  file for
.kw wlink
for creating a 32-bit application for Windows 3.0 or Windows 3.1:
.exam begin
format phar rex
debug all
file generic
option map
option stack=12288
option maxdata=2048K
option mindata=1024K
library win386
library wlib3r
.exam end
.np
After a program has been linked, it must be bound to the 32-bit
Windows supervisor
.kw WIN386.EXT
using
.kw WBIND.EXE
.ct .li .
