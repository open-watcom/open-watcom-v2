struct X1 {
    struct A {
	int ai;
    };
    int f1( void )
    {
	A a;
	int A::*apm;
	apm = &A::ai;
	return a.*apm;
    }
    int f2( void )
    {
	static A a;
	int A::*apm;
	apm = &A::ai;
	return a.*apm;
    }
};
struct X20 {
    struct A {
	int ai;
    };
    int f1( void )
    {
	A a;
	A *pa;
	int A::*apm;
	pa = &a;
	apm = &A::ai;
	return pa->*apm;
    }
    int f2( void )
    {
	static A a;
	A *pa;
	int A::*apm;
	pa = &a;
	apm = &A::ai;
	return pa->*apm;
    }
};
struct X43 {
    struct A {
	int ai;
    };
    void f( void )
    {
	A a;
	int A::*apm;
	apm = &A::ai;
	(a.*apm) = 1;
    }
};
struct X55 {
    struct A {
	int ai;
    };
    void f( void )
    {
	A a;
	A *pa;
	int A::*apm;
	pa = &a;
	apm = &A::ai;
	(pa->*apm) = 1;
    }
};
struct B69 {
    int bi;
};
struct D72 : public B69 {
    int di;
};

void x76( void )
{
    B69 b;
    B69 *bp = &b;
    int B69::*bpm = &B69::bi;
    
    D72 d;
    D72 *dp = &d;
    int D72::*dpm = &D72::di;
    (b.*dpm) = 1;		// can't go from B69 to D72
    (bp->*dpm) = 2;		// can't go from B69* to D72*
}

void x89( void )
{
    class B {
    public:
	int bi;
    };
    class D : private B {
    public:
	int di;
    };
    
    B b;
    int B::* bpm = &B::bi;
    D d;
    int D::* dpm = &D::di;
    (b.*bpm) = 1;	// ok
    (d.*dpm) = 2;	// ok
    (d.*bpm) = 3;	// no good, not accessible
}

void x109( void )
{
    class A {
    public:
	int ai;
    };
    class B : public A {
    };
    class D : public A, public B {
    public:
	int di;
    };
    
    A a;
    int A::* apm = &A::ai;
    D d;
    int D::* dpm = &D::di;
    (a.*apm) = 1;	// ok
    (d.*dpm) = 2;	// ok
    (d.*apm) = 3;	// no good, ambiguous
}
