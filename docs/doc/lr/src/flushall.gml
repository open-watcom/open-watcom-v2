.func _flushall flushall
.ansiname _flushall
.synop begin
#include <stdio.h>
int _flushall( void );
.ixfunc2 '&StrIo' _flushall

.deprec
int flushall( void );
.ixfunc2 '&StrIo' flushall
.synop end
.desc begin
The
.id &funcb.
function clears all buffers associated with input streams
and writes any buffers associated with output streams.
A subsequent read operation on an input file causes new data
to be read from the associated file or device.
.np
Calling the
.id &funcb.
function is equivalent to calling the
.reffunc fflush
for all open stream files.
.deprfunc flushall _flushall
.desc end
.return begin
The
.id &funcb.
function returns the number of open streams.
When an output error occurs while writing to a file, the
.kw errno
global variable will be set.
.return end
.see begin
.seelist fopen fflush
.see end
.exmp begin
#include <stdio.h>

void main()
  {
    printf( "The number of open files is %d\n",
            _flushall() );
  }
.exmp output
The number of open files is 4
.exmp end
.class WATCOM
.system
