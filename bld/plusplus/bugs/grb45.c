// operator overloading problem
struct S {
    operator unsigned int();
};

int foo( S &x )
{
    return x == 0;              // ambiguous: overloader can't choose between
                                //            two stdops cases...
}
