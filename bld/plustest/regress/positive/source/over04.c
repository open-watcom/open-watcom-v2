#include "fail.h"

int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

void *over_test(void *p) {
    return (int *)p + 1;
}

const void *over_test(const void *p) {
    return (int *)p + 2;
}

volatile void *over_test(volatile void *p) {
    return (int *)p + 3;
}

const volatile void *over_test(const volatile void *p)
{
    return (int *)p + 4;
}

int main()
{
    int *p = a;
    if( over_test(p) != (p + 1) ) fail(__LINE__);
    const int *pc = &a[1];
    volatile int *pv = &a[2];
    const volatile int *pcv = &a[3];
    if( over_test(pc) != (p + 3) ) fail(__LINE__);
    if( over_test(pv) != (p + 5) ) fail(__LINE__);
    if( over_test(pcv) != (p + 7) ) fail(__LINE__);
    _PASS;
}
