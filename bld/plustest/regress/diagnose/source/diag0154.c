struct XX {
    void virtual foo();
    void virtual bar();
};

struct OK0 : XX {
};

struct A {
    virtual void f1( );
    virtual void f2( int ) = 0;
    virtual void f3( int,int ) = 0;
    virtual void f4( int );
    void virtual f5( int,int ) = 0;
    virtual void f6( );
};

struct A1 : virtual A {
    virtual void f3( int,int );
};

struct A2 : virtual A {
    virtual void f2( int );
    void virtual f5( int,int );
};

struct OK1 : A1, A2 {
};

struct B1 : virtual A {
    virtual void f2( int ) = 0;
    virtual void f3( int,int );
    void virtual f5( int,int ) = 0;
};

struct B2 : virtual A {
    virtual void f2( int );
    virtual void f3( int,int ) = 0;
    void virtual f5( int,int );
};

struct BAD1 : B1, B2 {
};

struct BAD2 : BAD1 {
};

struct OK2 : A {
    virtual void f1( );
    virtual void f2( int );
    virtual void f3( int,int );
    virtual void f4( int );
    void virtual f5( int,int );
    virtual void f6( );
};

struct OK3 : A {
    virtual void f2( int );
    virtual void f3( int,int );
    virtual void f4( int );
    void virtual f5( int,int );
    virtual void f6( );
};

struct BAD3 : A {
    virtual void f1( );
    virtual void f3( int,int );
    virtual void f4( int );
    void virtual f5( int,int );
    virtual void f6( );
};

struct BAD4 : A {
    virtual void f1( );
    virtual void f2( int );
    virtual void f4( int );
    void virtual f5( int,int );
    virtual void f6( );
};

struct OK4 : A {
    virtual void f1( );
    virtual void f2( int );
    virtual void f3( int,int );
    void virtual f5( int,int );
    virtual void f6( );
};

struct BAD5 : A {
    virtual void f1( );
    virtual void f2( int );
    virtual void f3( int,int );
    virtual void f4( int );
    virtual void f6( );
};

struct OK5 : A {
    virtual void f1( );
    virtual void f2( int );
    virtual void f3( int,int );
    virtual void f4( int );
    void virtual f5( int,int );
};

struct BAD6 : A {
    virtual void f1( );
    virtual void f2( int ) = 0;
    virtual void f3( int,int ) = 0;
    virtual void f4( int );
    void virtual f5( int,int ) = 0;
    virtual void f6( );
};

struct BAD7 : A {
    virtual void f1( );
    virtual void f2( int ) = 0;
    virtual void f3( int,int ) = 0;
    virtual void f4( int );
    void virtual f5( int,int ) = 0;
    virtual void f6( );
};

struct BAD8 : BAD6, BAD7 {
};

struct BAD9 : virtual A {
    virtual void f1( );
    virtual void f2( int ) = 0;
    virtual void f3( int,int ) = 0;
    virtual void f4( int );
    void virtual f5( int,int ) = 0;
    virtual void f6( );
};

struct BAD10 : virtual A {
    virtual void f1( );
    virtual void f2( int ) = 0;
    virtual void f3( int,int ) = 0;
    virtual void f4( int );
    void virtual f5( int,int ) = 0;
    virtual void f6( );
};

struct BAD11 : BAD9, BAD10 {
};

struct BAD12 : A {
    // all brand new virtual functions
    virtual void f1( char );
    virtual void f2( char,int );
    virtual void f3( char,int,int );
    virtual void f4( char,int );
    void virtual f5( char,int,int );
    virtual void f6( char );
};

struct BAD13 {
    virtual int foo() = 0;
};

struct BAD14 : BAD13 {
    virtual int foo() const;
};

struct BAD15 {
    virtual int foo( int const & ) = 0;
};

struct BAD16 : BAD15 {
    virtual int foo( int & );
};

OK0 x39;
OK1 x40;
BAD1 x41;
OK2 x42;
OK3 x43;
BAD2 x44a;
BAD3 x44;
BAD4 x45;
OK4 x46;
BAD5 x47;
OK5 x48;
BAD6 x49;
BAD7 x50;
BAD8 x51;
BAD9 x52;
BAD10 x53;
BAD11 x54;
BAD12 x54;
BAD13 x55;
BAD14 x56;
BAD15 x57;
BAD16 x58;

OK0 y39;
OK1 y40;
BAD1 y41;
OK2 y42;
OK3 y43;
BAD2 y44a;
BAD3 y44;
BAD4 y45;
OK4 y46;
BAD5 y47;
OK5 y48;
BAD6 y49;
BAD7 y50;
BAD8 y51;
BAD9 y52;
BAD10 y53;
BAD11 y54;
BAD12 y55;
BAD13 y55;
BAD14 y56;
BAD15 y57;
BAD16 y58;

OK0 w39[2];
OK1 w40[2];
BAD1 w41[2];
OK2 w42[2];
OK3 w43[2];
BAD2 w44a[2];
BAD3 w44[2];
BAD4 w45[2];
OK4 w46[2];
BAD5 w47[2];
OK5 w48[2];
BAD6 w49[2];
BAD7 w50[2];
BAD8 w51[2];
BAD9 w52[2];
BAD10 w53[2];
BAD11 w54[2];
BAD12 w55[2];
BAD13 w55[2];
BAD14 w56[2];
BAD15 w57[2];
BAD16 w58[2];
