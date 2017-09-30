void sam( void )
{
    static int a;
    int s;
    struct C {
	void foo( void )
	{
	    s = 1;			// accessing an auto
	}
	void bar( int x, int y = a )	// ditto
	{
	    a = x + y;
	}
    };
    for( s = 0; s < 10; ++s ) {
	a += s;
    }
}

struct Q {
    void foo( void );
};

void bar( char );

void foo( void )
{
    struct A;
    {
	struct B {
	    friend void foo( void );	// OK
	    friend void bar( char );	// error
	};
	struct C {
	    friend struct D;		// OK
	    friend struct A;		// error
	    friend struct B;		// error
	    friend struct Q;		// error
	    void memC( int x )
	    {
		a = x;
	    }
	private:
	    int a;
	};
	struct D {
	    friend void C::memC( int );	// error
	    int d;
	    void memD( C *p )
	    {
		p->a = 1;
	    }
	};
    }
    struct A {
	void memA( void );		// not inline
    };
}
