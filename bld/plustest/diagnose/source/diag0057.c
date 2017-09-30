typedef unsigned size_t;

int foo( int, int );
char foo( int, int );

union U {
    virtual void foo( void );
};

struct A {
    virtual static void sam( void );
    virtual A();
    virtual void *operator new( size_t );
};

struct B1 {
    virtual int foo( void );
    virtual B1 *foo( int, int );
};

struct D1 : B1 {
    float foo( void );
    virtual D1 *foo( int, int );
};

struct B2 {
    virtual void foo( int );
    virtual void foo( char );
    virtual void bar( int );
    virtual void bar( char );
};

struct D2 : B2 {
    virtual void foo( double );
    void bar( double );
};

struct B3 {
    virtual void foo( int, ... );
};

struct D3 : B3 {
    void foo( int, ... );
};

struct C {
    C();
    ~C();
    C(int,int);
};

void foo( char *d )
{
    C* c;

    c = new C[*d];
    c = new C[d];
    c = new C[10](1,2);
    d = new char('a');
    d = new char[10]('a','b');
    d = new char[10]('a');
}

struct Q {
    virtual Q *foo( void );
};

struct R : Q {
    virtual R &foo( void );
};
