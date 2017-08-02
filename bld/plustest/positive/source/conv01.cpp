#include "fail.h"

unsigned called;

template <class T>
    struct A {
	operator const T &() const;
	static const T r;
	A() {}
    };
template <class T>
    A<T>::operator const T &() const
    {
	if( called != 0 ) fail(__LINE__);
	++called;
	return r;
    }
template <class T>
    const T A<T>::r = 0;

template <class T>
    struct OP {
	static void op(const T &v)
	{
	    if( called != 1 ) fail(__LINE__);
	    ++called;
	}
    };

template <class T, class OP>
    struct Imp {
	void f(const T &t) {
	    if( called != 0 ) fail(__LINE__);
	    OP::op(t);
	    if( called != 2 ) fail(__LINE__);
	    ++called;
	}
    };

int main()
{
    typedef void *P;
    Imp< A<P>, OP<P> >  var;
    A<P> p;

    var.f( p );
    if( called != 3 ) fail(__LINE__);
    _PASS;
}
