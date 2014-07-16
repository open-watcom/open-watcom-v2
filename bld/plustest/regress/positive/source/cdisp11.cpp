#include "fail.h"
#include <stdlib.h>
#include <string.h>

#if __WATCOMC__ > 1060

#pragma inline_depth(0)

void *operator new( unsigned, void *p )
{
    return p;
}

int bar() {
    static int x;
    return x++;
}

struct V5;
V5 *thunk_OK;

struct V1 {
    int __u;
    virtual void __u( int ) {
    }
    virtual void __u( int ) {
    }
    virtual void __u( int ) {
    }
    virtual void __u( int ) {
    }
};
struct V2 {
    int __u;
    virtual void foo( int ) {
    }
};
struct V3 {
    int __u;
    virtual void foo( int ) {
    }
};
struct V4 {
    int __u;
    virtual void __u( int ) {
    }
    virtual void __u( int ) {
    }
    virtual void __u( int ) {
    }
    virtual void __u( int ) {
    }
    virtual void __u( int ) {
    }
    virtual void __u( int ) {
    }
};
struct V5 : V1, V2, V3 {
    int __u;
    virtual void foo( int x ) {
	if( this != thunk_OK ) fail(__LINE__);
	if( x >= 1000 ) fail(__LINE__);
    }
    V5() {
	thunk_OK = this;
	if( bar() < 1000 ) {
	    V2 *p = this;
	    p->foo( bar() );
	    V3 *q = this;
	    q->foo( bar() );
	}
    }
};
struct V6 {
    int __u;
    virtual void __u( int ) {
    }
};
struct V7 : V4, V5, V6 {
    int __u;
    virtual void __u( int ) {
    }
};


struct C1 {
    int __u;
    virtual void __u( int ) {
    }
};
struct C2 : virtual V7 {
    int __u;
    virtual void foo( int ) {
    }
};
struct C3 {
    int __u;
    virtual void __u( int ) {
    }
};
struct C4 {
    int __u;
    virtual void __u( int ) {
    }
};
struct C5 : C1, C2, C3 {
    int __u;
    virtual void foo( int ) {
    }
};
struct C6 {
    int __u;
    virtual void __u( int ) {
    }
};
struct C7 : virtual C4, virtual C5, virtual C6, virtual V7 {
    int __u;
    virtual void __u( int ) {
    }
};




struct D1 {
    int __u;
    virtual void __u( int ) {
    }
};
struct D2 : virtual V7 {
    int __u;
    virtual void __u( int ) {
    }
};
struct D3 {
    int __u;
    virtual void __u( int ) {
    }
};
struct D4 {
    int __u;
    virtual void __u( int ) {
    }
};
struct D5 : D1, D2, D3 {
    int __u;
    virtual void __u( int ) {
    }
};
struct D6 {
    int __u;
    virtual void __u( int ) {
    }
};
struct D7 : D4, D5, D6 {
    int __u;
    virtual void __u( int ) {
    }
    D7() {
	thunk_OK = this;
	if( bar() ) {
	    V2 *p = this;
	    p->foo( bar() );
	    V3 *q = this;
	    q->foo( bar() );
	}
    }
};
struct D8 : virtual D7 {
    int __u;
    D8() {
	thunk_OK = this;
	if( bar() ) {
	    V2 *p = this;
	    p->foo( bar() );
	    V3 *q = this;
	    q->foo( bar() );
	}
    }
};

int main() {
    char *p = new char[4096];
    memset( p, -1, 4096 );
    p += ((((unsigned)p)+0xff)&~0xff) - (unsigned)p;
    new (p) D8;
    _PASS;
}
#else

int main()
{
    _PASS;
}

#endif
