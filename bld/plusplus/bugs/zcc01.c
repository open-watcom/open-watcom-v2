struct {
    int a;
    void virtual foo( int x ) { a = x; }
} x;

void foo( void )
{
    x.foo( 1 );
}
