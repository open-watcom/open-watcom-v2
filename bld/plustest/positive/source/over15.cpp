#include "fail.h"

int main()
{
    int x = 1, y = 2, z;

    z = ( int & ) x == y;
    z = x == ( int & ) y;
    z = ( int & ) x != y;
    z = x != ( int & ) y;
    z = ( int & ) x < y;
    z = x < ( int & ) y;
    z = ( int & ) x > y;
    z = x > ( int & ) y;

    z = ( int & ) x + y;
    z = x + ( int & ) y;
    z = ( int & ) x - y;
    z = x - ( int & ) y;
    z = ( int & ) x * y;
    z = x * ( int & ) y;
    z = ( int & ) x / y;
    z = x / ( int & ) y;
    z = ( int & ) x % y;
    z = x % ( int & ) y;

    z = ( int & ) x ^ y;
    z = x ^ ( int & ) y;
    z = ( int & ) x & y;
    z = x & ( int & ) y;
    z = ( int & ) x | y;
    z = x | ( int & ) y;

    z = ( int & ) x << y;
    z = x << ( int & ) y;
    z = ( int & ) x >> y;
    z = x >> ( int & ) y;

    _PASS;
}
