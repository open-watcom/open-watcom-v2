struct M {
    void a( void );
};

struct N : M {
};

struct V : M, N {
};

struct A : virtual V {
};

struct B : virtual V {
};

struct C : A, B {
};

void foo( C *p )
{
    p->a();		// error; a is ambiguous
}
