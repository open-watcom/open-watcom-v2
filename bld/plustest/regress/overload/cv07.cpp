#include "dump.h"

typedef int A3[3];
typedef A3 A2_A3[2];
const A2_A3 a = {
    { 1, 2, 3 },
    { 4, 5, 6 },
};

void foo( A3[3] ) { GOOD; };
void foo( const A3[3] ) { GOOD; };	// should call this function

int main() {
    foo( a );
    CHECK_GOOD(11);
    return errors != 0;
}
