/* these are valid declarations */
extern "C" extern int f1;
extern "C" static int f2;
extern "C" int extern f3;
extern "C" int static f4;
extern "C" typedef int (*CF)(int);
extern "C" int a;
extern "C" { int b; };
extern "C" enum X { X1, X2, X3 };
extern "C" class Z {};
void foo( void )
{
    f1 = 1;
    f2 = 2;
    f3 = 3;
    f4 = 4;
    a = 1;
    b = 2;
}
struct A {
    int a;
    int foo( int = a );	/* default arg cannot access 'this' */
};
struct S {
    int a;
    static int foo();
};

int S::foo()
{
    return a;		/* static member functions cannot access 'this' */
}
