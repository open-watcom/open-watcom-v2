.func _smalloc
#include <malloc.h>
void *_smalloc( size_t size );
.ixfunc2 '&Memory' &func
.funcend
.desc begin
The &func function allocates space from the shared memory heap for an
object of
.arg size
bytes.
Nothing is allocated when the
.arg size
argument has a value of zero.
.pp
A block of memory allocated using the &func function should be freed
using the
.kw free
function.
.desc end
.return begin
The &func function returns a pointer to the start of the allocated
memory.
The &func function returns
.mono NULL
if there is insufficient memory available or if the requested size is
zero.
.return end
.see begin
.im seealloc _smalloc
.see end
.exmp begin
#include <malloc.h>

void main()
  {
    char *buffer;
.exmp break
    buffer = (char *)_smalloc( 80 );
    .
    .
    .
    free( buffer );
  }
.exmp end
.class PenPoint
.system
