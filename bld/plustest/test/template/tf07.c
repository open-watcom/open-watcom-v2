extern void f( int, int );		// B:y M:n W:y

int ss;

template <class T>
    void f( T x, T y )
    {
	for( int i = 0; i < 10; ++i ) {
	    x+=y;
	}
	ss = x;
    }
extern void f( unsigned, unsigned );	// B: y M: n W: y

struct S {
    friend void f( char, char );	// B: y M: n W: y
};

void main()
{
    extern void f( double, double );	// B: n M: n W: y
    f(3.1,4.0);
}
