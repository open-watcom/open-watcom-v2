// Abstract classes are ignored for throw conversions

struct Abstract
{
    Abstract();
    virtual int goop() = 0;
protected:
    ~Abstract();
    Abstract( Abstract const & );
};

struct Concrete
{
    Concrete();
protected:
    ~Concrete();
    Concrete( Concrete const & );
};

struct Actual :public Abstract, public Concrete
{
    int goop();
    Actual();
    ~Actual();
    Actual( Actual const & );
};

struct Virtual :public virtual Abstract, public virtual Concrete
{
    int goop();
    Virtual();
    ~Virtual();
    Virtual( Virtual const & );
};

void throw_actual( Actual const &a )
{
    throw a;
}

void throw_virtual( Virtual const &v )
{
    throw v;
}

extern void fun();

void catch_1()
{
    try {
        fun();
    } catch( Abstract & ) {
        fun();
    } catch( Concrete & ) {
        fun();
    } catch( Virtual & ) {
        fun();
    } catch( Actual & ) {
        fun();
    } catch( ... ) {
        fun();
    }
}

void catch_1_ok()
{
    try {
        fun();
    } catch( Virtual & ) {
        fun();
    } catch( Actual & ) {
        fun();
    } catch( ... ) {
        fun();
    }
}

void catch_2()
{
    try {
        fun();
    } catch( Abstract ) {
        fun();
    } catch( Concrete ) {
        fun();
    } catch( Virtual ) {
        fun();
    } catch( Actual ) {
        fun();
    } catch( ... ) {
        fun();
    }
}

void catch_2_ok()
{
    try {
        fun();
    } catch( Virtual ) {
        fun();
    } catch( Actual ) {
        fun();
    } catch( ... ) {
        fun();
    }
}


struct Ambig
{
    Ambig();
    Ambig( Ambig const & );
    ~Ambig();
};

struct Intermediate : public Ambig
{
    Intermediate();
    ~Intermediate();
    Intermediate( Intermediate const & );
};

struct Derived : public Intermediate, public Ambig
{
    Derived();
    ~Derived();
    Derived( Derived const & );
};

void throw_derived( Derived const &d )
{
    throw d;        // no error: Ambig is ignored
}

void catch_3()
{
    try {
        fun();
    } catch( Ambig & ) {
        fun();
    } catch( Intermediate & ) {
        fun();
    } catch( Derived & ) {
        fun();
    } catch( ... ) {
        fun();
    }
}

void catch_3_err()
{
    try {
        fun();
    } catch( Intermediate & ) {
        fun();
    } catch( Derived & ) {
        fun();
    } catch( ... ) {
        fun();
    }
}

void catch_3_ok()
{
    try {
        fun();
    } catch( Derived & ) {
        fun();
    } catch( ... ) {
        fun();
    }
}

void catch_4()
{
    try {
        fun();
    } catch( Ambig ) {
        fun();
    } catch( Intermediate ) {
        fun();
    } catch( Derived ) {
        fun();
    } catch( ... ) {
        fun();
    }
}

void catch_4_err()
{
    try {
        fun();
    } catch( Intermediate ) {
        fun();
    } catch( Derived ) {
        fun();
    } catch( ... ) {
        fun();
    }
}

void catch_4_ok()
{
    try {
        fun();
    } catch( Derived ) {
        fun();
    } catch( ... ) {
        fun();
    }
}
