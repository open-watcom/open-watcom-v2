struct x1 {
    void fi( int );
    typedef int Tint;
    void fi( Tint );            //identical redeclaration
    void fi( const int );               //not distinct
    void fi( volatile int );    //not distinct
    
    void fri( int );
    void fri( int & );          //not distinct
    
    void fpi( int * );
    void fpi( int [] );         //identical redeclaration
    
    void fa( int (*)[10] );
    void fa( int [5][10] );             //identical redeclaration
    
    float fb( char, int );
    double fb( char, int );             // not distinct
};
struct x20 {
    void f( auto void (*)( int   ), auto void (*)( int   ) );
    void f( auto void (*)( int   ), auto void (*)( char  ) );
    void f( auto void (*)( int   ), auto void (*)( float ) );
    void f( auto void (*)( char  ), auto void (*)( int   ) );
    void f( auto void (*)( char  ), auto void (*)( char  ) );
    void f( auto void (*)( char  ), auto void (*)( float ) );
    void f( auto void (*)( float ), auto void (*)( int   ) );
    void f( auto void (*)( float ), auto void (*)( char  ) );
    void f( auto void (*)( float ), auto void (*)( float ) );
    
    static void h( char );
    static void h( int );
    static void h( float );
    
    void g( void )
    {
        f( h, h );
    }
};
struct x40 {
    struct A {};
    struct B : public A {};
    struct C : public B {};
    struct D : public C {};
    
    void bar1( C, float ); void bar1( A, int );   void bar1( B, int );
    void bar2( C, float ); void bar2( B, int );   void bar2( A, int );
    void bar3( A, int );   void bar3( C, float ); void bar3( B, int );
    void bar4( A, int );   void bar4( B, int );   void bar4( C, float );
    void bar5( B, int );   void bar5( C, float ); void bar5( A, int );
    void bar6( B, int );   void bar6( A, int );   void bar6( C, float );
    
    void foo( D d, int i ) {
        bar1( d, i );
        bar2( d, i );
        bar3( d, i );
        bar4( d, i );
        bar5( d, i );
        bar6( d, i );
    }
};
struct x62 {
    class A { public: int a; };
    class B { public: int b; };
    class C : public A, public B { public: int c; };
    
    void f( A * );
    void f( B * );
    
    void g( C *pc )
    {
        f( pc );
    }
};
struct x75 {
    //       A       A
    //        \     /
    //         B   C
    //          \ /
    //       D
    
    class A { public: int a; };
    class B : public A { public: int b; };
    class C : public A { public: int c; };
    class D : public B, public C { public: int d; };
    
    void f( A * );
    void f( char );
    
    void g( D *pd )
    {
        f( pd );        // ambiguous
    }
};
struct x95 {
    //   A
    //    \
    //     B   A
    //      \ /
    //       D
    
    class A { public: int a; };
    class B : public A { public: int b; };
    class C : public A { public: int c; };
    class D : public B, public A { public: int d; };
    
    void f( A * );
    void f( char );
    
    void g( D *pd )
    {
        f( pd );        // ambiguous
    }
};
struct x115 {
    class B {
        public:
            int public_bi;
            operator char();
            operator short();
    };
    
    void f( short );
    void f( char );
    
    void g( B b )
    {
        f( b ); // should be ambiguous
                    // f( b.operator char() ); or
                    // f( b.operator short() );
    }
};
struct x133 {
    class A {
        public:
            int public_ai;
            operator short();
    };
    class B : public A {
        public:
            int public_bi;
            operator char();
    };
    void f( short );
    void f( char );
    void g( B b )
    {
        f( b ); // should be ambiguous
                    // f( b.operator char() ); or
                    // f( b.A::operator short() );
    }
};
struct x153 {
    void fncvpT( float );
    void fncvpT( void * );
    void gncvpT( int const *a, int volatile *b )
    {
        fncvpT( a );    // should fail
        fncvpT( b );    // should fail
    }
};
struct x162 {
    void f( void near *, void far * );
    void f( void far *, void near * );
    
    void g( int *a, int *b )
    {
        f( a, b );
    }
};
struct x171 {
    class A {
        public:
            int ai;
            char ac;
    };
    class B : public A {
        public:
            int bi;
            char bc;
    };
    
    void f( int );
    void f( int A:: *pai );
    void f( char );
    void f( char A:: *pac );
    
    void g( int B:: *pbi, char B:: *pbc )
    {
        f( pbi );
        f( pbc );
    }
};
template<class T> void x194( T, T );

void x196( int i, char c ) {
    x194( i, i );
    x194( c, c );
    x194( i, c );
}
struct X201 {
    // straightforward case
    struct S {
        operator = ( S const & );
    };
    struct T : public S {
    };
    void g( T a, T const b ) {
        b = a;          // generated op=() does not respect const-ness of 'b'
    }
    
    // optimized case
    struct Q { };
    void f( Q a, Q const b ) {
        b = a;          // generated op=() is optimized to a bit-wise copy
                            // that does not respect const-ness of 'b'
    }
};
struct x219 {
    struct A {
        void f( int );
        void f( char ) const;
    };
    void g( A const &a ) {
        a.f( 1 );               // should pick f(int) and then complain
                            // because can't invoke non-const member function
                            // with const this pointer
    }
    
    // test overloading with const volatile this pointer across derivations
    // in each of the following 'foo_..' functions, the selection of
    // S::operator[] is driven by the modifiers on the 'this' parameter
    
    struct S {
                       int & operator[](int i)               ;
        const          int & operator[](int i) const         ;
              volatile int & operator[](int i)       volatile;
        const volatile int & operator[](int i) const volatile;
    };
    struct T : S {
    };
    
    int &foo_c( S const &x )
    {
        return( x[1] ); // cannot strip const
    }
    
    int &foo_v( S volatile &y )
    {
        return( y[1] ); // cannot strip volatile
    }
    
    int &foo_cv( S const volatile &z )
    {
        return( z[1] ); // cannot strip const or volatile
    }
    
    int &foo_c( T const &x )
    {
        return( x[1] ); // cannot strip const
    }
    
    int &foo_v( T volatile &y )
    {
        return( y[1] ); // cannot strip volatile
    }
    
    int &foo_cv( T const volatile &z )
    {
        return( z[1] ); // cannot strip const or volatile
    }
};
