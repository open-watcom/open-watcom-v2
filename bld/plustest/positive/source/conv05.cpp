#include "fail.h"

#if __WATCOM_REVISION__ >= 8
class BASE {
protected:
    char const *s;
public:
    BASE(char const *str);
    char const *strval() const;
};

BASE::BASE(char const *str) : s(str) {
}

char const *BASE::strval() const {
    return s;
}

class DERIV: public BASE {
    int slen;
public:
    DERIV(char const *str);
    DERIV(BASE const &);
    DERIV(DERIV const &d);
    void check( unsigned, unsigned ) const;
};

DERIV::DERIV(char const *str)
    :BASE(str) {
    slen=strlen(str);
}
DERIV::DERIV(DERIV const &d)
    :BASE(d.strval()) {
    slen=strlen(d.strval());
}

DERIV::DERIV(BASE const &ref) : BASE(ref.strval())
{
    slen=strlen(s);
}

void DERIV::check( unsigned c, unsigned line ) const
{
    if( slen != c ) fail(line);
}

int main() {
    BASE base1("the first string");
    BASE base2("the second string");
    BASE base3("a third string");

    DERIV deriv1 = DERIV(base1);
    DERIV deriv2 = (DERIV)base2;
    DERIV deriv3 = base3;

    deriv1.check( 16, __LINE__ );
    deriv2.check( 17, __LINE__ );
    deriv3.check( 14, __LINE__ );
    ( DERIV(base1) ).check( 16, __LINE__ );
    ( (DERIV)base2 ).check( 17, __LINE__ );
    _PASS;
}
#else
ALWAYS_PASS
#endif
