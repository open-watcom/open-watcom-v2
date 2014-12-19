.func qsort
.synop begin
#include <stdlib.h>
void qsort( void *base,
            size_t num,
            size_t width,
            int (*compar) ( const void *,
                            const void *) );
.synop end
.*
.safealt
.*
.desc begin
The
.id &funcb.
function sorts an array of
.arg num
elements, which is pointed to by
.arg base
.ct , using a modified version of Sedgewick's Quicksort algorithm.
Each element in the array is
.arg width
bytes in size.
The comparison function pointed to by
.arg compar
is called with two arguments that point to elements in the array.
The comparison function shall return an integer less than, equal to,
or greater than zero if the first argument is less than, equal to,
or greater than the second argument.
.np
The version of the Quicksort algorithm that is employed was proposed
by Jon Louis Bentley and M. Douglas McIlroy in the article
"Engineering a sort function" published in
.bi Software -- Practice and Experience,
23(11):1249-1265, November 1993.
.desc end
.return begin
The
.id &funcb.
function returns no value.
.return end
.see begin
.seelist qsort qsort_s bsearch bsearch_s
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *CharVect[] = { "last", "middle", "first" };
.exmp break
int compare( const void *op1, const void *op2 )
{
    const char **p1 = (const char **) op1;
    const char **p2 = (const char **) op2;
    return( strcmp( *p1, *p2 ) );
}
.exmp break
void main()
{
    qsort( CharVect, sizeof(CharVect)/sizeof(char *),
          sizeof(char *), compare );
    printf( "%s %s %s\n",
            CharVect[0], CharVect[1], CharVect[2] );
}
.exmp output
first last middle
.exmp end
.class ISO C
.system
