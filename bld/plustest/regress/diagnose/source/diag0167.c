enum X { x1, x2, x3 };
enum Y { y1, y2, y3 };

void x4()
{
    Y d;

    d = Y(x1);	// should be OK
}

static void x11() {}
static void x12() {}

void main(int x)
{
    (x ? x11 : x12)();
}

struct S {
};
struct T {
};

T const & foo( S &c )
{
    return c;
}

T const & foo( S &c, S &d )
{
    return ( T&) d;
}

 void x34()
 {
    void* cp;

    cp += 1;	// should not be allowed
 }

template <class T>
    void swap( T&l, T&r )
    {
	T t; t=l;l=r;r=t;
	T __far c;
    }

int __near i, __near j;

void x52()
{
    swap( i, j );
}
