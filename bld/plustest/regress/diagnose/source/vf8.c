struct B0 {				//	B0 B1 B2
    int : 1;				//       \ |  /
};					//        \|/
struct B1 {				//	   B3
    int : 1;				//	   ||
};					//	   VV
struct B2 {				//	   ||
    int : 1;				//	B4 || B1	-- B5 -> B1
};					//       \ || /		   is ambiguous
struct B3 : B0, B1, B2 {		//	  \||/		   but each
    int : 1;				//	   B5		   jump is not
};					//			i.e., B5->B3->B1
struct B4 {
    int : 1;
};
struct B5 : B4, virtual B3, B1 {
    int : 1;
};

struct P {
    int p;
};
struct A {
    virtual B1 &foo(void);
    A();
    virtual ~A();
};
struct B : P, virtual A {
    virtual B3 &foo(void);
    B();
    virtual ~B();
};
struct C : P, virtual B {
    virtual B5 &foo(void);
    C();
    virtual ~C();
};
B5 b5;

A::A()
{
}
A::~A()
{
}
B1 & A::foo( void )
{
    B3 & r = b5;
    return r;
}
B::B()
{
}
B::~B()
{
}
B3 & B::foo( void )
{
    return b5;
}
C::C()
{
}
C::~C()
{
}
B5 & C::foo( void )
{
    return b5;
}

void main( void )
{
    A *a;
    B *b;
    C *c;

    a = new A;
    a->foo();
    delete a;
    b = new B;
    b->foo();
    a = b;
    a->foo();
    delete a;
    c = new C;
    c->foo();
    b = c;
    b->foo();
    a = b;
    a->foo();
    delete a;
}
