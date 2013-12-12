.func sbrk
.synop begin
#include <stdlib.h>
void *sbrk( int increment );
.ixfunc2 '&Memory' &func
.synop end
.desc begin
.if '&machsys' ne 'QNX' .do begin
Under 16-bit DOS and Phar Lap's 386|DOS-Extender, the data segment is
grown contiguously.
.do end
The "break" value is the address of the first byte of unallocated
memory.
When a program starts execution, the break value is placed following
the code and constant data for the program.
As memory is allocated, this pointer will advance when there is no
freed block large enough to satisfy an allocation request.
The &func function can be used to set a new "break" value for the
program by adding the value of
.arg increment
to the current break value.
This increment may be positive or negative.
.if '&machsys' ne 'QNX' .do begin
.np
Under other systems, heap allocation is discontiguous.
The &func function can only be used to allocate additional
discontiguous blocks of memory.
The value of
.arg increment
is used to determine the minimum size of the block to be allocated and
may not be zero or negative.
The actual size of the block that is allocated is rounded up to a
multiple of 4K.
.do end
.np
The variable
.kw _amblksiz
defined in
.hdrfile stdlib.h
contains the default increment by which the "break" pointer for memory
allocation will be advanced when there is no freed block large enough
to satisfy a request to allocate a block of memory.
This value may be changed by a program at any time.
.if '&machsys' ne 'QNX' .do begin
.np
Under 16-bit DOS, a new process started with one of the
.kw spawn...
or
.kw exec...
functions is loaded following the break value.
Consequently, decreasing the break value leaves more space available to
the new process.
Similarly, for a resident program (a program which remains in memory
while another program executes),
increasing the break value will leave more space available to be
allocated by the resident program after other programs are loaded.
.do end
.desc end
.return begin
If the call to &func succeeds, a pointer to the start of the new block
of memory is returned.
.if '&machsys' ne 'QNX' .do begin
Under 16-bit DOS, this corresponds to the old break value.
.do end
If the call to &func fails, &minus.1 is returned.
.im errnoref
.return end
.see begin
.im seealloc sbrk
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

#if defined(M_I86)
#define alloc( x, y ) sbrk( x ); y = sbrk( 0 );
#else
#define alloc( x, y ) y = sbrk( x );
#endif
.exmp break
void main()
 {
    void *brk;
.exmp break
#if defined(M_I86)
    alloc( 0x0000, brk );
    /* calling printf will cause an allocation */
    printf( "Original break value %p\n", brk );
    printf( "Current amblksiz value %x\n", _amblksiz );
    alloc( 0x0000, brk );
    printf( "New break value after printf \t\t%p\n", brk );
#endif
    alloc( 0x3100, brk );
    printf( "New break value after sbrk( 0x3100 ) \t%p\n",
            brk );
    alloc( 0x0200, brk );
    printf( "New break value after sbrk( 0x0200 ) \t%p\n",
            brk );
#if defined(M_I86)
    alloc( -0x0100, brk );
    printf( "New break value after sbrk( -0x0100 ) \t%p\n",
            brk );
#endif
 }
.exmp end
.class WATCOM
.system
