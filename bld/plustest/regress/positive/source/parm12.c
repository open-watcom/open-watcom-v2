#include "fail.h"

/* see 8.3.5 Functions
 *
 * (3) [...] After producing the list of parameter types, several
 * transformations take place upon these types to determine the
 * function type. Any cv-qualifier modifying a parameter type is
 * deleted.
*/

int f(const int a);
int f(int a) {
    a = 0;
    return a;
}

int g(const int *a) {
    return 0;
}
int g(int *a) {
    return 1;
}

int h(int a);
int h(const int a) {
    return g(&a);
}


int main() {
    if( f(1) != 0 ) fail( __LINE__ );
    if( h(1) != 0 ) fail( __LINE__ );

    _PASS;
}
