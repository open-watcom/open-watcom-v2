class X
{
    public:
        X(){}
        void fn(){}
};

void foo()
{
        new X()->fn();          // should be a syntax error (can't shift '->')
}
