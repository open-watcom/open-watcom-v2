#include "link3.h"

int (*returns_PUBDEF())( int, int )
{
    return COMDAT_fn;
}

int COMDAT_fn(int x,int y)
{
    return x ^ y;
}

int (*returns_PRIDEF())( int, int )
{
    return COMDAT_fn;
}
