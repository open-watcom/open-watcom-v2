struct V {
    operator int();
    int v;
};

struct A : virtual V {
    operator int();
    int a;
};

struct B : V {
    operator double();
    int b;
};

struct E {
    operator int();
};

struct C : virtual V, public E {
    operator char *();
    int c;
};

struct D : A, B, C {
    operator int();
    int foo( void );
};

D x;

// D	- A	* V
//	- B	- V
//	- C	- E
//		* V

// V, A, E, D all have operator int() but D's overrides them all

int D::foo( void )
{
    return operator int();
}

struct P {
    operator char *();
};

int y;

void foo( P p )
{
    if( p ) {		// should convert to "char *"
	++y;
    }
}

struct I {
    operator int();
};

void bar( I p )
{
    if( p ) {		// should convert to "int"
	++y;
    }
}
