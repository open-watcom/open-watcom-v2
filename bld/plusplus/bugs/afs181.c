// op= isn't found
struct S {
    int a;
};

S & (S::* mp)( S const & ) = &S::operator=;
