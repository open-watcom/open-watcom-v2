/* MetaWare gets this right; Borland and MS doesn't */
struct VV {
    int v;
};

struct MM : private virtual VV {
    VV::v;
};

struct BB : private MM, private virtual VV {
    VV::v;
};

void ok( BB *p )
{
    p->v = 1;
}

void not_ok( BB *p )
{
    p->MM::v = 1;
}
