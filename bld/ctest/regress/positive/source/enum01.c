#include "fail.h"

enum A {
    c1 = 120
};

const int c2 = 120;

unsigned long b1()
{
    double a = 1.25;
    return a * c1;
}

unsigned long b2()
{
    double a = 1.25;
    return c1 * a;
}

unsigned long b3()
{
    float a = 1.25;
    return a * c1;
}

unsigned long b4()
{
    float a = 1.25;
    return c1 * a;
}

unsigned long ok()
{
    double a = 1.25;
    return a * c2;
}

int main()
{
    if( b1() != 150 ) fail(__LINE__);
    if( b2() != 150 ) fail(__LINE__);
    if( b3() != 150 ) fail(__LINE__);
    if( b4() != 150 ) fail(__LINE__);
    if( ok() != 150 ) fail(__LINE__);
    _PASS;
}
