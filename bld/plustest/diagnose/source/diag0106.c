// unions cannot have static *data* members! *fn* members are OK
union U {
    static int x;
    static void foo();
};
static union {
    static int x;
    static void foo();
};

void f11()
{
    int a;
    static int b;
    struct S {
	int m;
	static int n;
	friend void f18()
	{
	    a = 1;
	    b = 2;
	    m = 3;
	    n = 4;
	}
    };
}

struct X28 {
    virtual ~X28() {}
    virtual ~X28() = 0;
};
struct X32 {
    virtual ~X32() = 0;
    virtual ~X32() {}
};

X28 x37;
X32 x38;

struct X40 {
    void foo();
};

virtual void X40::foo()
{
}

union U48 {
    int &r;
    int *p;
};

int z48;
U48 x48 = z48;

union U56 {
    int &r;
    int *p;
    U56(int &x) : r(x) {}
};

int z56;
U56 x56 = z56;
