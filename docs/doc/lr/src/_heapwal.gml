.func begin _heapwalk Functions
.func2 _heapwalk
.func2 _bheapwalk
.func2 _fheapwalk
.func2 _nheapwalk
.func end
.synop begin
.ixfunc2 '&Heap' _heapwalk
.ixfunc2 '&Heap' _bheapwalk
.ixfunc2 '&Heap' _fheapwalk
.ixfunc2 '&Heap' _nheapwalk
.ixfunc2 '&Memory' _heapwalk
.ixfunc2 '&Memory' _bheapwalk
.ixfunc2 '&Memory' _fheapwalk
.ixfunc2 '&Memory' _nheapwalk
#include <malloc.h>
int  _heapwalk( struct _heapinfo *entry );
int _bheapwalk( __segment seg, struct _heapinfo *entry );
int _fheapwalk( struct _heapinfo *entry );
int _nheapwalk( struct _heapinfo *entry );

struct _heapinfo {
    void __far *_pentry;   /* heap pointer */
    size_t     _size;      /* heap entry size */
    int        _useflag;   /* heap entry 'in-use' flag */
};
#define _USEDENTRY      0
#define _FREEENTRY      1
.synop end
.desc begin
The
.id &funcb.
functions along with
.kw _heapchk
and
.kw _heapset
are provided for debugging heap related problems in programs.
.np
The
.id &funcb.
functions walk through the heap, one entry per call,
updating the
.kw _heapinfo
structure with information on the next heap entry.
The structure is defined in
.mono <malloc.h>.
You must initialize the
.arg _pentry
field with
.mono NULL
to start the walk through the heap.
.np
Each function walks a particular heap, as listed below:
.begterm 12
.termhd1 Function
.termhd2 Heap Walked
.term _heapwalk
Depends on data model of the program
.term _bheapwalk
Based heap specified by
.arg seg
value;
.mono _NULLSEG
specifies all based heaps
.term _fheapwalk
Far heap (outside the default data segment)
.term _nheapwalk
Near heap (inside the default data segment)
.endterm
.np
In a small data memory model, the
.id &funcb.
function is equivalent to the
.kw _nheapwalk
function; in a large data memory model, the
.id &funcb.
function is
equivalent to the
.kw _fheapwalk
function.
.desc end
.return begin
These functions return one of the following manifest constants which
are defined in
.mono <malloc.h>.
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term _HEAPOK
The heap is OK so far, and the
.kw _heapinfo
structure contains information about the next entry in the heap.
.term _HEAPEMPTY
The heap is empty.
.term _HEAPBADPTR
The
.kw _pentry
field of the
.arg entry
structure does not contain a valid pointer into the heap.
.term _HEAPBADBEGIN
The header information for the heap was not found or has been damaged.
.term _HEAPBADNODE
The heap contains a bad node, or is damaged.
.term _HEAPEND
The end of the heap was reached successfully.
.endterm
.return end
.see begin
.seelist _heapchk _heapenable _heapgrow _heapmin _heapset _heapshrink _heapwalk
.see end
.exmp begin
#include <stdio.h>
#include <malloc.h>

heap_dump()
  {
    struct _heapinfo h_info;
    int heap_status;

    h_info._pentry = NULL;
    for(;;) {
      heap_status = _heapwalk( &h_info );
      if( heap_status != _HEAPOK ) break;
      printf( "  %s block at %Fp of size %4.4X\n",
        (h_info._useflag == _USEDENTRY ? "USED" : "FREE"),
        h_info._pentry, h_info._size );
    }
.exmp break
    switch( heap_status ) {
    case _HEAPEND:
      printf( "OK - end of heap\n" );
      break;
    case _HEAPEMPTY:
      printf( "OK - heap is empty\n" );
      break;
    case _HEAPBADBEGIN:
      printf( "ERROR - heap is damaged\n" );
      break;
    case _HEAPBADPTR:
      printf( "ERROR - bad pointer to heap\n" );
      break;
    case _HEAPBADNODE:
      printf( "ERROR - bad node in heap\n" );
    }
  }
.exmp break
void main()
  {
    char *p;
    heap_dump();   p = (char *) malloc( 80 );
    heap_dump();   free( p );
    heap_dump();
  }
.exmp output
.blktext begin
On 16-bit 80x86 systems, the following output is produced:
.blktext end
.blkcode begin
.if '&machsys' eq 'QNX' .do begin
  USED block at 000c:0c06 of size 0008
  USED block at 000c:0c0e of size 0022
  USED block at 000c:0c30 of size 0402
  FREE block at 000c:1032 of size 1BCC
OK - end of heap
  USED block at 000c:0c06 of size 0008
  USED block at 000c:0c0e of size 0022
  USED block at 000c:0c30 of size 0402
  USED block at 000c:1032 of size 0052
  FREE block at 000c:1084 of size 1B7A
OK - end of heap
  USED block at 000c:0c06 of size 0008
  USED block at 000c:0c0e of size 0022
  USED block at 000c:0c30 of size 0402
  FREE block at 000c:1032 of size 1BCC
OK - end of heap
.do end
.el .do begin
OK - heap is empty
  USED block at 23f8:0ab6 of size 0202
  USED block at 23f8:0cb8 of size 0052
  FREE block at 23f8:0d0a of size 1DA2
OK - end of heap
  USED block at 23f8:0ab6 of size 0202
  FREE block at 23f8:0cb8 of size 1DF4
OK - end of heap
.do end
.blkcode end
.blktext begin
On 32-bit 80386/486 systems, the following output is produced:
.blktext end
.blkcode begin
OK - heap is empty
  USED block at 0014:00002a7c of size 0204
  USED block at 0014:00002c80 of size 0054
  FREE block at 0014:00002cd4 of size 1D98
OK - end of heap
  USED block at 0014:00002a7c of size 0204
  FREE block at 0014:00002c80 of size 1DEC
OK - end of heap
.blkcode end
.exmp end
.class WATCOM
.system
