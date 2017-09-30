#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace n1 {
    int foo( int x );
};
namespace n2 { using namespace n1; };
namespace n3 { using namespace n2; };
namespace n4 { using namespace n3; };
namespace n5 {
    using namespace n4;
    int foo( char );
};
int n5::foo( char x )
{
    return x;
}
int n1::foo( int x )
{
    return x + 1;
}

int i;

int main() {
    using namespace n5;
    if( foo( __LINE__) != (__LINE__+1) ) fail(__LINE__);
    if( foo( 'a' ) != 'a' ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
