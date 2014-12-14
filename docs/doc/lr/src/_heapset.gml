.func begin _heapset Functions
.func2 _heapset
.func2 _bheapset
.func2 _fheapset
.func2 _nheapset
.func end
.synop begin
.ixfunc2 '&Heap' _heapset
.ixfunc2 '&Heap' _bheapset
.ixfunc2 '&Heap' _fheapset
.ixfunc2 '&Heap' _nheapset
.ixfunc2 '&Memory' _heapset
.ixfunc2 '&Memory' _bheapset
.ixfunc2 '&Memory' _fheapset
.ixfunc2 '&Memory' _nheapset
#include <malloc.h>
int  _heapset( unsigned char fill_char );
int _bheapset( __segment seg, unsigned char fill_char );
int _fheapset( unsigned char fill_char );
int _nheapset( unsigned char fill_char );
.synop end
.desc begin
The
.id &func.
functions along with
.kw _heapchk
and
.kw _heapwalk
are provided for debugging heap related problems in programs.
.np
The
.id &func.
functions perform a consistency check on the unallocated
memory space or "heap" just as
.kw _heapchk
does, and sets the heap's free entries with the
.arg fill_char
value.
.np
Each function checks and sets a particular heap, as listed below:
.begterm 12
.termhd1 Function
.termhd2 Heap Filled
.term _heapset
Depends on data model of the program
.term _bheapset
Based heap specified by
.arg seg
value;
.mono _NULLSEG
specifies all based heaps
.term _fheapset
Far heap (outside the default data segment)
.term _nheapset
Near heap (inside the default data segment)
.endterm
.np
In a small data memory model, the
.id &func.
function is equivalent to the
.kw _nheapset
function; in a large data memory model, the
.id &func.
function is
equivalent to the
.kw _fheapset
function.
.desc end
.return begin
The
.id &func.
functions return one of the following manifest constants
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
    int heap_status;
    char *buffer;
.exmp break
    buffer = (char *)malloc( 80 );
    malloc( 1024 );
    free( buffer );
    heap_status = _heapset( 0xff );
    switch( heap_status ) {
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
