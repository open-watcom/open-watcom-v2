struct T;
struct S {
    T operator ->();
    int a;
};

struct U;
struct T {
    U operator ->();
    int a;
};

struct U {
    S operator ->();
    int a;
};

struct V {
    T &operator ->();
};

struct W {
    T *operator ->();
};

operator ->( int )
{
}

struct A {
    struct B {
	A *operator ->( int );
    };
    B operator ->( int );
    B &operator ->( double );
    operator ->();
};

struct Q {
    struct A {
	A *operator ->();
    };
    struct B {
	A operator ->();
    };
    struct C {
	B &operator ->();
    };
};

void foo( S *p )
{
    p->a = 1;
}
