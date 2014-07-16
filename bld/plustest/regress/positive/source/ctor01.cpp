#include "fail.h"

// 96/04/12 -- J.W.Welch    -- cast to references now should fail

int error_occurred;

struct V {
    V();
};
V::V() {
}
struct D : virtual V {
    D(int);
};
D::D( int x ) {
    if( x != 0xaaaa ) {
        fail(__LINE__);
        ++error_occurred;
    }
}

int zap_parm_regs( int a, int b, int c, int d )
{
    return a + b + c + d;
}

void foo( D const &, unsigned line )
{
    if( error_occurred ) {
        fail(line);
        error_occurred = 0;
    }
}

int main()
{
    zap_parm_regs( -1, -2, -3, -4 );
#if __WATCOM_REVISION__ < 8
    foo( (D&)0xaaaa, __LINE__ );
    zap_parm_regs( -1, -2, -3, -4 );
#endif
    foo( (D)0xaaaa, __LINE__);
    zap_parm_regs( -1, -2, -3, -4 );
    foo( (D)0xaaaa, __LINE__);
    zap_parm_regs( -1, -2, -3, -4 );
#if __WATCOM_REVISION__ < 8
    foo( (D&)0xaaaa, __LINE__);
    zap_parm_regs( -1, -2, -3, -4 );
#endif
    _PASS;
}
