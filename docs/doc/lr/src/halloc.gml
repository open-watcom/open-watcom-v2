.func halloc
.synop begin
#include <malloc.h>
void __huge *halloc( long int numb, size_t size );
.ixfunc2 '&Memory' &func
.synop end
.desc begin
The &func function allocates space for an array of
.arg numb
objects of
.arg size
bytes each and initializes each object to 0.
When the size of the array is greater than 64K bytes, then the size of
an array element must be a power of 2 since an object could straddle a
segment boundary.
.desc end
.return begin
The &func function returns a far pointer (of type
.id void huge *
.ct ) to the start of the allocated memory.
The
.mono NULL
value is returned if there is insufficient memory available.
The
.mono NULL
value is also returned if the size of the array is greater than 64K
bytes and the size of an array element is not a power of 2.
.return end
.see begin
.im seealloc halloc
.see end
.exmp begin
#include <stdio.h>
#include <malloc.h>

void main()
  {
    long int __huge *big_buffer;
.exmp break
    big_buffer = (long int __huge *)
                  halloc( 1024L, sizeof(long) );
    if( big_buffer == NULL ) {
      printf( "Unable to allocate memory\n" );
    } else {

      /* rest of code goes here */

      hfree( big_buffer );  /* deallocate */
    }
  }
.exmp end
.class WATCOM
.system
