struct S {
    int a,b;
    S();
};
struct B {
    int z;
    B(int=0);
};

struct T : B {
    T() : a(1), b(2), a(3), S(1), z(1), f(2), S(3), d(d), B(1), c_OK(2), r_OK(d)
    {
	a = 1;
    }
    void foo( void ) : a(2), b(3) {
	a = 1;
    }
    int a,b;
    const c_bad;
    const c_OK;
    void f(void);
    static int d;
    int &r_bad;
    int &r_OK;
    static int const z1;
    void (&z2)( void );
    void sam( void ) const;
};

S::S() : c(1), d(2)
{
    b = 1;
}

void foo( int a ) : a(2), b(3) {
    a = 1;
}
