class B;

class A {
    int i;
protected:
    void prot() { i=1; };
public:
    A() { i=0; }
    void f(B*);
};

class B: public A {
public:
    B() {}
};

void A::f(B *b) { b->prot(); }	// should be OK

struct S {
    int bf : 1;
    S() : bf(0) {}	// should be OK
};

typedef int FN( int );

void foo()
{
    void();
    FN();
}

struct X32 {
    operator char const * volatile * const volatile *();
    operator char * X32::*();
};
X32::operator char const * volatile * const volatile *()
{
    return 0;
}
X32::operator char * X32::*()
{
    return 0;
}

unsigned x451 = sizeof( "1234" "5678" );
unsigned x452 = sizeof( L"1234" L"5678" );
unsigned x453 = sizeof( "1234" L"5678" );
unsigned x454 = sizeof( L"1234" "5678" );

struct X50;

X50 * foo( X50 &y )
{
    return &y;		// assuming no operator '&' defined if 'S' is undefined
}
