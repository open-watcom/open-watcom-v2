/* should this be diagnosed? NO. this example works */
struct S {
    static int foo( int x, int z = 3, int y = foo(1) )
    {
        if( x == 1 ) return 1;
        return y;
    }
};

void main( void )
{
    S::foo(4);
    S::foo(1);
}
