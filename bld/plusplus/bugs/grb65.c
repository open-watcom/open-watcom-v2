    struct S {
        int     a;
        long    b;
    };

    class C {

    public:
        S       getS();
        void    setS(S* sp);
        void    tryme();
        void    doSomething();

    private:
        S       s_;
    };


    inline S C::getS()
    {
        return s_;
    }

    inline void C::setS(S* sp)
    {
        s_ = *sp;
    }

    void C::tryme()
    {
    // leaves a static var with no reference to it
        S s = getS();
        doSomething();
        setS(&s);
    }
