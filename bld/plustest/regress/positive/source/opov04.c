#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace N {
    int ok;
    struct O {
	friend void operator <<( O const &, int );
    };
    void operator <<( O const &, int v )
    {
	ok = v;
    }
};

int main() {
    N::O x;

    x << __LINE__;
    if( N::ok != (__LINE__-1) ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
