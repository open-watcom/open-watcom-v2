void x1( unsigned );

void x3( void *p )
{
    x1( 1, p );
}

void x8( unsigned, void * );

void x10()
{
    x8( 1 );
}

void x15( unsigned, signed char );
void x15( unsigned, unsigned char );

void x18( char x)
{
    x15( 1, x );
}

struct X23 {};
struct X24 : private X23 {};

void x26( X23 * );

void x28( X24 *p )
{
    x26( p );
}

struct X33 {
    X33( int x ) : a(x) {}
    void *operator new( unsigned, X23 *ptr ){
	return( ptr );
    }
    int a;
};

void x40( X24 *p ){
    X33 *a;

    a = new (p) X33( 1 );
    delete a;
}

struct X48 {
    int a;
    X48( int data ): a( data ) {};
    void *operator new( unsigned size, signed char ){
	return( ::new char[ size ] );
    }
    void *operator new( unsigned size, unsigned char ){
	return( ::new char[ size ] );
    }
};

void x59( void ){
    X48 *a;

    a = new( 'a' ) X48( 1 );
    delete a;
}

struct X66 {
    void *operator new( unsigned, char, char );
};

void x70() {
    X66 *a;

    a = new ('a') X66;
}

void x70(int i) {
    X66 *a;

    a = new ('a','b','c') X66;
}
