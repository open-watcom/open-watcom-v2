template <class T>
    struct S;

S<int> *x;

template <class T>
    struct S {
	int a;
    };

void foo()
{
    x->a = 1;
}
