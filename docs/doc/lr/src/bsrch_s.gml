.func bsearch_s
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdlib.h>
void *bsearch_s( const void *key,
                 const void *base,
                 rsize_t nmemb,
                 rsize_t size,
    int (*compar)( const void *k, const void *y, void *context ),
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
is not equal to zero, then none of
.arg key, base
.ct , or
.arg compar
shall be a null pointer.
If there is a runtime-constraint violation, the &func function does not
search the array.
.rtconst end
.*
.desc begin

The &func function searches an array of
.arg nmemb
objects, the initial element of
which is pointed to by
.arg base
.ct , for an element that matches the object pointed to by
.arg key
.ct .li .
The size of each element of the array is specified by
.arg size
.ct .li .
The comparison function pointed to by
.arg compar
is called with three arguments. The first
two point to the key object and to an array element, in that order. The function shall
return an integer less than, equal to, or greater than zero if the key object is considered,
respectively, to be less than, to match, or to be greater than the array element. The array
shall consist of: all the elements that compare less than, all the elements that compare
equal to, and all the elements that compare greater than the key object, in that order.
The third argument to the comparison function is the
.arg context
argument passed to &func.
The sole use of context by &funcs is to pass it to the comparison function.

.desc end
.*
.return begin
The &func function returns a pointer to a matching element of the array,or a null
pointer if no match is found or there is a runtime-constraint violation. If two elements
compare as equal, which element is matched is unspecified.
.return end
.see begin
.seelist bsearch_s bsearch lfind lsearch qsort qsort_s
.see end
.exmp begin
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
static void * context = NULL;
.exmp break
#define NUM_KW  sizeof(keywords) / sizeof(char *)
.exmp break
int kw_compare( const void *p1, const void *p2, void *context )
{
    const char *p1c = (const char *) p1;
    const char **p2c = (const char **) p2;
    return( strcmp( p1c, *p2c ) );
}
.exmp break
int keyword_lookup( const char *name )
{
    const char **key;
    key = (char const **) bsearch_s( name, keywords, NUM_KW,
                   sizeof( char * ),  kw_compare, context );
    if( key == NULL ) return( -1 );
    return key - keywords;
}
.exmp break
int main()
{
    printf( "%d\n", keyword_lookup( "case" ) );
    printf( "%d\n", keyword_lookup( "crigger" ) );
    printf( "%d\n", keyword_lookup( "auto" ) );
    return 0;
}
//************ Sample program output ************
//2
//-1
//0
.exmp output
2
-1
0
.exmp end
.class TR 24731
.system
