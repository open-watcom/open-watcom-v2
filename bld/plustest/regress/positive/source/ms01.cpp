// MS VC++ compatibility
#include "fail.h"

struct T {
    int a;
    int b;
} a[] = { { 1, 1 }, { 2, 2 } };

unsigned x() {
#if __WATCOM_REVISION__ >= 8
    return sizeof a / sizeof T;
#else
    return sizeof a / sizeof(T);
#endif
}

int main() {
    if( x() != 2 ) _fail;
    _PASS;
}
