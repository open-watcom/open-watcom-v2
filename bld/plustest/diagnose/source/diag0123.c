struct C {
    int a[10];
};

char c;
char &rc = c;

double d;
double &rd = d;

C s;
C &rs = s;

void x5()
{
    int x;

    // should all warn about being TRUE
    if( sizeof( char ) == sizeof( char & ) )
	if( sizeof( double ) == sizeof( double & ) )
	    if( sizeof( C ) == sizeof( C & ) )
		++x;
    if( sizeof( c ) == sizeof( rc ) )
	if( sizeof( d ) == sizeof( rd ) )
	    if( sizeof( s ) == sizeof( rs ) )
		++x;
}

struct S1 {
    ~S1();
};

struct D1pri : private S1 {
    int : 0;
    virtual ~D1pri();
    int : 0;
};

struct D1pub : public S1 {
    int : 0;
    virtual ~D1pub();
    int : 0;
};

struct D1pro : protected S1 {
    int : 0;
    virtual ~D1pro();
    int : 0;
};

struct S2 {
    virtual void foo();
    ~S2();
};

struct D2pri : private S2 {
    int : 0;
    virtual ~D2pri();
    int : 0;
};

struct D2pub : public S2 {
    int : 0;
    virtual ~D2pub();
    int : 0;
};

struct D2pro : protected S2 {
    int : 0;
    virtual ~D2pro();
    int : 0;
};

struct X73;

X73 *foo( X73 &r )
{
    return &r;		// warning: assuming class X73 has no overloaded op &
}

struct X73 {
    X73 *operator &();
};

X73 *bar( X73 &r )
{
    return &r;
}

// this is also in the C compiler!
void foo( int x, int y, int z )
{
    do
	while( x < 10 )
	    x+=y,--z;
    while( z < 10 );
    do
    } while( --x );
}
