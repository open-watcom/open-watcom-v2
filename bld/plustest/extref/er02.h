struct B1 {
    int b1;
    virtual void f1( int v ) {
	b1 = v;
    }
    virtual void f3( int v ) {
	b1 = ~v;
    }
    B1() : b1(1) {
    }
};
struct B2 {
    int b2;
    virtual void f2( int v ) {
	b2 = v;
    }
    virtual void f3( int v ) {
	b2 = ~v;
    }
    B2() : b2(2) {
    }
};
struct B2I {
    int b2;
    virtual void f2( int v ) {
	b2 = v;
    }
    virtual void f3( int v ) {
	b2 = ~v;
    }
    B2I() : b2(2) {
    }
};
struct D : B1, B2 {
    int d;
    virtual void f1( int v ) {
	d = v + 1;
    }
    virtual void f2( int v ) {
	d = v + 2;
    }
    virtual void f3( int v ) {
	d = -v;
    }
    D() : d(3) {
    }
};
