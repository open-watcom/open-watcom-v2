int foo( int x, int y = foo(1) )
{
    if( x == 1 ) return 1;
    return y;
}

void main( void )
{
    foo(4);
    foo(1);
}
