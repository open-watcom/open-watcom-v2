template <class T>
    struct S;

struct S<char> {
    S<char>() { a = 1; }
    int a;
};

S<char> v9;

// this is a function, really
struct X12 {
    virtual int (*foo( int ))(int) = 0;
};

void foo( X12 *p )
{
    int (*fn)(int);

    fn = p->foo(1);
}

struct X23 {
    int x;
    X23( int n ) : x(n) {}
    struct N;
private:
    struct N {
    };
};

short seg;
struct X33 {
    int a;
    int b;
};

int far X33::* mpf;
int __based(seg) X33::* mps;
int volatile X33::* mpc;
