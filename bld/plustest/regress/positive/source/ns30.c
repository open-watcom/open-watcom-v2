#include "fail.h"

#if __WATCOM_REVISION__ >= 8
namespace JavaTools
{
};
namespace CF
{
    using namespace JavaTools;
};
namespace CF
{
    using namespace JavaTools;
};
namespace CF
{
    using namespace JavaTools;
};
namespace CF
{
    using namespace JavaTools;
};
namespace CF
{
    using namespace JavaTools;
};
namespace CF
{
    using namespace JavaTools;
};
using namespace JavaTools;
namespace
{
};
using namespace CF;

char a[1000];
unsigned curr = 0;

void * __cdecl operator new( unsigned x ) {
    unsigned off = curr;
    curr += x;
    return &a[off];
}
void  __cdecl operator delete( void *p ) {
    curr = (char*)p - a;
}
void * __cdecl operator new[]( unsigned x ) {
    unsigned off = curr;
    curr += x;
    return &a[off];
}
void  __cdecl operator delete[]( void *p ) {
    curr = (char*)p - a;
}

struct S {
    int x;
};

int main() {
    S v;

    v.x = 0;
    ((int&)v.x) |= 1;
    if( v.x != 1 ) _fail;
    ((int&)v.x) |= 2;
    if( v.x != 3 ) _fail;
    int *p = new int(1);
    if( *p != 1 ) _fail;
    if( p != (void*)&a[0] ) _fail;
    int *q = new int[10];
    if( *q != 0 ) _fail;
    if( q != (void*)&a[sizeof(int)] ) _fail;
    delete [] q;
    if( curr != sizeof(int) ) _fail;
    delete p;
    if( curr != 0 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
