#include "fail.h"

#ifdef __WATCOM_RTTI__

#define hier( x ) \
    struct V_U##x { \
	virtual void __u(){}; \
    }; \
    struct V_O##x { \
	virtual void __u(){}; \
    }; \
    struct V_I##x { \
	virtual void __u(){}; \
    }; \
    struct B_U##x { \
	virtual void __u(){}; \
    }; \
    struct B_O##x { \
	virtual void __u(){}; \
    }; \
    struct B_I##x { \
	virtual void __u(){}; \
    }; \
    struct X##x : \
	    public virtual V_U##x, \
	    private virtual V_I##x, \
	    protected virtual V_O##x, \
	    public B_U##x, \
	    private B_I##x, \
	    protected B_O##x \
    { \
	int _x[10]; \
    }; \
    struct Y##x : \
	    public virtual V_U##x, \
	    private virtual V_I##x, \
	    protected virtual V_O##x, \
	    public B_U##x, \
	    private B_I##x, \
	    protected B_O##x \
    { \
	int y[10]; \
    }; \
    struct Z##x : X##x, private Y##x { \
	int z[10]; \
    };

hier( 0 )
hier( 1 )

void check_z( void *p, unsigned line ) {
    if( p ) fail(line);
}
void check_nz( void *p, unsigned line ) {
    if( !p ) fail(line);
}

int main() {
    B_U0 *p = (X0*) new Z0;
    check_z( dynamic_cast< V_U1 * >( p ), __LINE__ );
    check_nz( dynamic_cast< V_U0 * >( p ), __LINE__ );
    _PASS;
}

#else

ALWAYS_PASS

#endif

