#include "fail.h"

#if __WATCOMC__ > 1060

#include <stddef.h>

struct Q {
    int b,a,d;
};

struct S {
    Q a[5][5][2];
};

struct T {
    S b[10];
};

struct V {
    T c[10];
};

struct D : virtual V {
    Q q;
    int filler[10];
};

struct E {
    int filler[20];
};

struct F : D, E {
    int filler[10];
};

unsigned d[] = {
    offsetof( F, c[3] ),
    offsetof( F, q.d ),
    offsetof( F, c[2].b[3].a[3][2][1].d ),
};

void check( unsigned line, unsigned i, void *p, void *q )
{
    if( d[i] != ((char*)q-(char*)p)) fail(line);
}

F x;

int main() {
    check( __LINE__, 0, &(x), &(x.c[3]) );
    check( __LINE__, 1, &(x), &(x.q.d) );
    check( __LINE__, 2, &(x), &(x.c[2].b[3].a[3][2][1].d) );
    _PASS;
}

#else

ALWAYS_PASS

#endif
