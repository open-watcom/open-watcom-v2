enum E1 { EA1, EA2, EA3 };
enum E1;

struct X4 {
    void foo( int = 0, int = 1, int = 2, int = 3, int = 4 );
    void (*bar( int(*x)(int,int,int,int) = 0, int = 1, int = 2, int = 3, int = 4, ... ))(int,int,int,int,int);
};

void x8( X4 *p )
{
    p->foo( 1, 2, p );
    p->bar( 0, 2, 3, p );
}
