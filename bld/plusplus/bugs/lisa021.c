// from David Brandow and Ivan, Optima++ bug
// now overload\oper01.cpp
// Status: bug fixed in 11.0

enum MMIdx { NULLIDX = 0 };

struct WString {
    WString( int i );
    friend WString operator+( const char * a, const WString & b );
};

void main()
{
    const char *        s;
    MMIdx               idx = NULLIDX;

    s = s + idx;
}
