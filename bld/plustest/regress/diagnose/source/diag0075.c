struct C {
    C(int);
    C();
};

struct N {
};

C *p0 = new C;
C *p1 = new C();
C *p2 = new C(1);
C *p3 = new C("asdf");
N *q0 = new N;
N *q1 = new N();
N *q2 = new N(1);
N *q3 = new N("asdf");

int __based((__segment)__self) *y;
int __based((__segment)__self) x;
int __based((__segment)__self) z[1];

void foo( void )
{
    int __based(__segname("FOO")) x;
}
struct S {
    int __based(__segname("FOO")) x;
};

typedef I1;
typedef int I1;
typedef I1 I1;
typedef int I1;

typedef int I2;
typedef int &I2;

typedef int I3;
typedef short I3;

typedef double I4;
typedef long double I4;

typedef int I5;
typedef double I5;

typedef char *P1;
typedef char far *P1;

typedef char *P2;
typedef char far *P2;

void foo( int x )
{
    switch( x ) {
	case 0 :
	    for( int i=0; i<10; ++i )
	    {
	    }
	case 1:
	{
	    for( int j=0; j<10; ++j )
	    {
	    }
	default:
	    for( int k=0; k<10; ++k )
	    {
	    }
	}
    };
}

struct S {
    int t;
    S(int x) : t(x)
};
