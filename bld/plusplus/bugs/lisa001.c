class X {
    public:
    static void f();
    void f();                  // ill-formed
    void f() const;            // ill-formed
    void f() const volatile;   // ill-formed
};

main()
{
    X x;

    x.f();
}
