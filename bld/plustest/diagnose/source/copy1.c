struct V {
    int v;
    V(){v=__LINE__;}
};
struct A : virtual V {
    int a;
    A(){a=__LINE__;}
};
struct B : A {
    int b;
    B(){b=__LINE__;}
};
struct C : B, A {
    int c;
    C(){c=__LINE__;}
};
struct D : virtual A, C {
    int d;
    D(){d=__LINE__;}
};

void foo( char *p )
{
    *p = 'a';
}

void bar( C *p, C *q )
{
    *p = *q;
    if( p->v != 3 ) {
	foo( 0 );
    }
}

void main( void )
{
    C *c;
    D *d;

    c = new C;
    d = new D;
    bar( c, d );
}
