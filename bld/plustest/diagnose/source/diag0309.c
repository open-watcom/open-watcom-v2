/* part of bug 514 - compiler was crashing on redefinition of specialisation */
template < class T >
struct X{
    static const int v = 1;
};

template <>
struct X< int >{
    static const int v = 2;
};

template <>
struct X< signed int >{
    static const int v = 3;
};

template <>
struct X< int >{
    static const int v = 4;
};


int main() {
    return( X< int >::v );
}
