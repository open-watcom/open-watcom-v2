.func begin _expand Functions
.func2 _expand
.func2 _bexpand
.func2 _fexpand
.func2 _nexpand
.func end
.synop begin
#include <malloc.h>
void        *_expand( void *mem_blk, size_t size );
void __based(void) *_bexpand( __segment seg,
                              void __based(void) *mem_blk,
                              size_t size );
void __far  *_fexpand(void __far  *mem_blk,size_t size);
void __near *_nexpand(void __near *mem_blk,size_t size);
.ixfunc2 '&Memory' _expand
.ixfunc2 '&Memory' _bexpand
.ixfunc2 '&Memory' _fexpand
.ixfunc2 '&Memory' _nexpand
.synop end
.desc begin
The
.id &func.
functions change the size of the previously allocated block
pointed to by
.arg mem_blk
by attempting to expand or contract the memory block without moving
its location in the heap.
The argument
.arg size
specifies the new desired size for the memory block.
The contents of the memory block are unchanged up to the shorter
of the new and old sizes.
.np
Each function expands the memory from a particular heap, as listed below:
.begterm 8
.termhd1 Function
.termhd2 Heap Expanded
.term _expand
Depends on data model of the program
.term _bexpand
Based heap specified by
.arg seg
value
.term _fexpand
Far heap (outside the default data segment)
.term _nexpand
Near heap (inside the default data segment)
.endterm
.np
In a small data memory model, the
.id &func.
function is equivalent to the
.kw _nexpand
function; in a large data memory model, the
.id &func.
function is
equivalent to the
.kw _fexpand
function.
.desc end
.return begin
The
.id &func.
functions return the value
.arg mem_blk
if it was successful in changing the size of the block.
The return value is
.mono NULL
(
.ct .mono _NULLOFF
for
.kw _bexpand
.ct )
if the memory block could not be expanded to the desired size.
It will be expanded as much as possible in this case.
.np
The appropriate
.kw _msize
function can be used to determine the new size of the expanded block.
.return end
.see begin
.im seealloc _expand
.see end
.exmp begin
#include <stdio.h>
#include <malloc.h>
.exmp break
void main()
  {
    char *buf;
    char __far *buf2;
.exmp break
    buf = (char *) malloc( 80 );
    printf( "Size of buffer is %u\n", _msize(buf) );
    if( _expand( buf, 100 ) == NULL ) {
        printf( "Unable to expand buffer\n" );
    }
    printf( "New size of buffer is %u\n", _msize(buf) );
    buf2 = (char __far *) _fmalloc( 2000 );
    printf( "Size of far buffer is %u\n", _fmsize(buf2) );
    if( _fexpand( buf2, 8000 ) == NULL ) {
        printf( "Unable to expand far buffer\n" );
    }
    printf( "New size of far buffer is %u\n",
             _fmsize(buf2) );
  }
.exmp output
Size of buffer is 80
Unable to expand buffer
New size of buffer is 80
Size of far buffer is 2000
New size of far buffer is 8000
.exmp end
.class WATCOM
.system
