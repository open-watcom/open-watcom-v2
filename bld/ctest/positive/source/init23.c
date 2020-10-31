#include "fail.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* Initialization involving flexible array members */

typedef struct {
    int     i;
} A;

typedef struct {
    long    l;
    A       a[];
} B;

static B b1 = { 6, { {0}, {1}, {2}, {3}, {4}, {5} } };
static B b2 = { 2, { {0}, {1} } };

int main(void) {
    long i;

    if (sizeof(B) != sizeof(long))     _fail;
    if (offsetof(B,a) != sizeof(long)) _fail;

    for (i = 0; i < b1.l; i++) {
        if (b1.a[i].i != i)
            _fail;
    }

    for (i = 0; i < b2.l; i++) {
        if (b2.a[i].i != i)
            _fail;
    }

    _PASS;
}
