.func _bheapseg
#include <malloc.h>
__segment _bheapseg( size_t size );
.ixfunc2 '&Memory' _bheapseg
.funcend
.desc begin
The &func function allocates a based-heap segment of at least
.arg size
bytes.
.np
The argument
.arg size
indicates the initial size for the heap.
The heap will automatically be enlarged as needed if there is not enough
space available within the heap to satisfy an allocation request by
.kw _bcalloc
.ct,
.kw _bexpand
.ct,
.kw _bmalloc
.ct,
or
.kw _brealloc
.ct .li .
.np
The value returned by &func is the segment value or selector for the
based heap.
This value must be saved and used as an argument to other based heap
functions to indicate which based heap to operate upon.
.np
Each call to &func allocates a new based heap.
.desc end
.return begin
The value returned by &func is the segment value or selector for the
based heap.
This value must be saved and used as an argument to other based heap
functions to indicate which based heap to operate upon.
A special value of
.kw _NULLSEG
is returned if the segment could not be allocated.
.return end
.see begin
.if &e'&dohelp eq 0 .do begin
.seelist _bheapseg _bfreeseg _bcalloc _bexpand _bmalloc _brealloc
.do end
.el .do begin
.seelist _bheapseg _bfreeseg calloc _expand malloc realloc
.do end
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

struct list {
    struct list __based(__self) *next;
    int         value;
};
.exmp break
void main()
  {
    int         i;
    __segment   seg;
    struct list __based(seg) *head;
    struct list __based(seg) *p;
.exmp break
    /* allocate based heap */
    seg = _bheapseg( 1024 );
    if( seg == _NULLSEG ) {
      printf( "Unable to allocate based heap\n" );
      exit( 1 );
    }
.exmp break
    /* create a linked list in the based heap */
    head = 0;
    for( i = 1; i < 10; i++ ) {
      p = _bmalloc( seg, sizeof( struct list ) );
      if( p == _NULLOFF ) {
        printf( "_bmalloc failed\n" );
        break;
      }
      p->next = head;
      p->value = i;
      head = p;
    }
.exmp break
    /* traverse the linked list, printing out values */
    for( p = head; p != 0; p = p->next ) {
      printf( "Value = %d\n", p->value );
    }
.exmp break
    /* free all the elements of the linked list */
    for( ; p = head; ) {
      head = p->next;
      _bfree( seg, p );
    }
    /* free the based heap */
    _bfreeseg( seg );
  }
.exmp end
.class WATCOM
.system
