.func _memmax
#include <malloc.h>
size_t _memmax( void );
.ixfunc2 '&Memory' &func
.funcend
.desc begin
The &func function returns the size of the largest contiguous block
of memory available for dynamic memory allocation in the near heap
(the default data segment).
In the tiny, small and medium memory models, the default data segment
is only extended as needed to satisfy requests for memory allocation.
Therefore, you will need to call
.kw _nheapgrow
in these memory models before calling &func
in order to get a meaningful result.
.pp
.desc end
.return begin
The &func function returns the size of the largest contiguous block
of memory available for dynamic memory allocation in the near heap.
If 0 is returned, then there is no more memory available in the
near heap.
.return end
.see begin
.seelist _memmax calloc _freect _memavl _heapgrow malloc
.see end
.exmp begin
#include <stdio.h>
#include <malloc.h>
.exmp break
void main()
  {
    char *p;
    size_t size;
.exmp break
    size = _memmax();
    printf( "Maximum memory available is %u\n", size );
    _nheapgrow();
    size = _memmax();
    printf( "Maximum memory available is %u\n", size );
    p = (char *) _nmalloc( size );
    size = _memmax();
    printf( "Maximum memory available is %u\n", size );
  }
.exmp output
Maximum memory available is 0
Maximum memory available is 62700
Maximum memory available is 0
.exmp end
.class WATCOM
.system
