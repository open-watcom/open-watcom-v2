struct B {
    ::q;
    D::a;
};
void (far *p)() const;
void (foo())() const;
void (far *bar())() const;
void test_parm( void a() const );
class X;
void (X::* q)() const;

typedef unsigned size_t;
struct A {
    static void foo( void );
    static void foo( int ) const;
    virtual static void foo( double );
    virtual static void foo( char ) const;
};
struct N {
    void * operator new ( size_t );
    virtual void * operator new ( size_t, int );
    void * operator new ( size_t, double ) const;
    virtual void * operator new ( size_t, char ) const;
};
