#include "dump.h"

static void good1();
static void good2();
static void good3();

#if __WATCOM_REVISION__ >= 8
// from customer

struct INT64 {
    long lo, hi;

    INT64(long h, long l) { hi = h; lo = l; good1(); }
    operator unsigned int() const
    {
	good2();
	return (unsigned int)  lo;
    }
    friend INT64 operator + (const INT64 &a, const unsigned int &b);
};
  
  
inline INT64 operator + (const INT64 &a, const unsigned int &b)
{
    good3();
    return INT64(a.hi, b);
}
  
int foo(const INT64 &a)
{
    return a + 1u;  /* the error is occuring on this line */
}
#endif

static void good1() {
    GOOD;
}
static void good2() {
    GOOD;
}
static void good3() {
    GOOD;
}

int main() {
#if __WATCOM_REVISION__ >= 8
    INT64 a( 1, 1 );
    CHECK_GOOD(36);
    foo( a );
    CHECK_GOOD(36+39+42+36)
#else
    good1();
    CHECK_GOOD(36);
    good3();
    good1();
    good2();
    CHECK_GOOD(36+39+42+36)
#endif
    return errors != 0;
}
