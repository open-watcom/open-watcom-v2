struct A {
    int b;
};
struct C {
    int A::b;
};

int a;

struct S {
    static int a;
    struct T {
	T();
	~T();
    };
    int q;
    void foo( void );
    void foo( int );
};

void S::foo( void )
{
    a = 1;
}

void S::foo( double x )
{
    a = x;
}

void foo( S::T *p )
{
    p->~T();
}
