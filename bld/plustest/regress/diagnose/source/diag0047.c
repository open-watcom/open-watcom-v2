struct S {
    typedef S T;
    struct Q;
    typedef struct Z R;
    typedef struct { int a; } X;
};

struct T {
    const int a;
    int & b;
};

int i;

struct X {
    void f( void );
};

struct Y {
    static int i;
    friend void X::f( void )
    {
	i = 5;
    }
    friend void q( void )
    {
	i = 2;
    }
};

void bad_auto( void )
{
    unsigned far a;

    a = a;
}
