.func begin _msize Functions
.func2 _msize
.func2 _bmsize
.func2 _fmsize
.func2 _nmsize
.func end
.synop begin
.ixfunc2 '&Memory' _msize
.ixfunc2 '&Memory' _bmsize
.ixfunc2 '&Memory' _fmsize
.ixfunc2 '&Memory' _nmsize
#include <malloc.h>
size_t _msize( void *buffer );
size_t _bmsize( __segment seg, void __based(void) *buffer );
size_t _fmsize( void __far *buffer );
size_t _nmsize( void __near *buffer );
.synop end
.desc begin
The
.id &funcb.
functions return the size of the memory block pointed to by
.arg buffer
that was allocated by a call to the appropriate version of the
.kw calloc
.ct,
.kw malloc
.ct,
or
.kw realloc
functions.
.np
You must use the correct
.id &funcb.
function as listed below depending on
which heap the memory block belongs to.
.begterm 8
.termhd1 Function
.termhd2 Heap
.term _msize
Depends on data model of the program
.term _bmsize
Based heap specified by
.arg seg
value
.term _fmsize
Far heap (outside the default data segment)
.term _nmsize
Near heap (inside the default data segment)
.endterm
.np
In small data models (small and medium memory models),
.id &funcb.
maps to
.kw _nmsize
.ct .li .
In large data models (compact, large and huge memory models), &funcb
maps to
.kw _fmsize
.ct .li .
:cmt. .pp
:cmt. The
:cmt. .kw _nmsize
:cmt. function returns the size of the memory block allocated by a call to
:cmt. .kw _nmalloc
:cmt. .ct .li .
:cmt. .pp
:cmt. The
:cmt. .kw _fmsize
:cmt. function returns the size of the memory block allocated by a call to
:cmt. .kw _fmalloc
:cmt. .ct .li .
.desc end
.return begin
The
.id &funcb.
functions return the size of the memory block pointed to by
.arg buffer
.ct .li .
.return end
.see begin
.im seealloc
.see end
.exmp begin
#include <stdio.h>
#include <malloc.h>

void main()
  {
    void *buffer;
.exmp break
    buffer = malloc( 999 );
    printf( "Size of block is %u bytes\n",
                _msize( buffer ) );
  }
.exmp output
Size of block is 1000 bytes
.exmp end
.class WATCOM
.system
