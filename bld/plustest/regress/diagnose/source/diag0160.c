template <int x>
    struct S {
	ack x() {};
    };

typedef S<void *> T;

T x = T();

template<class T>
class StrangeCollection {
public:
  void addByRef(const T&);
  void addByPtr(T*);
};

StrangeCollection<int&> x17;

int x19(int selector)
{
    switch(selector) {
    default:
	return 1;
	break;		// break never executes but causes warning about
			// return value because it leaves the switch stmt
    }
}

typedef int (* __pascal x29)( int, int, int );
typedef int __pascal (* x30)( int, int, int );
typedef int (* __pascal *** x31)( int, int, int );

extern "C" {
    static void x34(float deltaT);
    static void x34(float deltaT)
    {
    }
}

struct X40 {
    int x,y;
};
struct X43 {
    double w,q;
};

void x47( X40 );

void x49( X43 x )
{
    x47( (X40) x );
}

typedef int X54;

void x56( X54 *p )
{
    p->~X54();
}

typedef struct X62 X61;
struct X62 {
    ~X62() {}
};

void x66( X61 *p )
{
    p->~X61();
}
