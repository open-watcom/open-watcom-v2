:CMT. common description of allocator and deallocator functions registered
:CMT. to the container classes.
:CMT. Parameter:
:CMT.   &lobjtype.      must be class name of the constructor
:CMT.======================================================================
These functions provide the ability to control how the allocation and
freeing of memory is performed, allowing for more efficient
memory handling than the general purpose global
:MONO operator new()
and
:MONO operator delete()
can provide.
Memory management optimizations may potentially be made through the
:HP1.allocator:eHP1. and :HP1.deallocator:eHP1. functions, but are not
recommended before managing memory is understood and determined to be
worth while.
:P.
The :HP1.allocator:eHP1.
.ix 'allocator' 'function'
function shall return a pointer to allocated memory of size at least the
argument, or zero(0) if the allocation cannot be performed.  Initialization
of the memory returned is performed by the
.MONO &lobjtype.
class.
:P.
The
.MONO &lobjtype.
class calls the :HP1.deallocator:eHP1. function only on
.ix 'dealloctor' 'function'
memory allocated by the :HP1.allocator:eHP1. function.
The :HP1.deallocator:eHP1. shall free the memory pointed to by the first
argument which is of size the second argument.  The size passed to the
:HP1.deallocator:eHP1. function is guaranteed to be the same size passed to
the :HP1.allocator:eHP1. function when the memory was allocated.
