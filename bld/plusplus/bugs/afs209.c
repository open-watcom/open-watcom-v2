// low priority because we don't know what we should do (AFS)
typedef void F();

F f;

void f() { }

struct A {
    F f;
    const F g;  // What about this one?
};
