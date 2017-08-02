// page 205 ARM
struct V {
    int x;
    void y(void);
};

struct B : virtual V {
    int x;
    void y(void);
};

struct C : virtual V {
};

struct D : B, C {
    int z;
};

void foo( D *p )
{
    p->x = 1;		// OK B::x dominates V::x
    p->y();		// OK B::y dominates V::y
}
