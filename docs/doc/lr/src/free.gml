.if &farfnc eq 0 .do begin
.func free
#include <stdlib.h>
void free( void *ptr );
.ixfunc2 '&Memory' free
.synop end
.desc begin
When the value of the argument
.arg ptr
is
.mono NULL,
the &func function does nothing; otherwise, the &func function
deallocates the memory block located by the argument
.arg ptr
which points to a memory block previously allocated through a call to
.kw calloc
.ct,
.kw malloc
or
.kw realloc
.ct .li .
After the call, the freed block is available for allocation.
.desc end
.return begin
The &func function returns no value.
.return end
.see begin
.im seealloc free
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
  {
    char *buffer;
.exmp break
    buffer = (char *)malloc( 80 );
    if( buffer == NULL ) {
      printf( "Unable to allocate memory\n" );
    } else {

      /* rest of code goes here */

      free( buffer );  /* deallocate buffer */
    }
  }
.exmp end
.class ANSI
.do end
.************************
.el .do begin
.func begin free Functions
.func2 free
.func2 _bfree
.func2 _ffree
.func2 _nfree
.func gen
#include <stdlib.h>  For ANSI compatibility (free only)
#include <malloc.h>  Required for other function prototypes
void free( void *ptr );
void _bfree( __segment seg, void __based(void) *ptr );
void _ffree( void __far  *ptr );
void _nfree( void __near *ptr );
.ixfunc2 '&Memory' free
.ixfunc2 '&Memory' _bfree
.ixfunc2 '&Memory' _ffree
.ixfunc2 '&Memory' _nfree
.synop end
.desc begin
When the value of the argument
.arg ptr
is
.mono NULL,
the &func function does nothing; otherwise, the &func function
deallocates the memory block located by the argument
.arg ptr
which points to a memory block previously allocated through a call to
the appropriate version of
.kw calloc
.ct,
.kw malloc
or
.kw realloc
.ct .li .
After the call, the freed block is available for allocation.
.np
Each function deallocates memory from a particular heap, as listed below:
.begterm 8
.termhd1 Function
.termhd2 Heap
.term free
Depends on data model of the program
.term _bfree
Based heap specified by
.arg seg
value
.term _ffree
Far heap (outside the default data segment)
.term _nfree
Near heap (inside the default data segment)
.endterm
.np
In a large data memory model, the &func function is equivalent to the
.kw _ffree
function; in a small data memory model, the &func function is
equivalent to the
.kw _nfree
function.
.desc end
.return begin
The &func functions return no value.
.return end
.see begin
.im seealloc free
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
  {
    char *buffer;
.exmp break
    buffer = (char *)malloc( 80 );
    if( buffer == NULL ) {
      printf( "Unable to allocate memory\n" );
    } else {

      /* rest of code goes here */

      free( buffer );  /* deallocate buffer */
    }
  }
.exmp end
.* The following two lines force entries out for these functions
.sr wfunc='x _bfree'
.sr mfunc='x _nfree'
.class ANSI
.do end
.system
