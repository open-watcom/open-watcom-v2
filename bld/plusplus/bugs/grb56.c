// from Adobe (AFS)
typedef char A64[64], A256[256];

struct S {
    // char [256] -T-> char * -T-> const char *
    S( const char * );
    // no trivial conversions from char [256]
    S( const A64 & );
};

void foo( S const & )
{
}

void main()
{
    A256 name;
    A64 name2;

    foo( name );        // should call S( const char *)
    foo( name2 );       // should be ambiguous
    // unfortunately, "name" and "name2" are typed as "char *" which
    // makes them indistinguishable to the overloader.
    // need to change the typing information that the overloader
    // receives in order to support this functionality.
}
