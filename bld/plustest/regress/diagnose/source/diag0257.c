void foo(int i1, int i2 = 2);
void foo(int i1, int i2, int i3 = 6);
void (*f)(int, int) = &foo; // OK 

void goo(int i1 );
void goo(int i1, int i2, int i3 = 6);
void (*f2)(int, int) = &goo; // error no match 

// extra test that should give error message without fatal error
class H;
void (H::*pmfh)(int,int,int) = &::operator();

// order shouldn't matter!
// 3 2 1
struct B {
    void operator() (int i1, int i2); // (3)
    // add (int,int)
    void operator() (int i1, int i2, int i3 = 6); // (2)
    // add (int,int,int) and (int, int, [int] )
    void operator() (int i1, int i2 = 3); // (1) 
    // don't re-add (int,int) ( redeclaration of (1) )
    // add (int, [int] )
};
void (B::*pmfb)(int, int) = &B::operator();

// 3 1 2
struct C {
    void operator() (int i1, int i2); // (3)
    void operator() (int i1, int i2 = 3); // (1) 
    void operator() (int i1, int i2, int i3 = 6); // (2)
};
void (C::*pmfc)(int, int) = &C::operator();

// 1 2 3
struct D {
    void operator() (int i1, int i2 = 3); // (1) 
    void operator() (int i1, int i2, int i3 = 6); // (2)
    void operator() (int i1, int i2); // (3)
};
void (D::*pmfd)(int, int) = &D::operator();

// 1 3 2
struct E {
    void operator() (int i1, int i2 = 3); // (1) 
    void operator() (int i1, int i2); // (3)
    void operator() (int i1, int i2, int i3 = 6); // (2)
};
void (E::*pmfe)(int, int) = &E::operator();

// 2 1 3
struct F {
    void operator() (int i1, int i2, int i3 = 6); // (2)
    void operator() (int i1, int i2 = 3); // (1) 
    void operator() (int i1, int i2); // (3)
};
void (F::*pmff)(int, int) = &F::operator();

// 2 3 1
struct G {
    void operator() (int i1, int i2, int i3 = 6); // (2)
    void operator() (int i1, int i2); // (3)
    void operator() (int i1, int i2 = 3); // (1) 
};
void (G::*pmfg)(int, int) = &G::operator();

void main()
{
    B b;
    C c;
    D d;
    E e;
    F f;
    G g;

    foo(2,2);	// ambig
    goo(2,2);	// OK 

    b.operator()(4,3);
    c.operator()(4,3);
    d.operator()(4,3);
    e.operator()(4,3);
    f.operator()(4,3);
    g.operator()(4,3);
}
