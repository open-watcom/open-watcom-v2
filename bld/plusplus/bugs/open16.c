#include <stdlib.h>
#include <stdio.h>

struct S {
    S(){ printf("ctor\n"); }
    ~S(){ printf("dtor\n"); }
};

void main()
{
    S x;        // 'x' will not be destructed

    exit(0);
}
