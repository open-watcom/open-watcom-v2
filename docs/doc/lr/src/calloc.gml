.if &farfnc eq 0 .do begin
.func calloc
.synop begin
#include <stdlib.h>
void *calloc( size_t n, size_t size );
.ixfunc2 '&Memory' &funcb
.synop end
.desc begin
The
.id &funcb.
function allocates space for an array of
.arg n
objects, each of length
.arg size
bytes.
Each element is initialized to 0.
.np
A block of memory allocated using the
.id &funcb.
function should be freed
using the
.kw free
function.
.desc end
.return begin
The
.id &funcb.
function returns a pointer to the start of the allocated
memory.
The return value is
.mono NULL
if there is insufficient memory available or if the value of the
.arg size
argument is zero.
.return end
.see begin
.im seealloc
.see end
.cp 10
.exmp begin
#include <stdlib.h>

void main()
  {
    char *buffer;
.exmp break
    buffer = (char *)calloc( 80, sizeof(char) );
  }
.exmp end
.class ISO C
.do end
.************************
.el .do begin
.func begin calloc Functions
.func2 calloc
.func2 _bcalloc
.func2 _fcalloc
.func2 _ncalloc
.func end
.synop begin
#include <stdlib.h>  For ISO C compatibility (calloc only)
#include <malloc.h>  Required for other function prototypes
void *calloc( size_t n, size_t size );
void __based(void) *_bcalloc( __segment seg,
                              size_t n,
                              size_t size );
void __far  *_fcalloc( size_t n, size_t size );
void __near *_ncalloc( size_t n, size_t size );
.ixfunc2 '&Memory' calloc
.ixfunc2 '&Memory' _bcalloc
.ixfunc2 '&Memory' _fcalloc
.ixfunc2 '&Memory' _ncalloc
.synop end
.desc begin
The
.idbold &funcb.
functions allocate space for an array of
.arg n
objects, each of length
.arg size
bytes.
Each element is initialized to 0.
.np
Each function allocates memory from a particular heap, as listed below:
.begterm 8
.termhd1 Function
.termhd2 Heap
.term calloc
Depends on data model of the program
.term _bcalloc
Based heap specified by
.arg seg
value
.term _fcalloc
Far heap (outside the default data segment)
.term _ncalloc
Near heap (inside the default data segment)
.endterm
.np
In a small data memory model, the
.idbold &funcb.
function is equivalent to the
.kw _ncalloc
function; in a large data memory model, the
.idbold &funcb.
function is
equivalent to the
.kw _fcalloc
function.
.np
A block of memory allocated should be freed
using the appropriate
.kw free
function.
.desc end
.return begin
The
.idbold &funcb.
functions return a pointer to the start of the allocated
memory.
The return value is
.mono NULL
(
.ct .kw _NULLOFF
for
.kw _bcalloc
.ct )
if there is insufficient memory available or if the value of the
.arg size
argument is zero.
.return end
.see begin
.im seealloc
.see end
.cp 10
.exmp begin
#include <stdlib.h>

void main()
  {
    char *buffer;

    buffer = (char *)calloc( 80, sizeof(char) );
  }
.exmp end
.class ISO C
.do end
.system
