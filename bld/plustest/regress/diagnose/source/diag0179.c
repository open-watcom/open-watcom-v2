struct A {
};
struct B : A {
};
struct C {
};

A *pa;
B *pb;
C *pc;

A a;
B b;
C c;

A &ra = a;
B &rb = b;
C &rc = c;

// Pointers

A *pab( void )
{
    return pb;
}

A *pac( void )
{
    return pc;
}

B *pba( void )
{
    return pa;
}

B *pbc( void )
{
    return pc;
}

C *pca( void )
{
    return pa;
}

C *pcb( void )
{
    return pb;
}

void foo( void )
{
    pa = pb;
    pa = pc;
    pb = pa;
    pb = pc;
    pc = pa;
    pc = pb;
}

// References

A &rab( void )
{
    return rb;
}

A &rac( void )
{
    return rc;
}

B &rba( void )
{
    return ra;
}

B &rbc( void )
{
    return rc;
}

C &rca( void )
{
    return ra;
}

C &rcb( void )
{
    return rb;
}
