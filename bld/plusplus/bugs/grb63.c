// low priority (we should wait to see if the standard is clear about this)
// 'const' and 'volatile' are not significant in overloading at the
// first level
struct S {
    int a;
    operator int () const { return a; }
};
struct T {
    int a;
    operator int () const { return a + 1; }
};
struct S2 {
    int a;
    operator int () const;
};
struct T2 {
    int a;
    operator int () const;
};

// these are not different enough
int sam( int );
int sam( const int );
int sam( volatile int );

int foo(int i, char c) { return i + c;}
int foo(int const i, int n) { return i + 2 * n; }
int foo(int volatile i, long L) { return i + 3 * int(L);  }
int foo(int &r, float f) { return r + 4 * int(f); }

void bar( T const &x, S const &y )
{
    foo( x, 2 );
    foo( x, 2L );
    foo( y, 2 );
    foo( y, 2L );
}

void bar( T2 const &x, S2 const &y )
{
    foo( x, 2 );
    foo( x, 2L );
    foo( y, 2 );
    foo( y, 2L );
}
