struct V {
    operator int();
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

int int2a( void )
{
    return int(x);
}

int int2b( void )
{
    return (int) x;
}

int int3( void )
{
    return x.operator int();
}
