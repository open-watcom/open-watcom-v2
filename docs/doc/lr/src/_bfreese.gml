.func _bfreeseg
.synop begin
#include <malloc.h>
int _bfreeseg( __segment seg );
.ixfunc2 '&Memory' _bfreeseg
.synop end
.desc begin
The
.id &func.
function frees a based-heap segment.
.pp
The argument
.arg seg
indicates the segment returned by an earlier call to
.kw _bheapseg
.ct .li .
.desc end
.return begin
The
.id &func.
function returns 0 if successful and -1 if an error occurred.
.return end
.see begin
.if &e'&dohelp eq 0 .do begin
.seelist _bcalloc _bexpand _bfree _bheapseg _bmalloc _brealloc
.do end
.el .do begin
.seelist _bheapseg calloc _expand free malloc realloc
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
