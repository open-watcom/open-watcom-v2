#include "fail.h"

#define MAX_ALLOCS	200

#define MIN_ALLOC	64

unsigned index;
char buff[MAX_ALLOCS*MIN_ALLOC];
unsigned alloc;
unsigned asize[MAX_ALLOCS];

void *stack_alloc( unsigned s )
{
    unsigned sindex = index;
    index += MIN_ALLOC;
    asize[alloc++] = s;
    return &buff[sindex];
}

void stack_free( void *p )
{
    --alloc;
    index -= MIN_ALLOC;
    if( p != &buff[index] ) {
	fail(__LINE__);
    }
}

void stack_free_size( void *p, unsigned s )
{
    stack_free( p );
    if( s != asize[alloc] ) {
	fail(__LINE__);
    }
}

void *operator new( unsigned s )
{ return stack_alloc(s); }
void *operator new[]( unsigned s )
{ return stack_alloc(s); }
void *operator new( unsigned s, char )
{ return stack_alloc(s); }
void *operator new[]( unsigned s, char )
{ return stack_alloc(s); }
void operator delete( void *p )
{ stack_free( p ); }
void operator delete[]( void *p )
{ stack_free( p ); }

struct X {
};

struct S {
    void * operator new( unsigned s)
    { return stack_alloc(s); }
    void * operator new[]( unsigned s)
    { return stack_alloc(s); }
    void operator delete( void *p, unsigned s )
    { stack_free_size( p, s ); }
    void operator delete[]( void *p, unsigned s )
    { stack_free_size( p, s ); }
    struct N1 {
	void *operator new( unsigned s, X const & )
	{ return stack_alloc(s); }
	void *operator new[]( unsigned s, X const & )
	{ return stack_alloc(s); }
	void operator delete( void *p )
	{ stack_free( p ); }
	void operator delete[]( void *p )
	{ stack_free( p ); }
    };
    struct N2 : N1 {
    };
    struct N3 {
    };
};

X *p1;
S *p2;
S::N1 *p3;
S::N2 *p4;
S::N3 *p5;
X *pg1;
S *pg2;
S::N1 *pg3;
S::N2 *pg4;
S::N3 *pg5;

X vX;

void foo( int n )
{
    {
	p1 = new X;
	p2 = new S;
	p3 = new ( vX ) S::N1;
	p4 = new ( vX ) S::N2;
	p5 = new ( 'a' ) S::N3;
	pg1 = ::new X;
	pg2 = ::new S;
	pg3 = ::new ( 'b' ) S::N1;
	pg4 = ::new ( 'c' ) S::N2;
	pg5 = ::new ( 'd' ) S::N3;
	::delete pg5;
	::delete pg4;
	::delete pg3;
	::delete pg2;
	::delete pg1;
	delete p5;
	delete p4;
	delete p3;
	delete p2;
	delete p1;
    }
    {
	p1 = new X[n];
	p2 = new S[n];
	p3 = new ( vX ) S::N1[n];
	p4 = new ( vX ) S::N2[n];
	p5 = new ( 'a' ) S::N3[n];
	pg1 = ::new X[n];
	pg2 = ::new S[n];
	pg3 = ::new ( 'b' ) S::N1[n];
	pg4 = ::new ( 'c' ) S::N2[n];
	pg5 = ::new ( 'd' ) S::N3[n];
	::delete [] pg5;
	::delete [] pg4;
	::delete [] pg3;
	::delete [] pg2;
	::delete [] pg1;
	delete [] p5;
	delete [] p4;
	delete [] p3;
	delete [] p2;
	delete [] p1;
    }
    {
	p1 = new X;
	p2 = new S;
	p3 = new ( vX ) S::N1;
	p4 = new ( vX ) S::N2;
	p5 = new ( 'a' ) S::N3;
	pg1 = ::new X;
	pg2 = ::new S;
	pg3 = ::new ( 'b' ) S::N1;
	pg4 = ::new ( 'c' ) S::N2;
	pg5 = ::new ( 'd' ) S::N3;
	delete pg5;
	delete pg4;
	delete pg3;
	delete pg2;
	delete pg1;
	::delete p5;
	::delete p4;
	::delete p3;
	::delete p2;
	::delete p1;
    }
    {
	p1 = new X[n];
	p2 = new S[n];
	p3 = new ( vX ) S::N1[n];
	p4 = new ( vX ) S::N2[n];
	p5 = new ( 'a' ) S::N3[n];
	pg1 = ::new X[n];
	pg2 = ::new S[n];
	pg3 = ::new ( 'b' ) S::N1[n];
	pg4 = ::new ( 'c' ) S::N2[n];
	pg5 = ::new ( 'd' ) S::N3[n];
	delete [] pg5;
	delete [] pg4;
	delete [] pg3;
	delete [] pg2;
	delete [] pg1;
	::delete [] p5;
	::delete [] p4;
	::delete [] p3;
	::delete [] p2;
	::delete [] p1;
    }
}

int main()
{
    foo( 10 );
    foo( 0 );
    if( index ) fail(__LINE__);
    _PASS;
}
