template <class T>
    struct Q {
	T operator ->();  // no warn
	int q;
    };

struct S {
    void * operator ->();  // OK to warn
    int q;
};
struct DS : S {
};
struct B {};
struct T1 {
    S &operator ->();
};
struct T2 {
    DS &operator ->();
};
struct T3 {
    B &operator ->();
};
struct U;
struct W {
    U &operator ->();
};

void foo( S &r, T2 &q, W &z )
{
    Q<void *> x;
    r->q = 1; // diagnose on use not declaration (may be a template class!)
    q->q = 2;
    z->u = 3;
    x->q = 1;
}
