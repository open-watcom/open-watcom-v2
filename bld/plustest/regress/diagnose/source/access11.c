struct M {
    int a;
    static int b;
};

struct N : M {
    int a;
};

struct Q : N, M {
};

int foo( Q*p )
{
    int i;

    i = p->a;		// ambiguous? N::a dominates M::a?
    i += p->M::a;	// ambiguous
    i += p->N::a;	// OK
    i += p->M::b;	// OK
    return i;
}
