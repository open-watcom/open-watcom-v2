int x;

struct S {
    S()
    { s = ++x; }
    ~S()
    { s = ++x; }
    int s;
    int t;
};
typedef S SA[2][3][4];

S (*p)[3][4];

void foo( int x )
{
    p = new S[x][3][4];
    delete [] p;
    p = new SA;
    delete [] p;
}

main()
{
    foo( 1 );
    foo( 2 );
    foo( 0 );
}
