// 7.1.2 "inline" means *default* internal linkage (this is OK)

extern int inline foo( int x, int y )
{
    return x + y;
}

extern "C" int printf( const char *, ... );

void main()
{
    printf( "%d\n", foo( 1, 2 ) );
}
