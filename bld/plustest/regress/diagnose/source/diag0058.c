struct T {
    int field;
    void member_fn( void );
    virtual void virtual_member_fn( void );
    static void static_member_fn( void );
    static int static_field;
    T();
    virtual ~T();
};

// these are OK
int T::* ok1 = &T::field;
void (T::* ok2 )( void ) = &T::member_fn;
void (T::* ok3 )( void ) = &T::virtual_member_fn;
int *ok4 = &T::static_field;
void (*ok5)( void ) = &T::static_member_fn;

// these are not OK
int T::* bad1 = &T::static_field;
void (T::* bad2 )( void ) = &T::static_member_fn;

void foo( void )
{
    T *p;

    p = new T[100];
    ( delete p ) + 1;			// BAD
    p = new T[100];
    ( p != 0 ) ? delete p : foo();	// OK
    T const *q = new T;
    delete q;
    q = new T[100];
    delete q;
}

class X;

void bar( void )
{
    X *p;

    p = new X;
    delete p;
    p = new X[10][20];
    delete [] p;
}

struct V {
    int v;
};

struct S1 : virtual V {
    int s1;
};

struct S2 {
    virtual void foo();
};

extern void *memset( void *, char, unsigned );

void special( S1 *p1, S2 *p2 )
{
    memset( p1, 0, sizeof( *p1 ) );
    memset( p2, 0, sizeof( *p2 ) );
}

struct B {
    virtual void foo();
    ~B();
};

struct D : B {
    virtual void foo();
    virtual ~D();
};

void bar( B *p, D *q )
{
    delete p;
    delete [] q;
}

#error blank delimited error message
#error *!
#error
#error \
