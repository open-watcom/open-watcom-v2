.func begin _heapchk Functions
.func2 _heapchk
.func2 _bheapchk
.func2 _fheapchk
.func2 _nheapchk
.func end
.synop begin
.ixfunc2 '&Heap' _heapchk
.ixfunc2 '&Heap' _bheapchk
.ixfunc2 '&Heap' _fheapchk
.ixfunc2 '&Heap' _nheapchk
.ixfunc2 '&Memory' _heapchk
.ixfunc2 '&Memory' _bheapchk
.ixfunc2 '&Memory' _fheapchk
.ixfunc2 '&Memory' _nheapchk
#include <malloc.h>
int  _heapchk( void );
int _bheapchk( __segment seg );
int _fheapchk( void );
int _nheapchk( void );
.synop end
.desc begin
The
.id &funcb.
functions along with
.kw _heapset
and
.kw _heapwalk
are provided for debugging heap related problems in programs.
.np
The
.id &funcb.
functions perform a consistency check on the unallocated
memory space or "heap".
The consistency check determines whether all the heap entries are valid.
Each function checks a particular heap, as listed below:
.begterm 12
.termhd1 Function
.termhd2 Heap Checked
.term _heapchk
Depends on data model of the program
.term _bheapchk
Based heap specified by
.arg seg
value;
.kw _NULLSEG
specifies all based heaps
.term _fheapchk
Far heap (outside the default data segment)
.term _nheapchk
Near heap (inside the default data segment)
.endterm
.np
In a small data memory model, the
.id &funcb.
function is equivalent to the
.kw _nheapchk
function; in a large data memory model, the
.id &funcb.
function is
equivalent to the
.kw _fheapchk
function.
.desc end
.return begin
All four functions return one of the following manifest constants
which are defined in
.mono <malloc.h>.
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term _HEAPOK
The heap appears to be consistent.
.term _HEAPEMPTY
The heap is empty.
.term _HEAPBADBEGIN
The heap has been damaged.
.term _HEAPBADNODE
The heap contains a bad node, or is damaged.
.endterm
.return end
.see begin
.seelist _heapchk _heapenable _heapgrow _heapmin _heapset _heapshrink _heapwalk
.see end
.exmp begin
#include <stdio.h>
#include <malloc.h>
.exmp break
void main()
  {
    char *buffer;
.exmp break
    buffer = (char *)malloc( 80 );
    malloc( 1024 );
    free( buffer );
    switch( _heapchk() ) {
    case _HEAPOK:
      printf( "OK - heap is good\n" );
      break;
    case _HEAPEMPTY:
      printf( "OK - heap is empty\n" );
      break;
    case _HEAPBADBEGIN:
      printf( "ERROR - heap is damaged\n" );
      break;
    case _HEAPBADNODE:
      printf( "ERROR - bad node in heap\n" );
      break;
    }
  }
.exmp end
.class WATCOM
.system
