class C {
    static C a[];
    static C b[];
    static C c;
    static C d;
    C();
};

C C::a[] = { C(), C() };	// OK
C C::b[2];			// OK
C C::c = C();			// OK
C C::d;				// OK

typedef unsigned size_t;
extern void *operator new( size_t, size_t );
int (**p3)( int ) = new (int (*[10])( int ));
int x;
int (**p5)( int ) = new (int (*[x+1])( int ));
int (**p6)[10] = new (int (*)[x+1]);

void *p8 = new (int(sizeof(int[x+1]))) (int (*)[x+1]);
int *p9 = new (int []);

template <class T>
    void foo( T &, T &, int const & );

void bar( int x )
{
    foo(x,x,x);
}

extern int f31( int & );
extern int f32();
void f33( int b )
{
    if( b )
	int dummy = f32();
    else
	int dummy = f32();
    switch( b )
	int dummy = f32();
    while( b )
	int dummy = f31( b );
    do
	int dummy = f31( b );
    while( b > -10 );
    for( int i = 0; i < 10; ++i )
	int dummy = f31( b );
}
