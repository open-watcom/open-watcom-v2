class C {
    int x,y;
private:
    void foo( int );
public:
    void foo( int, int );
private:
    void foo( int, double );
public:
    void foo( char );
private:
    void foo( double );
};

void t( C *p )
{
    ((((p->foo))))( 1 );
    ((((p->foo))))( 1, 2 );
    ((((p->foo))))( 1, 2.0 );
    ((((p->foo))))( 'a' );
    ((((p->foo))))( 1.0 );
}
