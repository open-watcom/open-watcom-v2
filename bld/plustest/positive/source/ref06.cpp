#include "fail.h"
#include <string.h>

int count;

struct S {
    int sig;
    S() { ++count; sig = count; }
    S( int ) { ++count; sig = count; }
    S( S const & ) { ++count; sig = count; }
    ~S() { --count; }
    void *operator new( unsigned n )
    {
	void *p = ::new char[n];
	memset( p, 0, n );
	return p;
    }
};

struct T : S {
    T() : S() {}
    ~T() {}
private:
    T( const &r );
};

S &fref()
{
    return *new S;
}

S fobj()
{
    return S();
}

S &vref = *new S;
S vobj;

int main()
{
    if( count != 2 ) fail(__LINE__);
    {
	S &r = fref();
	if( count != 3 ) fail(__LINE__);
	delete &r;
    }
    if( count != 2 ) fail(__LINE__);
    {
	S const &r = fobj();
	if( count != 3 ) fail(__LINE__);
    }
    if( count != 2 ) fail(__LINE__);
    {
	S const &r = S();
	if( count != 3 ) fail(__LINE__);
    }
    if( count != 2 ) fail(__LINE__);
    {
	S const &r = T();
	if( count != 3 ) fail(__LINE__);
    }
    if( count != 2 ) fail(__LINE__);
    {
	S const &r = 1;
	if( count != 3 ) fail(__LINE__);
    }
    if( count != 2 ) fail(__LINE__);
    {
	S const &r = vref;
	if( count != 2 ) fail(__LINE__);
    }
    if( count != 2 ) fail(__LINE__);
    {
	S const &r = vobj;
	if( count != 2 ) fail(__LINE__);
    }
    if( count != 2 ) fail(__LINE__);
    _PASS;
}
