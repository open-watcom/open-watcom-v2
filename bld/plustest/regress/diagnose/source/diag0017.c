struct Q {
    int q;
};
struct W {
    int d;
};
struct T : W {
    int a;
};
class S : T {
    T::a;	// access decl can only have 'protected' or 'public' access
public:
    T::z;	// z not in T
    T::d;	// z not directly in T
    Q::a;	// Q not a base class of S
};
class B {
    public: int a;
    private: int b;
    protected: int c;
};
class D : private B {
public:
    int a;
    B::a;	// conflicts with 'int a'
    B::b;	// granting access
protected:
    B::c;
    B::a;	// reducing access
    int c;	// conflicts with 'B::c'
};
class X {
private:
    f(int);
public:
    f();
};
class Y : private X {
public:
    X::f;
};
