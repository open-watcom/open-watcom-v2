class Z;

struct C {
    void foo( Z * );
    void bar( Z * );
};

class Z {
    int a;
    friend void C::foo( Z *p );
    friend void C::not_found( void );
public:
    Z();
};

void C::foo( Z *p )
{
    p->a = 1;
}

void C::bar( Z *p )
{
    p->a = 1;
}
