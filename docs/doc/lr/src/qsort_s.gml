.func qsort_s
.synop begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdlib.h>
errno_t qsort_s( void *base,
                 rsize_t nmemb,
                 rsize_t size,
    int (*compar)( const void *x, const void *y, void *context ),
                 void *context );
.synop end
.*
.rtconst begin
Neither
.arg nmemb
nor
.arg size
shall be greater than
.kw RSIZE_MAX
.ct .li .
If
.arg nmemb
is not equal to zero, then neither
.arg base
nor
.arg compar
shall be a null pointer.
If there is a runtime-constraint violation, the
.id &funcb.
function does not sort the array.
.rtconst end
.*
.desc begin
The
.id &funcb.
function sorts an array of
.arg nmemb
objects, the initial element of which is
pointed to by
.arg base
.ct .li .
The size of each object is specified by
.arg size
.ct .li .
The contents of the array are sorted into ascending order according to a comparison
function pointed to by
.arg compar
.ct , which is called with three arguments. The first two
point to the objects being compared. The function shall return an integer less than, equal
to, or greater than zero if the first argument is considered to be respectively less than,
equal to, or greater than the second. The third argument to the comparison function is the
.arg context
argument passed to
.id &funcb.
The sole use of
.arg context
by
.id &funcb.
is to pass it to the comparison function.
If two elements compare as equal, their relative order in the resulting sorted array is
unspecified.
.desc end
.return begin
The
.id &funcb.
function returns zero if there was no runtime-constraint violation.
Otherwise, a non-zero value is returned.
.return end
.see begin
.seelist qsort_s qsort bsearch bsearch_s
.see end
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *CharVect[] = { "last", "middle", "first" };
.exmp break
int compare( const void *op1, const void *op2, void *context )
{
    const char **p1 = (const char **) op1;
    const char **p2 = (const char **) op2;
    return( strcmp( *p1, *p2 ) );
}
.exmp break
void main()
{
    void * context = NULL;
    qsort_s( CharVect, sizeof(CharVect)/sizeof(char *),
          sizeof(char *), compare, context );
    printf( "%s %s %s\n",
            CharVect[0], CharVect[1], CharVect[2] );
}
.exmp output
first last middle
.exmp end
.class TR 24731
.system
