// Status: compiles
struct A {
    A();
};

struct B {
    B();
    operator A&();
};

A& foo( B& b ) {
    return (A&)b;
}
