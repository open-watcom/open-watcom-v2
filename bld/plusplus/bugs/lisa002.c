class X {
    public:
    void f( int );
    void f( const int );
};

void X::f( int a )
{
}

main()
{
    X x;

    x.f(1);
}
