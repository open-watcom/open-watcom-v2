struct V {
    operator int();
    operator float();
    operator double();
    operator char*();
    operator double *();
    int v;
};

struct A : virtual V {
    operator float();
    int a;
};

struct B : V {
    operator double();
    int b;
};

struct C : virtual V {
    operator char *();
    int c;
};

struct D : A, B, C {
    operator int *();
    int d;
};

D x;			// operator int() is ambiguous!

int int1( void )
{
    return x;
}
