// page faults C7
struct S {
    S(int);
    operator int();
    int a;
};

S fn( int b, int i, S s )
{
    return b ? i : s;
}
