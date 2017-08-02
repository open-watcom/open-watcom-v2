#include "fail.h"

struct s2 {
    int _f;
    int *a;
};
struct s1 {
    int _f;
    struct s2 *m;
};

int bad( int i, struct s1 *p, int k, int l ) {
    int *x = p->m->a;
    ++x;
    return x[i];
}

int ok( int i, struct s1 *p, int k, int l ) {
    return (p->m->a+1)[i];
}

int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

struct s2 x2 = { -1, a };
struct s1 x1 = { -1, &x2 };

int main() {
    int i;
    for( i = 0; i < 6; ++i ) {
        if( bad( i, &x1, -2, -3 ) != ( i + 2 ) ) _fail;
        if( ok( i, &x1, -2, -3 ) != ( i + 2 ) ) _fail;
    }
    _PASS;
}
