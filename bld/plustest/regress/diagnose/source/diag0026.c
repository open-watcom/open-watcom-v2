struct HOME {
    static friend struct A;
    friend int;
    virtual struct B friend;
    friend struct C { int a; };
};
