/*
  some members that are not allowed in a union
*/
struct SA {
    SA & operator = ( SA & );
};
struct SC {
    SC();
};
struct SD {
    ~SD();
};

union C {
    static int a;
    virtual void vf1( void );
    void virtual vf2( void );
    extern int x;
    SA needs_assign;
    SC needs_ctor;
    SD needs_dtor;
};
