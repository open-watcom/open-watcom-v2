.func lfind
.synop begin
#include <search.h>
void *lfind( const void *key, /* object to search for  */
             const void *base,/* base of search data   */
             unsigned *num,   /* number of elements    */
             unsigned width,  /* width of each element */
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
The
.id &funcb.
function returns a pointer to the array element in
.arg base
that matches
.arg key
if it is found, otherwise
.mono NULL
is returned indicating that the
.arg key
was not found.
.return end
.see begin
.seelist lfind bsearch lsearch
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

static const char *keywords[] = {
        "auto",
        "break",
        "case",
        "char",
        /* . */
        /* . */
        /* . */
        "while"
};
.exmp break
void main( int argc, const char *argv[] )
  {
    unsigned num = 5;
    extern int compare( const void *, const void * );

    if( argc <= 1 ) exit( EXIT_FAILURE );
    if( lfind( &argv[1], keywords, &num, sizeof(char **),
                    compare ) == NULL ) {
      printf( "'%s' is not a C keyword\n", argv[1] );
      exit( EXIT_FAILURE );
    } else {
      printf( "'%s' is a C keyword\n", argv[1] );
      exit( EXIT_SUCCESS );
    }
  }
.exmp break
int compare( const void *op1, const void *op2 )
  {
    const char **p1 = (const char **) op1;
    const char **p2 = (const char **) op2;
    return( strcmp( *p1, *p2 ) );
  }
.exmp end
.class WATCOM
.system
