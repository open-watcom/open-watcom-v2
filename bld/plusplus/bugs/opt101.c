// copy ctors should copy bitfields as an entire unit!
struct S {
    int a : 1;
    int b : 1;
    S();
};

S x;

void foo( S );

void foo()
{
    foo(x);
}

