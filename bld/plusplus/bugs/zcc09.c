void foo( void )
{
    {
        class C {
            int a;
            friend void foo( void );
            int foo( int i )
            {
                if( i == 0 ) {
                    return( 1 );
                }
                return( foo( i - 1 ) * i );
            }
        };
        C x;

        x.foo( 3 );
    }
    {
        class C {
            int a;
            friend void foo( void );
            int foo( int i )
            {
                if( i == 0 ) {
                    return( 1 );
                }
                return( foo( i - 1 ) * ( i + 1 ) );
            }
        };
        C x;

        x.foo( 3 );
    }
}
