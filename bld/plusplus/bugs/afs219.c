// MFC wants to use nested class names
// GRB
struct A {
    struct B {
    };
};
typedef A::B A__B;
struct C {
    friend class A__B;      // this works
    friend class A::B;      // this doesn't
};
