// REVISIONS: BASED01
//
// 93/10/30 -- J.W.Welch        -- test applies only to 16-bit


#include "fail.h"
#include <malloc.h>
#include <string.h>

#ifdef _M_I86

#define N 10

__segment aseg;
#define aseg_b __based(aseg)

typedef struct S S;
struct S {
    unsigned    filler1;
    char        len;
    char        filler2[3];
    char        data[20];
};

typedef struct R R;
struct R {
    R __based((__segment)__self) *next;
    S __based(aseg) *link;
};

char aseg_b *test1( S aseg_b *p )
{       
    return( p->data + p->len );
}

int test2( S aseg_b *base, S aseg_b *entry )
{       
    return( entry - base );
}

char test3( S aseg_b *p, int i )
{
    return( p->data[i] );
}

int main()
{
    S aseg_b *p;
    char aseg_b *q;
    __segment alt_seg;
#define alt_seg_b __based(alt_seg)
    R alt_seg_b *z;
    R alt_seg_b *y;

    aseg = _bheapseg( 1000 );
    p = (S aseg_b *) _bmalloc( aseg, sizeof( *p ) * N );
    _fmemset( p, 0xff, sizeof( *p ) * N );
    p->len = 4;
    _fstrcpy( p->data, "1234" );
    q = test1( p );
    if( *q != '\0' || q[-1] != '4' ) fail(__LINE__);
    if( test2( p, &p[2] ) != 2 ) fail(__LINE__);
    if( test2( &p[2], p ) != -2 ) fail(__LINE__);
    if( test2( p, p + 3 ) != 3 ) fail(__LINE__);
    if( test2( p + 3, p ) != -3 ) fail(__LINE__);
    if( test3( p, 0 ) != '1' ) fail(__LINE__);
    if( test3( p, 1 ) != '2' ) fail(__LINE__);
    if( test3( p, 2 ) != '3' ) fail(__LINE__);
    if( test3( p, 3 ) != '4' ) fail(__LINE__);
    if( test3( p, 4 ) != '\0' ) fail(__LINE__);
    if( test3( p, 5 ) != (char) 0xff ) fail(__LINE__);
    alt_seg = _bheapseg( 1000 );
    z = (R alt_seg_b *) _bmalloc( alt_seg, sizeof( *z ) );
    _fmemset( z, 0xff, sizeof( *z ) );
    y = (R alt_seg_b *) _bmalloc( alt_seg, sizeof( *y ) );
    _fmemset( y, 0xff, sizeof( *y ) );
    z->next = (R __based((__segment)__self) *) y;
    z->link = p + 1;
    y->next = (R __based((__segment)__self) *) z;
    y->link = p;
    if( z->next != y ) fail(__LINE__);
    if( z->next->next != z ) fail(__LINE__);
    if( y->next != z ) fail(__LINE__);
    if( y->next->next != y ) fail(__LINE__);
    if( z->link->data[0] != (char) 0xff ) fail(__LINE__);
    if( y->link->data[0] != '1' ) fail(__LINE__);
    if( y->next->link->data[0] != (char) 0xff ) fail(__LINE__);
    if( z->next->link->data[0] != '1' ) fail(__LINE__);
    _PASS;
}

#else

ALWAYS_PASS

#endif
