typedef void VFV(void);

extern "C" VFV x,y,z;
extern "C" VFV a;
extern "C" VFV b;
extern "C" VFV c;

extern "C" int foo(VFV *);

int foo(VFV *v)
{
    v();
    return 1;
}

struct S {
    friend VFV f;
    virtual VFV z;
    virtual VFV y;
    virtual VFV x;
private:
    int p;
};

S * bar( S *p )
{
    p->y();
    return p;
}

void f( void )
{
    S v, *p;

    p = bar( &v );
    p->p = 1;
}

struct B {
protected:
    B( B const & );
};

struct D : B {
    D( B const &r ) : B(r) {
    }
};

void foo( B const &r )
{
    D x( r );
}

#error at least one error
