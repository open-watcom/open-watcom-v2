struct B1 {
    virtual void foo( int );
    virtual void foo( int, int );
};
struct D1 : B1 {
    void foo( int );		// hides B1::foo( int, int )
};

struct B2 {
    virtual void foo( int );
    virtual void foo( int, int );
};
struct D2 : B2 {
    void foo( int, int );	// hides B2::foo( int )
};

struct B3 {
    virtual void foo( int );
    virtual void foo( int, int );
};
struct D3 : B3 {
    void foo( int ) const;	// hides B3::foo( int ) + B3::foo( int, int )
    void foo( int, int ) const;
};

struct B4 {
    virtual void foo( int );
    virtual void foo( int, int );
};
struct D4 : B4 {
    void foo( char ) const;	// hides B4::foo( int ) + B4::foo( int, int )
    void foo( int, char ) const;
};

struct B5 : B1 {
    int foo;
};
struct D5 : B5 {
    void foo( int );		// hides B1::foo( int, int )
};

struct B6 : B2 {
    int foo;
};
struct D6 : B6 {
    void foo( int, int );	// hides B2::foo( int )
};

struct B7 : B3 {
    int foo;
};
struct D7 : B7 {
    void foo( int ) const;	// hides B3::foo( int ) + B3::foo( int, int )
    void foo( int, int ) const;
};

struct B8 : B4 {
    int foo;
};
struct D8 : B8 {
    void foo( char ) const;	// hides B4::foo( int ) + B4::foo( int, int )
    void foo( int, char ) const;
};

struct B9 {
    virtual void foo( char * );
    virtual void foo( int * );
};
struct D9 : B9 {
    virtual void foo( const char * );	// hides B9::foo( char * )
    virtual void foo( int * );
};
#error last line
