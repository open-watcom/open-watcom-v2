template< int n, int m >
struct A {
};

template< int n >
struct A< 1, n > {
};

template< int n >
struct A< n, n > {
};


template< class T >
struct B {
};

template< class T, class U >
struct B {
};

template<>
struct C< int > {
};

template< class T >
struct D< T > {
};

template< class T >
struct E {
};

template< class T >
struct E< T > {
};


int main() {
    A< 1, 1 > a;

    return 0;
}
