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

/* NB: If 0.1 is used instead of 0.25, the comparison may fail due
 * to a precision loss caused by a double -> float -> double conversion.
 */
struct s d = { 0.0, 0.25 };

main(void)
{
    if( d.a != d.c[0] ) _fail;
    if( d.b != d.c[1] ) _fail;
    if( d.a != 0.0 ) _fail;
    if( d.b != 0.25 ) _fail;
    _PASS;
}
