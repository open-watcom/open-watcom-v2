// access decls for types?
struct B {
    typedef int T;
};
struct D : private B {
    B::T;
    static T x;
};

D::T D::x = 1;
