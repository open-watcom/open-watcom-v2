// two errors are reported for the conversion (also happens with 9.5)
// no negative test added
struct V {
};
struct D : virtual V {
};

struct Z {
};
struct Q : virtual Z, virtual V {
};

typedef void (D::* mpD)( void );
typedef void (Q::* mpQ)( void );

mpQ conv( mpD x )
{
    return x;
}
