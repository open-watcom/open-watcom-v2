struct B {
    int x;
    void foo( void );
};
struct D : protected B {
    int y;
    void foo( void );
};
struct E : protected D {
    int z;
    void foo( void );
};

B *pB;
D *pD;
E *pE;

void B::foo( void )
{
    pB->x = 1;
    pD->x = 1;		// BAD (B is a protected base of D)
    pD->y = 1;
    pE->x = 1;		// BAD (B,D are protected bases of D,E respectively)
    pE->y = 1;		// BAD (D is a protected base of E)
    pE->z = 1;
}

void D::foo( void )
{
    pB->x = 1;
    pD->x = 1;
    pD->y = 1;
    pE->x = 1;		// BAD (D is a protected base of E)
    pE->y = 1;		// BAD (D is a protected base of E)
    pE->z = 1;
}

void E::foo( void )
{
    pB->x = 1;
    pD->x = 1;		// BAD (B is a protected base of D)
    pD->y = 1;
    pE->x = 1;
    pE->y = 1;
    pE->z = 1;
}
