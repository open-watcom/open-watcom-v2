#include "fail.h"

struct U {
    struct {
        int i;
        int j;
    };
    union {
        int k;
        int l;
    };
};
struct U v = { 256, 257, 258 };

int main() {
    if( v.i != 256 ) fail(__LINE__);
    if( v.j != 257 ) fail(__LINE__);
    if( v.k != 258 ) fail(__LINE__);
    if( v.l != 258 ) fail(__LINE__);
    _PASS;
}
