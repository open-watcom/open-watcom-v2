#include "fail.h"
#include <stdio.h>
#include <stdlib.h>

#include "link3.h"

int (*returns_COMDAT())( int, int )
{
    return COMDAT_fn;
}

extern int (*returns_PUBDEF())( int, int );

int main()
{
    if( returns_PUBDEF() != returns_COMDAT() ) fail(__LINE__);
    _PASS;
}
