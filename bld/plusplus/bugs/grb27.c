extern void foo( int &x );
extern void foo( const int &x );

void foo()
{
    int z = 3;
    const int y = 1;

    foo( y );           // const int &
    foo( z );           //       int &
    foo( 3 );           // const int &
}
