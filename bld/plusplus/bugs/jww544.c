// poor error diagnostic
struct O {
};

struct B1 : O {
};
struct B2 : O {
};

struct D : B1, B2 {
    void foo();
};

void bar( O * );

void D::foo()
{
    bar( this );
}
