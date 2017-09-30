int x16( int &x )
{
    return !&x;
}
int x20( int *x )
{
    int &r = *x;
    return &r != 0;
}
struct X25 {
    int &r;
    int foo();
    int test_this();
    int use_this();
    X25();
};
int X25::foo()
{
    return &r != 0;
}
int foo( X25 *p )
{
    return &(p->r) == 0;
}
int X25::test_this()
{
    if( this ) {
	return 2;
    }
    return this != 0 ? 6 : 7;
}
int X25::use_this()
{
    if( !this ) {
	return 3;
    }
    return this ? 4 : 5;
}

struct BAD1 {
    int a;
    union {
	int b;
	int c;
    } u;
};
BAD1 x8 = {
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
};
struct BAD2 {
    int a;
    union {
	int b;
	int c;
    };
};
BAD2 x19 = {
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
};
struct BAD3 {
    int a;
    union {
	char b;
	int c;
	double d;
    };
};
BAD3 x42 = {
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
};
struct BAD4 {
    int a;
    union {
	char b;
	int c;
	double d;
    };
    union {
	int e;
	double f;
    };
};
BAD4 x66 = {
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
};
