struct V {
    operator float();
    operator double();
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

struct D {
    operator int *();
    operator float();
    operator double();
    int d;
};

D x;		// ambiguity between float() or double() if going to int

int int1( void )
{
    return x;
}
