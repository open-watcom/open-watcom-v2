.if &farfnc eq 0 .do begin
.func realloc
#include <stdlib.h>
void * realloc( void *old_blk, size_t size );
.ixfunc2 '&Memory' &func
.synop end
.desc begin
When the value of the
.arg old_blk
argument is
.mono NULL,
a new block of memory of
.arg size
bytes is allocated.
.np
If the value of
.arg size
is zero, the
.kw free
function is called to release the memory pointed to by
.arg old_blk
.ct .li .
.np
Otherwise, the &func function re-allocates space for an object of
.arg size
bytes by either:
.begbull
.bull
shrinking the allocated size of the allocated memory block
.arg old_blk
when
.arg size
is sufficiently smaller than the size of
.arg old_blk
.ct .li .
.bull
extending the allocated size of the allocated memory block
.arg old_blk
if there is a large enough block of unallocated memory immediately
following
.arg old_blk
.ct .li .
.bull
allocating a new block and copying the contents of
.arg old_blk
to the new block.
.endbull
.np
Because it is possible that a new block will be allocated, no other
pointers should point into the memory of
.arg old_blk
.ct .li .
These pointers will point to freed memory, with possible disastrous
results, when a new block is allocated.
.np
The function returns
.mono NULL
when the memory pointed to by
.arg old_blk
cannot be re-allocated.
In this case, the memory pointed to by
.arg old_blk
is not freed so care should be exercised to maintain a pointer to the
old memory block.
.millust begin
buffer = (char *) realloc( buffer, 100 );
.millust end
.pc
In the above example,
.mono buffer
will be set to
.mono NULL
if the function fails and will no longer point to the old memory
block.
If
.mono buffer
was your only pointer to the memory block then you will have
lost access to this memory.
.desc end
.return begin
The &func function returns a pointer to the start of the re-allocated
memory.
The return value is
.mono NULL
if there is insufficient memory available or if the value of the
.arg size
argument is zero.
.return end
.see begin
.im seealloc realloc
.see end
.exmp begin
#include <stdlib.h>

void main()
  {
    char *buffer;
    char *new_buffer;
.exmp break
    buffer = (char *) malloc( 80 );
    new_buffer = (char *) realloc( buffer, 100 );
    if( new_buffer == NULL ) {

      /* not able to allocate larger buffer */

    } else {
      buffer = new_buffer;
    }
  }
.exmp end
.class ANSI
.do end
.************************
.el .do begin
.func begin realloc Functions
.func2 realloc
.func2 _brealloc
.func2 _frealloc
.func2 _nrealloc
.func gen
#include <stdlib.h>  For ANSI compatibility (realloc only)
#include <malloc.h>  Required for other function prototypes
void * realloc( void *old_blk, size_t size );
void __based(void) *_brealloc( __segment seg,
                               void __based(void) *old_blk,
                               size_t size );
void __far  *_frealloc( void __far  *old_blk,
                              size_t size );
void __near *_nrealloc( void __near *old_blk,
                              size_t size );
.ixfunc2 '&Memory' realloc
.ixfunc2 '&Memory' _brealloc
.ixfunc2 '&Memory' _frealloc
.ixfunc2 '&Memory' _nrealloc
.synop end
.desc begin
When the value of the
.arg old_blk
argument is
.mono NULL,
a new block of memory of
.arg size
bytes is allocated.
.np
If the value of
.arg size
is zero, the corresponding
.kw free
function is called to release the memory pointed to by
.arg old_blk
.ct .li .
.np
Otherwise, the &func function re-allocates space for an object of
.arg size
bytes by either:
.begbull
.bull
shrinking the allocated size of the allocated memory block
.arg old_blk
when
.arg size
is sufficiently smaller than the size of
.arg old_blk
.ct .li .
.bull
extending the allocated size of the allocated memory block
.arg old_blk
if there is a large enough block of unallocated memory immediately
following
.arg old_blk
.ct .li .
.bull
allocating a new block and copying the contents of
.arg old_blk
to the new block.
.endbull
.np
Because it is possible that a new block will be allocated, any
pointers into the old memory should not be maintained.
These pointers will point to freed memory, with possible disastrous
results, when a new block is allocated.
.np
The function returns
.mono NULL
when the memory pointed to by
.arg old_blk
cannot be re-allocated.
In this case, the memory pointed to by
.arg old_blk
is not freed so care should be exercised to maintain a pointer to the
old memory block.
.millust begin
buffer = (char *) realloc( buffer, 100 );
.millust end
.pc
In the above example,
.mono buffer
will be set to
.mono NULL
if the function fails and will no longer point to the old memory
block.
If
.mono buffer
was your only pointer to the memory block then you will have
lost access to this memory.
.np
Each function reallocates memory from a particular heap, as listed below:
.begterm 8
.termhd1 Function
.termhd2 Heap
.term realloc
Depends on data model of the program
.term _brealloc
Based heap specified by
.arg seg
value
.term _frealloc
Far heap (outside the default data segment)
.term _nrealloc
Near heap (inside the default data segment)
.endterm
.np
In a small data memory model, the &func function is equivalent to the
.kw _nrealloc
function; in a large data memory model, the &func function is
equivalent to the
.kw _frealloc
function.
.desc end
.return begin
The &func functions return a pointer to the start of the re-allocated
memory.
The return value is
.mono NULL
if there is insufficient memory available or if the value of the
.arg size
argument is zero.
The
.kw _brealloc
function returns
.kw _NULLOFF
if there is insufficient memory available or if the requested size is
zero.
.return end
.see begin
.im seealloc realloc
.see end
.exmp begin
#include <stdlib.h>
#include <malloc.h>

void main()
  {
    char *buffer;
    char *new_buffer;

    buffer = (char *) malloc( 80 );
    new_buffer = (char *) realloc( buffer, 100 );
    if( new_buffer == NULL ) {

      /* not able to allocate larger buffer */

    } else {
      buffer = new_buffer;
    }
  }
.exmp end
.* The following two lines force entries out for these functions
.sr wfunc='x _brealloc'
.sr mfunc='x _nrealloc'
.class ANSI
.do end
.system
