// warning about the pure virtual destructor should be deferred
// to see if a definition is provided
struct A {
    virtual void foo() = 0;
    virtual ~A() = 0;
    virtual ~A(){}
};
