.func begin _heapshrink Functions
.func2 _heapshrink
.func2 _bheapshrink
.func2 _fheapshrink
.func2 _nheapshrink
.func gen
.ixfunc2 '&Heap' _heapshrink
.ixfunc2 '&Heap' _bheapshrink
.ixfunc2 '&Heap' _fheapshrink
.ixfunc2 '&Heap' _nheapshrink
.ixfunc2 '&Memory' _heapshrink
.ixfunc2 '&Memory' _bheapshrink
.ixfunc2 '&Memory' _fheapshrink
.ixfunc2 '&Memory' _nheapshrink
#include <malloc.h>
int  _heapshrink( void );
int _bheapshrink( __segment seg );
int _fheapshrink( void );
int _nheapshrink( void );
.func end
.desc begin
The &func functions attempt to shrink the heap to its smallest
possible size by returning all free entries at the end of the heap
back to the system.
This can be used to free up as much memory as possible before using the
.kw system
function or one of the
.kw spawn
functions.
.np
The various &func functions shrink the following heaps:
.begterm 12
.termhd1 Function
.termhd2 Heap Shrinked
.term _heapshrink
Depends on data model of the program
.term _bheapshrink
Based heap specified by
.arg seg
value;
.kw _NULLSEG
specifies all based heaps
.term _fheapshrink
Far heap (outside the default data segment)
.term _nheapshrink
Near heap (inside the default data segment)
.endterm
.np
In a small data memory model, the &func function is equivalent to the
.kw _nheapshrink
function; in a large data memory model, the &func function is
equivalent to the
.kw _fheapshrink
function.
It is identical to the
.kw _heapmin
function.
.desc end
.return begin
These functions return zero if successful, and non-zero if some error
occurred.
.return end
.see begin
.seelist &function. _heapchk _heapenable _heapgrow _heapmin _heapset _heapshrink _heapwalk
.see end
.exmp begin
#include <stdlib.h>
#include <malloc.h>

void main()
  {
    _heapshrink();
.if '&machsys' eq 'PP' .do begin
    system( "cd \\usr\\fred" );
.do end
.el .if '&machsys' eq 'QNX' .do begin
    system( "cd /home/fred" );
.do end
.el .do begin
    system( "chdir c:\\watcomc" );
.do end
  }
.im dblslash
.exmp end
.class WATCOM
.system
