#include "link3.h"

int COMDAT_fn(int x,int y)
{
    return x ^ y;
}

int (*returns_PUBDEF())( int, int )
{
    return COMDAT_fn;
}
