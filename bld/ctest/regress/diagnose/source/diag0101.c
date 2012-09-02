extern char f1( unsigned char p1, int p2, unsigned int p3, long p4, unsigned long p5 );
extern char f2( int, ... );
extern char f3( void );

char (*pf1)( unsigned char p1, int p2, unsigned int p3, long p4, unsigned long p5 ) = f1;
char (*pf2)( int, ... ) = f2;
char (*pf3)( void ) = f3;

int f1( int p1, unsigned int p2, long p3, unsigned long p4, long long p5 )
{
    f2( p1, p2, p3, p4, p5 );
    f3( p1, p2 );
    f2( p1 );
    f3( p1 );
    f2();
    f3();
    pf2( p1, p2, p3, p4, p5 );
    pf3( p1, p2 );
    pf2( p1 );
    pf3( p1 );
    pf2();
    pf3();
    return( 0 );
}
