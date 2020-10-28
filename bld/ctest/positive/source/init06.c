#include "fail.h"

/* Compound literal examples */
const char *aaa = (const char []){"aaa"};
int *iii        = (int []){3, 6, 9};

struct s { int i; };

int f(void)
{
    struct s *p = 0, *q;
    int j = 0;

again:
    q = p, p = &((struct s){ j++ });
    if (j < 2) goto again;

    return p == q && q->i == 1;
}

struct point { int x, y; };

void drawline(struct point a, struct point b)
{
    if (a.x != 1) _fail;
    if (a.y != 1) _fail;
    if (b.x != 3) _fail;
    if (b.y != 4) _fail;
}

int main(void)
{
    int *p = iii;

    if (aaa[0] != 'a') _fail;
    if (aaa[1] != 'a') _fail;
    if (aaa[2] != 'a') _fail;
    if (aaa[3])        _fail;
    if (iii[0] != 3)   _fail;
    if (iii[1] != 6)   _fail;
    if (iii[2] != 9)   _fail;

    p = (int [2]){*p};
    if (*p != *iii)    _fail;
    if (p[1])          _fail;

    if (f() != 1)      _fail;
    drawline((struct point){.x=1, .y=1},
             (struct point){.x=3, .y=4});

    _PASS;
}

