/* Check _Bool type conversion related warnings. Warnings must be emitted
 * before the conversion is done so that the original type is reported.
 */

#include <stdbool.h>

int foo( void )
{
    char    *pc;
    bool    bl = false;
    struct bit2 {
        bool b1;
        bool b2 : 1;
    } bits = { false, false };

    pc = false;
    pc = bl;
    bl = pc;        /* Valid, no warning! */
    pc = bits.b1;
    pc = bits.b2;

    return( pc );
}
