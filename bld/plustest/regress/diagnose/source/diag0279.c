struct S {
    short : 1;
    short a : 1;
    short : 1;
};
short &foo( S *p ) {
    return p->a;            // can't return ref. to bit field
}
short const &poo( S *p ) {
    return p->a;            // can return ref. to const bit field
}
