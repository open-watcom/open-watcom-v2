struct CTOR {
    CTOR();
};
struct uCTOR {
    static CTOR m;
};
struct DTOR {
    ~DTOR();
};
struct uDTOR {
    static DTOR m;
};
struct ASSIGN {
    void operator = ( ASSIGN const & );
};
struct uASSIGN {
    static ASSIGN m;
};
struct CONST {
    const int x;
    CONST( int v ) : x(v) {}
};
struct uCONST {
    static CONST m;
};
struct REF {
    int &r;
    REF( int &v ) : r(v) {}
};
struct uREF {
    static REF m;
};
struct NONCONST_COPY {
    NONCONST_COPY();
    NONCONST_COPY( NONCONST_COPY & );
};
struct uNONCONST_COPY {
    static NONCONST_COPY m;
};
struct NONCONST_ASSIGN {
    NONCONST_ASSIGN();
    void operator =( NONCONST_ASSIGN & );
};
struct uNONCONST_ASSIGN {
    static NONCONST_ASSIGN m;
};

union U1 {
    uCTOR x;
};
union U2 {
    uDTOR x;
};
union U3 {
    uASSIGN x;
};
union U4 {
    uCONST x;
};
union U5 {
    uREF x;
};
union U6 {
    uNONCONST_COPY x;
};
union U7 {
    uNONCONST_ASSIGN x;
};

struct S1 {
    uNONCONST_COPY x[10];
    uNONCONST_ASSIGN y;
};

uREF x75;
uCONST x76;

void foo( S1 &d, S1 const &s )
{
    S1 y(s);		// no ctor!
    S1 x = s;
    d = s;
}

// error messages should be sequential in line numbers!
struct T86 {
    struct S {
	struct N {
	    ~N() { int x; }
	};
	N x;
	~S() { int x; }
    };
    struct Q {
	S x;
	~Q() { int x; }
    };
    S x;
    ~T86() { int x; }
};
