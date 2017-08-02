// all other compilers allow this including 9.5 GA
// exists in 9.5 and 10.0 (AFS)
struct D;
struct B {
    D &foo( B &r )
    {
        return (D&) r;
    }
};
struct D : B {
};


struct E;
struct F;

typedef E E_array[10];
typedef F F_array[13];

E& foop( F& r )
{
    return (E&)r;
}

E_array& poop( F& r )
{
    return (E_array&)r;
}

E_array& goop( F_array& r )
{
    return (E_array&)r;
}

#error This is the only message
