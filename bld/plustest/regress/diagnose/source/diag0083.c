struct S {
    int a;
};
void foo( S *p )
{
    delete [p->a+1] p;
}
template <class T = int> class Z8;
template <class T = int()> class Z9;
template <> class Empty;
template <class T, int x> class Q foo();
class X;
template <
class T,
int size,
double d,
int *q,
int X::*,
void (z)(),
void (*err)(),
int T::*
> class C;

template <class T> class S;
template <class T> class W;
template <class T> class N {
    template <class Q> class S;
};
class Z {
    template <class T> class N {
	template <class Q> class S;
    };
};
