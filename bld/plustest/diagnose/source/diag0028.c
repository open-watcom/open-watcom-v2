struct C;

struct D {
    C & operator ^ ( int );
};

class C {
    void friend (*fn_ptr)( C * );
    void friend *ptr;
    friend C & D::operator ^ ( int );
    friend C & operator ^ ( C &, int );
    friend void foo( C * );
    int a;
public:
    C();
};

void foo( C *p )
{
    p->a = 1;
}

virtual int bar( int )
{
    return( 0 );
}

virtual int (*p1)( int );
inline int (*p2)( int );
