struct A {
    char a;
};
struct B {
    char b0;
    char b1;
};
struct C : A, B {
    char c;
};
struct D : virtual C {
    int d;
};

char B::* p = &B::b1;
char D::* q = p;
char B::* r = &C::b1;           // legal?
