.func lsearch
.synop begin
#include <search.h>
void *lsearch( const void *key, /* object to search for */
               void *base,      /* base of search data  */
               unsigned *num,   /* number of elements   */
               unsigned width,  /* width of each element*/
               int (*compare)( const void *element1,
                               const void *element2 ) );
.ixfunc2 '&Search' &funcb
.synop end
.desc begin
The
.id &funcb.
function performs a linear search for the value
.arg key
in the array of
.arg num
elements pointed to by
.arg base
.ct .li .
Each element of the array is
.arg width
bytes in size.
The argument
.arg compare
is a pointer to a user-supplied routine that will be called by &funcb
to determine the relationship of an array element with the
.arg key
.ct .li .
One of the arguments to the
.arg compare
function will be an array element, and the other will be
.arg key
.ct .li .
.pp
The
.arg compare
function should return 0 if
.arg element1
is identical to
.arg element2
and non-zero if the elements are not identical.
.desc end
.return begin
If the
.arg key
value is not found in the array, then it is added to the end
of the array and the number of elements is incremented.
The
.id &funcb.
function returns a pointer to the array element in
.arg base
that matches
.arg key
if it is found, or the newly added key if it was not found.
.return end
.see begin
.seelist lsearch bsearch lfind
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

void main( int argc, const char *argv[] )
  {
    int i;
    unsigned num = 0;
    char **array = (char **)calloc( argc, sizeof(char **) );
    extern int compare( const void *, const void * );

    for( i = 1; i < argc; ++i ) {
      lsearch( &argv[i], array, &num, sizeof(char **),
                  compare );
    }
    for( i = 0; i < num; ++i ) {
      printf( "%s\n", array[i] );
    }
  }
.exmp break
int compare( const void *op1, const void *op2 )
  {
    const char **p1 = (const char **) op1;
    const char **p2 = (const char **) op2;
    return( strcmp( *p1, *p2 ) );
  }
.exmp break
/* With input: one two one three four */
.exmp output
one
two
three
four
.exmp end
.class WATCOM
.system
