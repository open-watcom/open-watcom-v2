// page 288 of ARM

struct X {
};

struct T {
    T(int);
    T(X);
};

struct S {
    operator int();
    operator X();
};

S a;

T x = T(a);

// page 275 of ARM

class Boolean {
    int b;
public:
    Boolean operator+(Boolean);
    Boolean(int i) { b = i!= 0; };
    operator int() { return b; }
};

Boolean xbool(1);

void foo( Boolean xbool )
{
    xbool + 1;
}


// page 275 of ARM

class B;

class A {
public:
    A(B&);
};

class B {
public:
    operator A();
};

// #pragma on ( dump_rank )

B b;

// text says that the following is ambiguous, but
//
// Effect operation as:
//
//  (1) make an A from b: A::A( b )
//  (2) copy result to a1
//
// A::A(B&) is exact
// A::A(A&) is ambiguous
//
A a1 = b;
A a2 = A(b);
A a3(b);
