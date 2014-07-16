#include "fail.h"

#if __WATCOM_REVISION__ >= 8
template <class S, class C, class U, class E>
    struct Template {
	struct S *p;
	class C *q;
	union U *u;
	enum E *e;
	class S *__u;
	struct C *__u;
	Template( S *p, C *q, U *u, E *e )
	    : p(p)
	    , q(q)
	    , u(u)
	    , e(e)
	{
	}
	int foo() {
	    return (p->m+q->m+u->m+*e);
	}
    };

struct Struct {
    int __u;
    int m;
};

class Class {
public:
    int __u;
    int m;
};

union Union {
    int f;
    int m;
};

enum Enum {
    A, B, C
};

Struct s = { 0, 1<<1 };
Class c = { 0, 1<<2 };
Union u = { 1 << 3 };
Enum e = B;

Template<Struct,Class,Union,Enum> v(&s,&c,&u,&e);

int main() {
    if( v.foo() != 15 ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
