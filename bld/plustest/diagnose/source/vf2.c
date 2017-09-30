struct A {
    int a;					// offset 0
    						// offset 2 (vfptr)
    virtual void foo( void );			// 1
    virtual void is_virtual_a( void );		// 2
    void not_virtual_a( void );
    A();
};

struct B {
    						// offset 0 (vfptr)
    virtual void bar( void );			// 1
    virtual void foo( void );			// 2
    virtual void is_virtual_b( void );		// 3
    void not_virtual_b( void );
    int b;					// offset 2
    B();
};

struct Q {
    int q1;					// offset 0
    int q2;					// offset 2
};

struct P : Q, virtual B {
    						// offset 4 (vbptr)
    int p1;					// offset 6
    int p2;					// offset 8
};

struct R {
    int r1;
    int r2;
};

struct S : virtual A, Q {
    int s1;
    int s2;
};

struct C : R, P, virtual A, virtual B, S {
    						// offset a (vbptr)
    int c;					// offset c
    virtual void is_virtual_c( void );		// 1
    void not_virtual_c( void );
    void virtual sam( void );			// 2
    void virtual bar( void );			// 3
    void virtual foo( void );			// 4
    C();
};

A::A(){}
B::B(){}
C::C(){}

void foo( A *a, B *b, C *c )
{
    a->foo();
    b->bar();
    b->foo();
    c->foo();
    c->bar();
    c->sam();
    c->is_virtual_a();
    c->not_virtual_a();
    c->is_virtual_b();
    c->not_virtual_b();
    c->is_virtual_c();
    c->not_virtual_c();
}
