#include "fail.h"

struct s
{
    union
    {
        struct
        {
            float a, b;
        };
        float c[2];
    };
};

s d = { 0.0, 0.1 };

main(void)
{
    if( d.a != d.c[0] ) _fail;
    if( d.b != d.c[1] ) _fail;
    if( d.a != 0.0 ) _fail;
    if( d.b != 0.1f ) _fail;
    _PASS;
}

