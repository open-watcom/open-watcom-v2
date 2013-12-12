.func hfree
.synop begin
#include <malloc.h>
void hfree( void __huge *ptr );
.ixfunc2 '&Memory' &func
.synop end
.desc begin
The
.id &func.
function deallocates a memory block previously allocated by the
.kw halloc
function.
The argument
.arg ptr
points to a memory block to be deallocated.
After the call, the freed block is available for allocation.
.desc end
.return begin
The
.id &func.
function returns no value.
.return end
.see begin
.im seealloc hfree
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
.exmp break
      /* rest of code goes here */

      hfree( big_buffer );  /* deallocate */
    }
  }
.exmp end
.class WATCOM
.system
