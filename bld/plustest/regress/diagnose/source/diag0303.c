template< int i1, int i2 = 0, int i3 >
struct A {
};

template< int i1, int i2 = 0, class T >
struct B {
};

template< int i1, int i2 = 0, int i3 = i4 >
struct C {
};

template< class T = C >
struct D {
};

template< class T = test >
struct E {
};


int main() {
    C<0> c;
    D<> d;
    E<> e;

    return 0;
}
