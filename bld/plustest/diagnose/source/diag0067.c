typedef unsigned size_t;
struct S {
    struct T {
	T();
	void * operator new ( size_t );
	void operator delete( void * );
    private:
	~T();
    };
    struct N {
	N();
	void * operator new ( size_t );
    private:
	~N();
    };
    S();
    void * operator new ( size_t );
    void operator delete ( void * );
private:
    ~S();
};

void foo( int x )
{
    S *s;
    S::T *t;
    S::N *n;

    s = new S;
    delete s;
    s = new S[x];
    delete [] s;
    t = new S::T;
    delete t;
    t = new S::T[x];
    delete [] t;
    n = new S::N;
    delete n;
    n = new S::N[x];
    delete [] n;
}

class C1 {
    friend void foo( void );
    void operator delete( void *, size_t );
};

class C2 : C1 {
    friend void foo( void );
    C2();
    ~C2();
    int a[10];
};

C2::~C2()
{
}

C2 *p;

void foo( void )
{
    delete p;
}
