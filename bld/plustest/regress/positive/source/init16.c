#include "fail.h"

#if __WATCOM_REVISION__ >= 7

typedef union LI {
    struct {
	unsigned long LowPart;
	signed long HighPart;
    };
    struct {
	unsigned long lo;
	signed long hi;
    } u;
    double QuadPart;
} LI;
LI li = { 33, 333 };

union U {
    struct {
	int x,y;
    };
    int z;
};

U v = { 1, 2 };
#endif

struct s {
    int x,y;
};
struct x {
    s u;
    int z;
};
x v2 = { 1, 2, 3 };

int main() {
#if __WATCOM_REVISION__ >= 7
    if( li.u.lo != 33 ) fail(__LINE__);
    if( li.u.hi != 333 ) fail(__LINE__);
    if( v.x != 1 ) fail(__LINE__);
    if( v.y != 2 ) fail(__LINE__);
    if( v.z != 1 ) fail(__LINE__);
#endif
    if( v2.u.x != 1 ) fail(__LINE__);
    if( v2.u.y != 2 ) fail(__LINE__);
    if( v2.z != 3 ) fail(__LINE__);
    _PASS;
}
