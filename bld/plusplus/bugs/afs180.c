// won't expand T::~T()
// GRB
struct S {
    ~S();
};

template <class T>
    struct xyz {
        T x;
        foo() {
            x.T::~T();
        };
    };

xyz<S> y;

void bar( void ) {
    y.foo();
}
