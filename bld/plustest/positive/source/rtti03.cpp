#include "fail.h"

#ifdef __WATCOM_RTTI__

#pragma warning 807 10
class A { virtual void f(){}; int a; };
class B { virtual void g(){}; int b; };
class D : public virtual A, private B { int d; };
void g() {
    D   d;
    B*  bp = (B*)&d;  // cast needed to break protection
    A*  ap = &d;      // public derivation, no cast needed
    D&  dr = dynamic_cast<D&>(*bp);  // succeeds
    if( &dr != &d ) _fail;
    ap = dynamic_cast<A*>(bp);       // succeeds
    if( ap != (A*)&d ) _fail;
    bp = dynamic_cast<B*>(ap);       // fails
    if( bp != NULL ) _fail;
    ap = dynamic_cast<A*>(&dr);      // succeeds
    if( ap != (A*)&d ) _fail;
    bp = dynamic_cast<B*>(&dr);      // fails
    if( bp != NULL ) _fail;
}
class E : public D , public B { int e; };
class F : public E, public D { int f; };
void h() {
    F   f;
    A*  ap  = &f;                    // succeeds: finds unique A
    D*  dp  = dynamic_cast<D*>(ap);  // fails: yields 0
			       // f has two D sub-objects
    if( dp != NULL ) _fail;
    E*  ep1 = dynamic_cast<E*>(ap);  // succeeds
    if( ep1 == NULL ) _fail;
    if( ep1 != (E*)&f ) _fail;
}
namespace X {
struct B {};
struct D : B {};
B * foo(D* dp)
{
    B*  bp = dynamic_cast<B*>(dp);  // equivalent to B* bp = dp;
    return bp;
}
}
int main() {
    try {
	if( X::foo( new X::D ) == NULL ) _fail;
	g();
	h();
    } catch(...) {
	_fail;
    }
    _PASS;
}
#else
ALWAYS_PASS
#endif
