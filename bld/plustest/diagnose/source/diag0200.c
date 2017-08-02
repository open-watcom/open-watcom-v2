// test array ctor-initializer

struct S {
    int arr[3];
    S();
};

S::S() : arr( 1, 2, 3 )
{
}
