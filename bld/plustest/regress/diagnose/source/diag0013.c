/*
 * test for indistinct overloaded function declaration
 * test for ambiguous overloaded function
 * test for no matching overloaded function
 * test for distinct with order dependancy
 */

void fc( char );
void fc( char );	// identical redeclaration
typedef char CHAR;
void fc( CHAR );	// identical redeclartion

void fi( int );
void fi( int & ); 	// ambiguous declaration

void ff( float );
void ff( double );

void g( int a, char *b )
{
    ff( a );		// ambiguous resolution
    ff( b );		// no resolution
}

typedef int INT;

int foo( INT );
int foo( int );

int foo( int x )
{
    return( x + 1 );
}

int foo( INT y )	// redefinition of the same function
{
    return( y + 2 );
}

class A {
    public:
    	operator int ();
	operator int ();	// redeclaration, ok
	operator const int ();	// indistinct declaration, no good
	operator float ();
};

int bar( int, char );
float bar( char *, float );
int bar( int, char );		// redeclaration, ok
int bar( int &, char );		// not distinct, no good
int bar( char *, float );	// redeclaration in args but different return

void f1a( int const & );
void f1a( int );
void f1b( int );
void f1b( int const & );

void f2a( int volatile & );
void f2a( int );
void f2b( int );
void f2b( int volatile & );

void f3a( int const volatile & );
void f3a( int );
void f3b( int );
void f3b( int const volatile & );

// what is correct behaviour in this example?
// currently we report ambiguity, but perhaps that should not be so.
struct S {
    S(int);
};

void foo( S const & );
void foo( S );

void bar() {
    foo( S(1) );    // calls foo( S )
    foo(1);	    // claims ambiguous
}
