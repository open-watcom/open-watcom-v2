// runtime library bug given to me as bug10.c from Anthony.
// now positive\over11.c
// Status: bug fixed
struct XS {
    XS( char * );
    XS( XS const & );
};

struct E {
    E( XS const & );
    E( E const & );
};

struct D : E {
    D();
};
//#pragma on(dump_rank);

/* overloader says E("asdf") is ambiguous
    should be:
        E( XS const & = XS( char * = "asdf" ) )
        E( E const & = E( ??? ) )       shouldn't be close
*/
D::D() : E("asdf") {
}
