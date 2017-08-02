#ackbar

struct X3 {};
struct X4 : X3 {};
template <class T>
    struct T5 {
	operator T();
	T5( X3 const & );
	T5();
    };

T5<X4> x;

X4 must_parse( const X3& r ) {
	return( T5<X4>( r ) );
}

// wpp386 -za

extern int loop();
extern int test();
extern int val();
extern int d(int, int);

void loopy () {
    while( loop() ) {
	if( test() ) continue;
	
	int v = val();
	int b_modified = v & 0x1;
	
	d(v, b_modified);
    }
}

template <class K,class M>
    struct Map;

Map<int,long> *z;

template <class Key,class Mapping>
    struct Map {
	Key *p;
	void foo();
	Mapping *q;
    };

template <class Key,class Mapping>
    void Map<Key,Mapping>::foo()
    {
    }

double dd;
void *pp;

void foofoo()
{
    dd = (double) pp;
    pp = (void *) dd;
}
