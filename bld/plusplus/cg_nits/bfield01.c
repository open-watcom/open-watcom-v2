// bitfield optimizations
typedef struct S {
    unsigned a : 1;
    unsigned b : 1;
    unsigned c : 1;
} S;

void foo( S *p )
{
    if( p->a || p->b ) {        // if(( p->unit & 0011b ) != 0 )
        p->c = 1;
    }
    if( p->a && p->b ) {        // if(( p->unit & 0011b ) == 0011b )
        p->c = 1;
    }
}
