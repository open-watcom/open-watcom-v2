struct V {
    operator int ();
};

struct A : virtual V {
};

struct B : V {
};

struct C : A, B {
    operator int *();
};

C x;

int foo( void )
{
    return x;
}
