.if &farfnc eq 0 .do begin
.func malloc
.synop begin
#include <stdlib.h>
void *malloc( size_t size );
.ixfunc2 '&Memory' &func
.synop end
.desc begin
The &func function allocates space
for an object of
.arg size
bytes.
Nothing is allocated when the
.arg size
argument has a value of zero.
.desc end
.return begin
The &func function returns a pointer to the start of the allocated
memory.
The &func function returns
.mono NULL
if there is insufficient memory available or if the requested size is zero.
.return end
.see begin
.im seealloc malloc
.see end
.exmp begin
#include <stdlib.h>

void main()
  {
    char *buffer;
.exmp break
    buffer = (char *)malloc( 80 );
    if( buffer != NULL ) {

        /* body of program */

        free( buffer );
    }
  }
.exmp end
.class ANSI
.do end
.************************
.el .do begin
.func begin malloc Functions
.func2 malloc
.func2 _bmalloc
.func2 _fmalloc
.func2 _nmalloc
.func end
.synop begin
#include <stdlib.h>  For ANSI compatibility (malloc only)
#include <malloc.h>  Required for other function prototypes
void *malloc( size_t size );
void __based(void) *_bmalloc( __segment seg, size_t size );
void __far  *_fmalloc( size_t size );
void __near *_nmalloc( size_t size );
.ixfunc2 '&Memory' malloc
.ixfunc2 '&Memory' _bmalloc
.ixfunc2 '&Memory' _fmalloc
.ixfunc2 '&Memory' _nmalloc
.synop end
.desc begin
The &func functions allocate space for an object of
.arg size
bytes.
Nothing is allocated when the
.arg size
argument has a value of zero.
.np
Each function allocates memory from a particular heap, as listed below:
.begterm 8
.termhd1 Function
.termhd2 Heap
.term malloc
Depends on data model of the program
.term _bmalloc
Based heap specified by
.arg seg
value
.term _fmalloc
Far heap (outside the default data segment)
.term _nmalloc
Near heap (inside the default data segment)
.endterm
.np
In a small data memory model, the &func function is equivalent to the
.kw _nmalloc
function; in a large data memory model, the &func function is
equivalent to the
.kw _fmalloc
function.
.desc end
.return begin
The &func functions return a pointer to the start of the allocated
memory.
The &func
.ct ,
.kw _fmalloc
and
.kw _nmalloc
functions return
.mono NULL
if there is insufficient memory available or if the requested size is
zero.
The
.kw _bmalloc
function returns
.kw _NULLOFF
if there is insufficient memory available or if the requested size is
zero.
.return end
.see begin
.im seealloc malloc
.see end
.exmp begin
#include <stdlib.h>

void main()
  {
    char *buffer;
.exmp break
    buffer = (char *)malloc( 80 );
    if( buffer != NULL ) {

        /* body of program */

        free( buffer );
    }
  }
.exmp end
.* The following two lines force entries out for these functions
.sr wfunc='x _bmalloc'
.sr mfunc='x _nmalloc'
.class ANSI
.do end
.system
