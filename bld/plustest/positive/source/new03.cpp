#include "fail.h"
#include <stdio.h>
#include <stdlib.h>

float *alias(float & dst, const float & src)
{
    return new (&dst) float(src);
}
int *alias(int & dst, const int & src)
{
    return new (&dst) int(src);
}
char *alias(char & dst, const char & src)
{
    return new (&dst) char(src);
}
struct S { int a; int b; int c; };
typedef int S::* MP;
MP *alias(MP & dst, const MP & src)
{
    return new (&dst) MP(src);
}

int main()
{
    float y1 = 2.3;
    float x1, *px1;
    px1 = alias( x1, y1 );
    if( px1 != &x1 || *px1 != y1 || *px1 != x1 || x1 != y1 ) fail(__LINE__);
    char y2 = '@';
    char x2, *px2;
    px2 = alias( x2, y2 );
    if( px2 != &x2 || *px2 != y2 || *px2 != x2 || x2 != y2 ) fail(__LINE__);
    int y3 = '@';
    int x3, *px3;
    px3 = alias( x3, y3 );
    if( px3 != &x3 || *px3 != y3 || *px3 != x3 || x3 != y3 ) fail(__LINE__);
    MP y4 = &S::b;
    MP x4, *px4;
    px4 = alias( x4, y4 );
    if( px4 != &x4 || *px4 != y4 || *px4 != x4 || x4 != y4 ) fail(__LINE__);
    _PASS;
}
