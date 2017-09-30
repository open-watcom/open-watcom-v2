struct A {
    void ambig( void );
};
struct B {
    void ambig( void );
};
class C : A, B {
    class Z;
    void OK2( Z * );
    void notOK2( Z * );
    class Z {
	int a;
	extern friend void barf( Z * );
	friend void ambig( void );
	friend void OK1( Z * );
	friend void OK2( Z * );
	friend void double_OK( Z * );
	friend void double_OK( Z * );
    public:
	Z();
    };
public:
    C();
};

void OK1( C::Z *p )
{
    p->a = 1;
}

void C::OK2( C::Z *p )
{
    p->a = 1;
}

void notOK1( C::Z *p )
{
    p->a = 1;
}

void C::notOK2( C::Z *p )
{
    p->a = 1;
}
