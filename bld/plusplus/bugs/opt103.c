// ctors for S, T, and W should never be called as "component" ctors
struct S {
    virtual void foo();
    S(){}
};

struct T : S {
    T(){}
};

struct W : T, S {
    W(){}
};

W x;
