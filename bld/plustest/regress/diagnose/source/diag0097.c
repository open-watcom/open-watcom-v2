int jk;

int foo( unsigned u, unsigned long us, char *p )
{
    if( u >= 0 ) {
        jk = 98;
    }
    if( us >= 0 ) {
        jk = 99;
    }
    if( p >= 0 ) {
        jk = 100;
    }
    jk = ( p >=0 ) ? 43 : 44;
    return ( u >= 0 )
        || ( us >= 0 )
        || ( p >= 0 )
        || ( 0 <= u )
        || ( 0 <= us )
        || ( 0 <= p )
        || ( u < 0 )
        || ( us < 0 )
        || ( p < 0 )
        || ( 0 > u )
        || ( 0 > us )
        || ( 0 > p );
}

template <class T,unsigned long size,int *p>
    class S {
	T a[size+(sizeof( *p )/sizeof( void *))+1];
    };

int aa;
S<int,-1,&aa> x;
S<int,-1,&aa> x;

void foo()
{
    x.a[0] = 1;
}
