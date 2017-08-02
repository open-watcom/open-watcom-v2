struct PU {
    int a;
};
struct PO {
    int b;
};
struct PI {
    int c;
};
struct D : public PU, protected PO, private PI {
    int d;
};
struct DPI : private D {
    D::a;
    D::b;
    D::c;
};
struct DPO : protected D {
    D::a;
    D::b;
    D::c;
};
