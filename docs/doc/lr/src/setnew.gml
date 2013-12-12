.func set_new_handler _set_new_handler
.synop begin
#include <new.h>
PFV set_new_handler( PFV pNewHandler );
PFU _set_new_handler( PFU pNewHandler );
.synop end
.desc begin
The &func functions are used to transfer control to a user-defined
error handler if the
.kw new
operator fails to allocate memory.
The argument
.arg pNewHandler
is the name of a function of type
.kw PFV
or
.kw PFU
.ct .li .
.begterm 12
.termhd1 Type
.termhd2 Description
.term PFV
Pointer to a function that returns
.kw void
(i.e., returns nothing)
and takes an argument of type
.kw void
(i.e., takes no argument).
.term PFU
Pointer to a function that returns
.kw int
and takes an argument of type
.kw unsigned
which is the amount of space to be allocated.
.endterm
.np
In a multi-threaded environment, handlers are maintained separately
for each process and thread.
Each new process lacks installed handlers.
Each new thread gets a copy of its parent thread's new handlers.
Thus, each process and thread is in charge of its own free-store error
handling.
.desc end
.return begin
The &func functions return a pointer to the previous error handler so
that the previous error handler can be reinstated at a later time.
.np
The error handler specified as the argument to &_func returns zero
indicating that further attempts to allocate memory should be halted
or non-zero to indicate that an allocation request should be
re-attempted.
.return end
.see begin
.seelist _set_new_handler _bfreeseg _bheapseg calloc delete free
.seelist _set_new_handler malloc new realloc
.see end
.exmp begin
#include <stdio.h>
#include <new.h>

#if defined(__386__)
const size_t MemBlock = 8192;
#else
const size_t MemBlock = 2048;
#endif
.exmp break
/*
    Pre-allocate a memory block for demonstration
    purposes. The out-of-memory handler will return
    it to the system so that "new" can use it.
*/

long *failsafe = new long[MemBlock];
.exmp break
/*
    Declare a customized function to handle memory
    allocation failure.
*/

int out_of_memory_handler( unsigned size )
  {
    printf( "Allocation failed, " );
    printf( "%u bytes not available.\n", size );
    /* Release pre-allocated memory if we can */
    if( failsafe == NULL ) {
      printf( "Halting allocation.\n" );
      /* Tell new to stop allocation attempts */
      return( 0 );
    } else {
      delete failsafe;
      failsafe = NULL;
      printf( "Retrying allocation.\n" );
      /* Tell new to retry allocation attempt */
      return( 1 );
    }
  }
.exmp break
void main( void )
  {
    int i;

    /* Register existence of a new memory handler */
    _set_new_handler( out_of_memory_handler );
    long *pmemdump = new long[MemBlock];
    for( i=1 ; pmemdump != NULL; i++ ) {
      pmemdump = new long[MemBlock];
      if( pmemdump != NULL )
        printf( "Another block allocated %d\n", i );
    }
  }
.exmp end
.class WATCOM
.system
