.func _freect
.synop begin
#include <malloc.h>
unsigned int _freect( size_t size );
.ixfunc2 '&Memory' &funcb
.synop end
.desc begin
The
.id &funcb.
function returns the number of times that
.kw _nmalloc
(or
.kw malloc
in small data models)
can be called to allocate a item of
.arg size
bytes.
In the tiny, small and medium memory models, the default data segment
is only extended as needed to satisfy requests for memory allocation.
Therefore, you will need to call
.kw _nheapgrow
in these memory models before calling &funcb
in order to get a meaningful result.
.desc end
.return begin
The
.id &funcb.
function returns the number of calls as an unsigned integer.
.return end
.see begin
.seelist calloc _heapgrow Functions malloc Functions _memavl _memmax
.see end
.exmp begin
#include <stdio.h>
#include <malloc.h>
.exmp break
void main()
  {
    int  i;
.exmp break
    printf( "Can allocate %u longs before _nheapgrow\n",
            _freect( sizeof(long) ) );
    _nheapgrow();
    printf( "Can allocate %u longs after _nheapgrow\n",
            _freect( sizeof(long) ) );
    for( i = 1; i < 1000; i++ ) {
      _nmalloc( sizeof(long) );
    }
    printf( "After allocating 1000 longs:\n" );
    printf( "Can still allocate %u longs\n",
            _freect( sizeof(long) ) );
  }
.exmp output
Can allocate 0 longs before _nheapgrow
Can allocate 10447 longs after _nheapgrow
After allocating 1000 longs:
Can still allocate 9447 longs
.exmp end
.class WATCOM
.system
