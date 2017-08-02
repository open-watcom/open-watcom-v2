class C {
public:
    int a;
};
struct S {
    int a;
};

struct : C {	/* nameless struct with base classes allowed? */
    int q;
} *p;

struct OK : C {
};

class BAD : S {
};

void foo( OK *p, BAD *q )
{
    S *ps;

    ps = q;
    ps = (S*) q;
    p->a = 1;
    q->a = 2;
}

struct U1;

void foo( U1 *p )
{
    p->x = 1;
}

struct U2;

void foo( U2 &r )
{
    r.z = 1;
}
