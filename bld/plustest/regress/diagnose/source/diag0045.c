// this test case should not give an error.  See 13.3.3.2 paragraph 4.
// because B is derived from P2 and P2 is derived from V, conversion
// from B to P2 is preferred over conversion from B to V.

// Similarly, because B is derived from UV and UV is derived from V, conversion
// from B to UV is preferred over conversion from B to V

// Similarly, because B is derived from P2 and P2 is derived from UV, conversion
// from B to P2 is better than conversion from B to UV

// So conversion from B to UV is better than B to V
// but conversion from B to P2 is better than conversion from B to UV

// So, the best conversion is from B to P2 so no ambiguity

struct V {
    int : 2;
};

struct UV : public virtual V {
    int : 2;
};

struct RV : public virtual V {
    friend class RM;
    int : 2;
};

struct OV : public virtual V {
    int : 2;
};

struct UM : public UV, public RV, public OV {
    int : 2;
};

struct RM : public UV, public RV, public OV {
    int : 2;
    void foo( void );
};

struct OM : public UV, public RV, public OV {
    int : 2;
};

struct D1 : virtual OM {
    int : 2;
    void foo( void );
};

struct P1 : virtual OM {
};

struct D2 : P1 {
};

struct P2 : virtual OM {
};

struct P3 : P2 {
};

struct D3 : P3 {
};

struct B : public D1, D2, D3 {
    void foo( void );
};

void bar( UV * );
void bar( P2 * );
void bar( V * );

void foo( B *p )
{
    bar( p );
}
